/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/

#include "hdfs_transfer.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

using namespace kunlun;

HdfsFile::HdfsFile(const char *dst_file_name) {
  m_dst_file_name_ = std::string(dst_file_name);
  m_hdfs_file_ptr_ = NULL;
}

HdfsFile::~HdfsFile() {
  if (m_hdfs_file_ptr_ != NULL) {
    // watch out the zombie proc
    pclose(m_hdfs_file_ptr_);
  }
}

int HdfsFile::OpenAsPopenFd() {
  // TODO: dynamic string buffer needed.
  char buf[2048] = {'\0'};
  snprintf(buf, sizeof(buf) - 1, "hadoop  fs -put -p -f - %s",
           m_dst_file_name_.c_str());
  // TODO: biodirection popen-like API needed.
  FILE *fpt = popen(buf, "w");
  if (fpt == NULL) {
    setErr("popen() failed, errno: %d, errmsg: %s", errno, strerror(errno));
    return -1;
  }
  return m_fd_ = fileno(fpt);
}

int HdfsFile::WriteByteImpl(fd_t fd, char *buffer, size_t count) {
  ssize_t numWrite = 0;
  ssize_t numLeft = count;
  while (numLeft) {
    numWrite = write(fd, buffer, numLeft);
    if (numWrite < 0) {
      if (errno == EINTR || errno == EAGAIN) {
        // TODO maybe here can interfere time limit opts
        continue;
      } else {
        return count - numLeft;
      }
    }
    numLeft -= numWrite;
    buffer += numWrite;
  }
  return count - numLeft;
}

int HdfsFile::ReadByteImpl(fd_t fd, char *buffer, size_t count) { return 0; }
