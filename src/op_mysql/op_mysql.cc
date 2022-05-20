/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include "op_mysql.h"
#include "error.h"
#include "stdlib.h"
#include "string.h"
#include <sys/stat.h>
#ifdef ZETTA_LIB_COMPILER
#include "tool_func/tool_func.h"
#else
#include "zettalib/tool_func.h"
#endif
#include <memory>
#include <sys/types.h>
#include <vector>

using namespace kunlun;
using namespace std;

bool MysqlResult::Parse(MYSQL_RES *raw_mysql_res) {
  raw_mysql_res_ = raw_mysql_res;
  if (!raw_mysql_res_) {
    setErr("Invalid MySQL result handler");
    return false;
  }

  // fill column_index_map_ index_column_map_
  fields_num_ = mysql_num_fields(raw_mysql_res_);
  MYSQL_FIELD *fields = mysql_fetch_fields(raw_mysql_res_);
  for (unsigned int i = 0; i < fields_num_; i++) {
    column_index_map_[fields[i].name] = i;
    index_column_map_[i] = fields[i].name;
  }

  // fill result_vec_
  MYSQL_ROW mysql_raw_row;
  while ((mysql_raw_row = mysql_fetch_row(raw_mysql_res_)) != nullptr) {
    unsigned long *length_array = mysql_fetch_lengths(raw_mysql_res_);
    MysqlResRow *row = new MysqlResRow(column_index_map_);
    row->initByMysqlRawRes(mysql_raw_row, length_array, fields_num_);
    result_vec_.push_back(row);
  }

  // mysql_free_result(raw_mysql_res_);
  return true;
}

void MysqlResult::Clean() {
  for (unsigned int i = 0; i < result_vec_.size(); i++) {
    delete (result_vec_[i]);
  }
  result_vec_.clear();
  raw_mysql_res_ = nullptr;
  column_index_map_.clear();
  index_column_map_.clear();
  fields_num_ = 0;
  return;
}
void MysqlConnection::Reconnect() {
  Close();
  Connect();
}

void MysqlConnection::Close() {
  if (mysql_raw_ != nullptr) {
    mysql_close(mysql_raw_);
    delete mysql_raw_;
    mysql_raw_ = nullptr;
  }
  return;
}
bool MysqlConnection::Connect() {
  ENUM_SQL_CONNECT_TYPE connect_type = mysql_connection_option_.connect_type;
  if (connect_type == TCP_CONNECTION) {
    return ConnectImplByTcp();
  } else if (connect_type == UNIX_DOMAIN_CONNECTION) {
    return ConnectImplByUnix();
  }
  setErr("Unknown Connect Type, Tcp or Unix-domain is legal");
  return false;
}
bool MysqlConnection::SetAutoCommit() {
  char set_autocommit[1024] = {'\0'};
  if (mysql_connection_option_.autocommit) {
    sprintf(set_autocommit, "set session autocommit = 1");
  } else {
    sprintf(set_autocommit, "set session autocommit = 0");
  }
  MysqlResult res;
  int ret = ExcuteQuery(set_autocommit, &res, false);
  return ret == 0;
}

bool MysqlConnection::CheckIsConnected() {
  if (mysql_raw_ != nullptr) {
    return true;
  }
  if (reconnect_support_) {
    return Connect();
  }
  setErr("MySQL connection is not established and reconnect is disabled");
  return false;
}

