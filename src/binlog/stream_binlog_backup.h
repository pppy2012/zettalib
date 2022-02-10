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
