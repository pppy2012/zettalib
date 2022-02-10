/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#ifndef _STREAM_BINLOG_BACKUP_H_
#define _STREAM_BINLOG_BACKUP_H_

#include <string>
namespace kunlun {

class BinlogStream : public ZThread {
 public:
  BinlogStream();
  ~BinlogStream();
  int run();
 private:
  std::string m_binlog_dir_;
  std::string 
};

} // namespace kunlun

#endif /*_STREAM_BINLOG_BACKUP_H_*/