// sql_stmt: select / set / DDL ,return 0 means success, other than failed
//           result_set hold the retrived data
// sql_stmt: update / delete / insert , return >0 , 0 , <0 respectively,
// successfuly, no effect or  failed
//
// retry: will do the query one more time if failed. Be careful when the query
//        is in a active transaction context, retry may cause previous statment
//        rollback and start another new trx to do the current query, this will
//        cause the unexpected behavior. so set 'force_retry' to false if you do
//        not excactlly understand what happend inside
//
// This function support reconnect if set the flag
//
int MysqlConnection::ExcuteQuery(const char *sql_stmt, MysqlResult *result_set,
                                 bool force_retry) {
#define QUERY_FAILD -1

  // clean the result_set
  result_set->Clean();

  // reset errno
  last_errno_ = 0;

  // if reconnect flag is set, will do connect()
  if (!CheckIsConnected()) {
    return QUERY_FAILD;
  }

  // do the query
  int ret = mysql_query(mysql_raw_, sql_stmt);

  if (ret == 0) {
    // mysql_query() successfully
    MYSQL_RES *query_result = nullptr;
    query_result = mysql_use_result(mysql_raw_);

    unsigned long affect_rows = 0;
    if (query_result == nullptr) {
      // maybe the statement dose not generate result set
      // insert/update/delete/set ..
      affect_rows = mysql_affected_rows(mysql_raw_);
      if (affect_rows == (uint64_t)~0) {
        // error occour
        last_errno_ = mysql_errno(mysql_raw_);
        setErr("execute query failed: %s, error number: %d, sql: %s ",
               mysql_error(mysql_raw_), last_errno_, sql_stmt);
        return QUERY_FAILD;
      }
      return affect_rows;
    } else {
      bool ret = result_set->Parse(query_result);
      mysql_free_result(query_result);
      if (!ret) {
        setErr("%s", result_set->getErr());
        return QUERY_FAILD;
      }
      return 0;
    }
  }
  // mysql_query() failed
  last_errno_ = mysql_errno(mysql_raw_);
  if (last_errno_ == CR_SERVER_GONE_ERROR || last_errno_ == CR_SERVER_LOST) {
    if (force_retry) {
      Reconnect();
      // we set 'force_retry' to false, only requery once
      return ExcuteQuery(sql_stmt, result_set, false);
    }
    Close();
    setErr("execute query failed [this lead to connection closed]: %s, error "
           "number: %d, sql: %s",
           mysql_error(mysql_raw_), last_errno_, sql_stmt);
    return QUERY_FAILD;
  } else if (last_errno_ == CR_COMMANDS_OUT_OF_SYNC ||
             last_errno_ == CR_UNKNOWN_ERROR) {
    // close this connect immedietlly
    Close();
    setErr("execute query failed [this lead to connection closed]: %s, error "
           "number: %d, sql: %s",
           mysql_error(mysql_raw_), last_errno_, sql_stmt);
    return QUERY_FAILD;
  } else if (last_errno_ == ER_DUP_ENTRY) {
    // here we treat duplicate key as affected_num == 0
    setErr("execute query failed: %s, error number: %d, sql: %s ",
           mysql_error(mysql_raw_), last_errno_, sql_stmt);
    return 0;
  } else {
    // normall error
    setErr("execute query failed: %s, error number: %d, sql: %s ",
           mysql_error(mysql_raw_), last_errno_, sql_stmt);
    return QUERY_FAILD;
  }
  // can not reach here
  return QUERY_FAILD;
}

