#ifndef _REMOTE_TRANSFER_BASE_H_
#define _REMOTE_TRANSFER_BASE_H_

namespace kunlun {

class RemoteFileBase {
  typedef int fd_t;

public:
  RemoteFileBase();
  virtual ~RemoteFileBase();

  // must be reimplemented
  virtual int WriteByteImpl(fd_t, char *, size_t) = 0;
  virtual int ReadByteImpl(fd_t, char *, size_t) = 0;

  int RemoteWriteByte(char *, size_t);
  int RemoteReadByte(char *, size_t);
protected:
  fd_t m_fd_;

};
} // namespace kunlun

#endif /* _REMOTE_TRANSFER_BASE_H_ */
