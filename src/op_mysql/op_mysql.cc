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
#include <sys/types.h>

using namespace kunlun;
using namespace std;

bool MysqlCfgItem::PasrseItem(const char *item) {

  const char *location = strstr(item, "=");

  if (location == nullptr) {
    is_boolean_ = true;
    key_ = item;
    value_ = "";
    if (key_.empty()) {
      setErr("key is empty");
      return false;
    }
  } else {
    is_boolean_ = false;
    size_t prefix_len = (size_t)(location - item);
    key_.assign(item, prefix_len);
    key_ = kunlun::trim(key_);

    value_.assign(item + prefix_len + 1, strlen(item) - prefix_len);
    value_ = kunlun::trim(value_);
    value_ = kunlun::trim(value_, "'\"");

    if (key_.empty()) {
      setErr("key is empty");
      return false;
    }
    if (value_.empty()) {
      setErr("value is empty");
      return false;
    }
  }
  return true;
}
bool MysqlCfgItem::PasrseItem(const std::string &item_str) {

  const char *item = item_str.c_str();
  const char *location = strstr(item, "=");

  if (location == nullptr) {
    is_boolean_ = true;
    key_ = item;
    value_ = "";
    if (key_.empty()) {
      setErr("key is empty");
      return false;
    }
  } else {
    is_boolean_ = false;
    size_t prefix_len = (size_t)(location - item);
    key_.assign(item, prefix_len);
    key_ = kunlun::trim(key_);

    value_.assign(item + prefix_len + 1, strlen(item) - prefix_len);
    value_ = kunlun::trim(value_);
    value_ = kunlun::trim(value_, "'\"");

    if (key_.empty()) {
      setErr("key is empty");
      return false;
    }
    if (value_.empty()) {
      setErr("value is empty");
      return false;
    }
  }
  return true;
}
const std::string &MysqlCfgItem::get_key() const { return key_; }
const std::string &MysqlCfgItem::get_value() const { return value_; }

bool MysqlCfgFileParser::Parse() {
  struct stat sb;
  int ret = stat(etc_file_path_.c_str(), &sb);
  if (ret) {
    setErr("Etc file path: %s is invalid, %s", etc_file_path_.c_str(),
           strerror(errno));
    return false;
  }
  FILE *fp = fopen(etc_file_path_.c_str(), "r");
  if (fp == nullptr) {
    setErr("fopen() etc file %s failed, %s", etc_file_path_.c_str(),
           strerror(errno));
    return false;
  }
  char line_buff[4096] = {'\0'};
  while (line_buff == fgets(line_buff, 4096, fp)) {
    std::string tmp = line_buff;
    tmp = kunlun::trim(tmp);
    if (tmp.empty()) {
      // empty line
      continue;
    }
    size_t pos1 = tmp.find_first_of("#");
    size_t pos2 = tmp.find_first_of("[");
    if (pos1 == 0 || pos2 == 0) {
      // Comments or config block identifier
      bzero((void *)(line_buff), 4096);
      continue;
    }
    MysqlCfgItem item;
    bool ret = item.PasrseItem(tmp);
    if (!ret) {
      setErr("%s", item.getErr());
      fclose(fp);
      return false;
    }
    cfg_item_map_[item.get_key()] = item.get_value();
    bzero((void *)(line_buff), 4096);
  }
  if (cfg_item_map_.size() == 0) {
    setErr("There is no avilable config item in the given etc file");
    fclose(fp);
    return false;
  }
  fclose(fp);
  return true;
}

bool MysqlCfgFileParser::Has_Cfg(const char *key) {
  return cfg_item_map_.find(key) != cfg_item_map_.end();
}
bool MysqlCfgFileParser::Has_Cfg(const std::string &key) {
  return cfg_item_map_.find(key) != cfg_item_map_.end();
}

const char *MysqlCfgFileParser::get_cfg_value(const char *key) {
  auto iter = cfg_item_map_.find(key);
  return (iter->second).c_str();
}
const char *MysqlCfgFileParser::get_cfg_value(const std::string &key) {
  auto iter = cfg_item_map_.find(key);
  return (iter->second).c_str();
}

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
  mysql_options(mysql_raw_,MYSQL_DEFAULT_AUTH, "mysql_native_password");

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