bool MysqlConnection::ConnectImplByTcp() {
  if (mysql_raw_) {
    setErr("connection to mysql already established");
    return false;
  }

  // TODO: connection option is legal

  mysql_raw_ = new MYSQL;
  mysql_init(mysql_raw_);

  // set options
  // connect timeout
  mysql_options(
      mysql_raw_, MYSQL_OPT_CONNECT_TIMEOUT,
      (const void *)(&(mysql_connection_option_.connect_timeout_sec)));
  // read/write timeout
  mysql_options(mysql_raw_, MYSQL_OPT_READ_TIMEOUT,
                (const void *)(&(mysql_connection_option_.timeout_sec)));
  mysql_options(mysql_raw_, MYSQL_OPT_WRITE_TIMEOUT,
                (const void *)(&(mysql_connection_option_.timeout_sec)));
  // charset
  mysql_options(mysql_raw_, MYSQL_SET_CHARSET_NAME,
                mysql_connection_option_.charset.c_str());
  mysql_options(mysql_raw_, MYSQL_DEFAULT_AUTH, "mysql_native_password");

  // do realconnect()
  MysqlConnectionOption op = mysql_connection_option_;
  if (nullptr ==
      mysql_real_connect(mysql_raw_, op.ip.c_str(), op.user.c_str(),
                         op.password.c_str(),
                         op.database.empty() ? op.database.c_str() : nullptr,
                         op.port_num, NULL, 0)) {
    last_errno_ = mysql_errno(mysql_raw_);
    setErr("mysql_real_connect faild, Errno: %d, info: %s", last_errno_,
           mysql_error(mysql_raw_));
    Close();
    return false;
  }

  // connect successfully
  // set database if exists
  if (!op.database.empty() &&
      mysql_select_db(mysql_raw_, op.database.c_str()) != 0) {
    last_errno_ = mysql_errno(mysql_raw_);
    setErr("set connection default database [%s] faild, Errno: %d, info: %s",
           op.database.c_str(), last_errno_, mysql_error(mysql_raw_));
    Close();
    return false;
  }
  return SetAutoCommit();
}
bool MysqlConnection::ConnectImplByUnix() {
  if (mysql_raw_) {
    setErr("connection to mysql already established");
    return false;
  }

  // TODO: connection option is legal

  mysql_raw_ = new MYSQL;
  mysql_init(mysql_raw_);

  // set options
  // connect timeout
  mysql_options(
      mysql_raw_, MYSQL_OPT_CONNECT_TIMEOUT,
      (const void *)(&(mysql_connection_option_.connect_timeout_sec)));
  // read/write timeout
  mysql_options(mysql_raw_, MYSQL_OPT_READ_TIMEOUT,
                (const void *)(&(mysql_connection_option_.timeout_sec)));
  mysql_options(mysql_raw_, MYSQL_OPT_WRITE_TIMEOUT,
                (const void *)(&(mysql_connection_option_.timeout_sec)));
  // charset
  mysql_options(mysql_raw_, MYSQL_SET_CHARSET_NAME,
                mysql_connection_option_.charset.c_str());

  // do realconnect()
  MysqlConnectionOption op = mysql_connection_option_;
  if (nullptr ==
      mysql_real_connect(mysql_raw_, NULL, op.user.c_str(), op.password.c_str(),
                         op.database.empty() ? op.database.c_str() : nullptr, 0,
                         op.file_path.c_str(), 0)) {
    last_errno_ = mysql_errno(mysql_raw_);
    setErr("mysql_real_connect faild, Errno: %d, info: %s", last_errno_,
           mysql_error(mysql_raw_));
    Close();
    return false;
  }

  // connect successfully
  // set database if exists
  if (!op.database.empty() &&
      mysql_select_db(mysql_raw_, op.database.c_str()) != 0) {
    last_errno_ = mysql_errno(mysql_raw_);
    setErr("set connection default database [%s] faild, Errno: %d, info: %s",
           op.database.c_str(), last_errno_, mysql_error(mysql_raw_));
    Close();
    return false;
  }
  return SetAutoCommit();
}

bool MgrMysqlConnection::parseSeeds(std::vector<std::string> &container) {

  if (user_.empty()) {
    setErr("username is empty");
    return false;
  }

  if (passwd_.empty()) {
    setErr("password is empty");
    return false;
  }

  auto iter = container.begin();
  for (; iter != container.end(); iter++) {
    auto item = (*iter);
    auto ip_port = kunlun::StringTokenize(item, ":");
    if (ip_port.size() != 2) {
      setErr("group seeds: %s misformate", group_seeds_.c_str());
      return false;
    }
    std::unique_ptr<MysqlConnectionOption> ptr(new MysqlConnectionOption);
    ptr->ip = ip_port[0];       // ip
    ptr->port_str = ip_port[1]; // port
    ptr->port_num = (unsigned int)(::atoi(ip_port[1].c_str()));
    ptr->user = user_;
    ptr->password = passwd_;

    auto i = mgr_member_options_.find(*iter);
    if (i == mgr_member_options_.end()) {
      mgr_member_options_[*iter] = std::move(ptr);
    } else {
      setErr("mgr member in the group seeds: %s has the overlap hostaddr",
             group_seeds_.c_str());
      return false;
    }
  }
  return true;
}

