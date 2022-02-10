/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include "op_mysql.h"
#include "stdio.h"

using namespace kunlun;
int main()
{
  MysqlConnectionOption option;
  option.ip = "192.168.0.135";
  option.port_str = "6001";
  option.port_num = 6001;
  option.user = "pgx";
  option.password = "pgx_pwd";
  option.timeout_sec = 5;
  option.connect_timeout_sec = 5;

  MysqlConnection mysql_conn(option);
  mysql_conn.Connect();
  MysqlResult result;
  char sql[2048] = {'\0'};
  sprintf(sql, "select * from mysql.user limit 1");

  int ret = mysql_conn.ExcuteQuery(sql, &result, true);
  printf("ret: %d, msg: %s\n", ret, mysql_conn.getErr());

  for (unsigned int i = 0; i < result.GetResultLinesNum(); i++)
  {
    result[i].to_string();
  }

  return 0;
}
