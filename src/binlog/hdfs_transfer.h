/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/

#ifndef _HDFS_TRANSFER_H_
#define _HDFS_TRANSFER_H_

#include "errorcup.h"
#include "remote_transfer_base.h"
#include <string>
namespace kunlun {
class HdfsFile : public RemoteFileBase, public ErrorCup {
  typedef int fd_t;

public:
  HdfsFile(const char *dst_file_name);
  ~HdfsFile();

  /**
   * use popen to invoke the `hadoop` client program
   * under the ${PATH}, then return file descriptor
   * which refers to the m_dst_file_name to application,
   * which support the write()-like operation.
   * */
  int OpenAsPopenFd();

  std::string get_dst_file_name() const;
  void set_dst_file_name();

  virtual int WriteByteImpl(fd_t, char *, size_t) override final;
  virtual int ReadByteImpl(fd_t, char *, size_t) override final;

private:
  std::string m_dst_file_name_;
  FILE *m_hdfs_file_ptr_;
};

} // namespace kunlun

#endif /*_HDFS_TRANSFER_H_*/