bool MgrMysqlConnection::isValidPrimary(MysqlConnection *conn) {
  char sql[4096] = {'\0'};
  snprintf(sql, 4096,
           "select MEMBER_HOST, MEMBER_PORT from "
           "performance_schema.replication_group_members where MEMBER_ROLE = "
           "'PRIMARY' and MEMBER_STATE = 'ONLINE");
  kunlun::MysqlResult result;
  int ret = conn->ExcuteQuery(sql, &result);
  // only support single master mgr mode
  if (ret != 0 || result.GetResultLinesNum() != 1) {
    return false;
  }
  return true;
}

bool MgrMysqlConnection::refreshMaster() {

  int master_found = false;
  auto iter = mgr_member_conn_.begin();
  for (; iter != mgr_member_conn_.end(); ++iter) {
    MysqlConnection *conn = iter->second.get();
    if ((conn->CheckIsConnected()) && // will do reconnect
        isValidPrimary(conn)) {
      master_found = true;
      master_ = conn;
      break;
    }
  }
  if (!master_found) {
    setErr("no avilable master in current group");
  }
  return master_found;
}
MysqlConnection *MgrMysqlConnection::get_master() { return master_; }

bool MgrMysqlConnection::init() {
  if (group_seeds_.empty()) {
    setErr("group seeds is empty");
    return false;
  }
  auto container = kunlun::StringTokenize(group_seeds_, ",");

  if (!parseSeeds(container)) {
    return false;
  }

  // init member mysql connection handler
  auto iter = mgr_member_options_.begin();
  for (; iter != mgr_member_options_.end(); iter++) {
    MysqlConnectionOption *option = (iter->second).get();
    std::unique_ptr<MysqlConnection> ptr(new MysqlConnection(*option));
    ptr->Connect();
    mgr_member_conn_[iter->first] = std::move(ptr);
  }

  return refreshMaster();
}

bool StorageShardConnection::isValidPrimary(MysqlConnection *conn) {
  if (!(conn->CheckIsConnected())) { // will do reconnect
    setErr("current connection is not connected");
    return false;
  }

  char sql_buff[1024] = {'\0'};
  snprintf(sql_buff, 1024, "set autocommit = 0");
  kunlun::MysqlResult result;
  int ret = conn->ExcuteQuery(sql_buff, &result);
  if (ret != 0) {
    setErr("error info :%s", conn->getErr());
    return false;
  }
  bzero((void *)sql_buff, 1024);
  result.Clean();

  // begin
  snprintf(sql_buff, 1024, "begin");
  ret = conn->ExcuteQuery(sql_buff, &result);
  if (ret != 0) {
    setErr("error info :%s", conn->getErr());
    return false;
  }
  bzero((void *)sql_buff, 1024);
  result.Clean();

  // insert tmp value to the kunlun_sysdb.cluster_info
  snprintf(sql_buff, 1024,
           "insert into kunlun_sysdb.cluster_info (id,cluster_name,shard_name) "
           "values (100,'tmpname','tmpname')");
  ret = conn->ExcuteQuery(sql_buff, &result);
  if (ret <= 0) {
    setErr("error info :%s", conn->getErr());

    conn->Close();
    return false;
  }

  // rollback
  snprintf(sql_buff, 1024, "rollback");
  ret = conn->ExcuteQuery(sql_buff, &result);
  if (ret != 0) {
    setErr("error info :%s", conn->getErr());

    conn->Close();
    return false;
  }

  return true;
}
