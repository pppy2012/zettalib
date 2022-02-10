/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include <sys/types.h>
#include "remote_transfer_base.h"
namespace kunlun {

RemoteFileBase::RemoteFileBase(){
  m_fd_ = -1;
}
RemoteFileBase::~RemoteFileBase(){}

int RemoteFileBase::RemoteReadByte(char *buffer, size_t count) {
  return ReadByteImpl(m_fd_, buffer, count);
}

int RemoteFileBase::RemoteWriteByte(char *buffer, size_t count) {
  return WriteByteImpl(m_fd_, buffer, count);
}

} // namespace kunlun
