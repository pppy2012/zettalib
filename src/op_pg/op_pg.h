/*
   Copyright (c) 2019-2022 ZettaDB inc. All rights reserved.

   This source code is licensed under Apache 2.0 License,
   combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/

#ifndef _CLUSTER_MGR_OP_PG_H_
#define _CLUSTER_MGR_OP_PG_H_
#include <vector>
#include <map>
#include <assert.h>
#ifdef ZETTA_LIB_COMPILER
#include "error/errorcup.h"
#include "op_mysql/op_mysql.h"
#else
#include "zettalib/errorcup.h"
#include "zettalib/op_mysql.h"
#endif
#include "pgsql/libpq-fe.h"

using namespace std;

namespace kunlun 
{

class PgResRow {
public:
    PgResRow(const std::map<std::string, int>& si_map) :
        result_(NULL), si_map_(si_map) {}
    ~PgResRow(); 

    void init(char** value, unsigned long *lens, uint32_t field_num);
    typedef char * charptr;
    char * operator[] (const char* name) {
        std::map<std::string, int>::const_iterator it = si_map_.find(name);
        if(it == si_map_.end())
            return (char*) "NULL";

        int field = it->second;
        return result_[field];
    }

    char * operator[] (size_t index) {
        if(index >= si_map_.size())
            return (char*) "NULL";
        return result_[index];
    }
private:
    PgResRow(const PgResRow& rht) = delete;
    PgResRow & operator = (const PgResRow& rht) = delete;

    char** result_;
    std::map<std::string, int> si_map_;
    uint32_t num_;
};    

class PgResult : public kunlun::ErrorCup {
public:
    PgResult() {}
    ~PgResult() { Clear(); }

    bool Parse(PGresult* res);
    void Clear();
    void SetAffectedNum(int affected_num) {
        affected_num_ = affected_num;
    }
    int GetAffectedNum() const {
        return affected_num_;
    }

    uint32_t GetNumRows() const {
        return row_vec_.size();
    }

    PgResRow &operator[] (uint32_t index) {
        assert(index < row_vec_.size());
        return *(row_vec_[index]);
    }

private:
    PgResult(const PgResult& pgres) = delete;
    PgResult& operator = (const PgResult &pgres) = delete;

private:
    int affected_num_;
    /*
    * for select or show
    */
    uint32_t field_num_;
    std::map<std::string, int> si_map_;
    std::map<int, std::string> is_map_;
    std::vector<PgResRow*> row_vec_;
};

typedef struct PGConnectionOption_ {
    ENUM_SQL_CONNECT_TYPE connection_type = TCP_CONNECTION;
    string ip = "";
    string port_str = "";
    unsigned int port_num = 0;
    string user = "";
    string password = "";
    string database = "";
    std::string sock_path="";
    int connect_timeout = 10;
    bool autocommit = true;
} PGConnectionOption;

class PGConnection : public kunlun::ErrorCup {
public:
    explicit PGConnection(PGConnectionOption option)
        : last_errno_(0), pg_conn_(nullptr), reconnect_support_(true),
        pg_connection_option_(option) {}
    ~PGConnection() { Close(); }

public:
    bool Connect();
    bool CheckIsConnected();
    void Close();
    bool SetAutoCommit();
    void Reconnect();

    int ExecuteQuery(const char* sql_stmt, PgResult* res, bool force_retry=false);
    int GetLastErrno() const {
        return last_errno_;
    }
    int GetPGConnectionFd() {
        return PQsocket(pg_conn_);
    }

private:
    int last_errno_;
    PGconn *pg_conn_;
    bool reconnect_support_;
    PGConnectionOption pg_connection_option_;
};

}
#endif
