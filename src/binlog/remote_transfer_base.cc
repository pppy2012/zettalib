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
