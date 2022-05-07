/*
   Copyright (c) 2019-2022 ZettaDB inc. All rights reserved.

   This source code is licensed under Apache 2.0 License,
   combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include "op_pg.h"

namespace kunlun 
{

bool PGConnection::Connect() {
    if(pg_conn_) {
        setErr("connection to pg already established");
        return false;
    }

    if(!(pg_connection_option_.connection_type == TCP_CONNECTION || 
            pg_connection_option_.connection_type == UNIX_DOMAIN_CONNECTION)) {
        setErr("connection type is not supported");
        return false;
    }

    //std::string conninfo;
    char conninfo[1024] = {0};
    std::string host;
    if(pg_connection_option_.connection_type == TCP_CONNECTION)
        host = pg_connection_option_.ip;
    else
        host = pg_connection_option_.sock_path;
    
    std::string database;
    if(!pg_connection_option_.database.empty())
        database = pg_connection_option_.database;
    else
        database = "postgres";
    
    snprintf(conninfo, 1024, "host=%s port=%d user=%s password=%s connect_timeout=%d dbname=%s", 
                host.c_str(),pg_connection_option_.port_num,
                pg_connection_option_.user.c_str(), pg_connection_option_.password.c_str(), 
                pg_connection_option_.connect_timeout, database.c_str());
    
    pg_conn_ = PQconnectdb(conninfo);
    if(!pg_conn_) {
        setErr("PQconnectdb error by conninfo: %s", conninfo);
        return false;
    }

    if(PQstatus(pg_conn_) != CONNECTION_OK) {
        setErr("Connection to database failed: %s, conninfo: %s", 
                PQerrorMessage(pg_conn_), conninfo);
        PQfinish(pg_conn_);
        pg_conn_ = nullptr;
        return false;
    }
    return true;
} 

bool PGConnection::CheckIsConnected() {
    if(pg_conn_ != nullptr)
        return true;

    if(reconnect_support_)
        Connect();
    
    setErr("PG connection is not established and reconnect is disabled");
    return false;
}

bool PGConnection::SetAutoCommit() {
    return true;
}

int PGConnection::ExecuteQuery(const char* sql_stmt, PgResult* pgResult, bool force_retry) {
    if(!CheckIsConnected())
        return -1;
    
    PGresult *res = PQexec(pg_conn_, sql_stmt);
    unsigned int status = PQresultStatus(res);

    //for insert/update/delete...
    if(status == PGRES_COMMAND_OK) {
        char* affected_num = PQcmdTuples(res);
        pgResult->SetAffectedNum(atoi(affected_num));
    } else if(status == PGRES_TUPLES_OK) {
        if(!pgResult->Parse(res))
            return -1;
    } else {
        char *errnum = PQresultErrorField(res, PG_DIAG_SQLSTATE);
        if(strcmp(errnum, "08006") == 0 ||
                strcmp(errnum, "08003") == 0) {
            if(force_retry) {
                Reconnect();
                return ExecuteQuery(sql_stmt, pgResult, false);
            }
            setErr("execute query sql: %s failed, %s, %s", sql_stmt, 
                        errnum, PQresultErrorMessage(res));
            last_errno_ = atoi(errnum);
            return -1;
        }
    }
    return 0;
}

void PGConnection::Reconnect() {

}

void PGConnection::Close() {
    if(pg_conn_) {
        PQfinish(pg_conn_);
        pg_conn_ = nullptr;
    }
}

void PgResRow::init(char** value, unsigned long* lens, uint32_t field_num) {
    num_ = field_num;
    if(field_num == 0)
        return;

    result_ = new charptr[field_num];
    for(uint32_t i=0; i<field_num; i++) {
        int len = lens[i];
        if("" == value[i]) {
            result_[i] = (char*)"NULL";
        } else {
            result_[i] = new char[len+1];
            memcpy(result_[i], value[i], len);
            result_[i][len] = 0;
        }
    }
}

PgResRow::~PgResRow() {
    if(result_) {
        for(uint32_t i=0; i<num_; i++) {
            if(result_[i] != (char*)"NULL")
                delete[] result_[i];
        }

        delete[] result_;
    }
}

void PgResult::Clear() {
    field_num_ = 0;
    si_map_.clear();
    is_map_.clear();
    for(auto it : row_vec_) 
        delete it;
    row_vec_.clear();
}

bool PgResult::Parse(PGresult* res) {
    Clear();

    field_num_ = PQnfields(res);
    for(uint32_t i=0; i<field_num_; i++) {
        std::string f_name = PQfname(res, i);
        si_map_[f_name] = i;
        is_map_[i] = f_name;
    }

    uint32_t row_num_ = PQntuples(res);
    for(uint32_t i=0; i<row_num_; i++) {
        unsigned long *lens = new unsigned long[field_num_];
        char** value = new char*[field_num_];
        for(uint32_t j=0; j<field_num_; j++) {
            lens[j] = PQgetlength(res, i, j);
            value[j] = PQgetvalue(res, i, j);
        }

        PgResRow* pgres = new PgResRow(si_map_);
        pgres->init(value, lens, field_num_);
        row_vec_.emplace_back(pgres);

        delete[] lens;
        delete[] value;
    }
    return true;
}

}