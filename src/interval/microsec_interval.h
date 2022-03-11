/*
  Copyright (c) 2019-2022 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#pragma once
#include <stdlib.h>
#include <sys/time.h>
namespace kunlun {
// Interval time checker in milsecond level
// 1 sec = 1 * 1000000 microsecond
class MicroSecInterval {
public:
  explicit MicroSecInterval(time_t interval_microsec,
                          time_t last_timeout_microsec = 0)
      : interval_microsec_(interval_microsec),
        last_timeout_microsec_(last_timeout_microsec) {}
  ~MicroSecInterval() {}

  bool timeout() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    cur_microsec_ = tv.tv_sec * 1000 * 1000 + tv.tv_usec ;
    if (labs(cur_microsec_ - last_timeout_microsec_) >= interval_microsec_) {
      last_timeout_microsec_ = cur_microsec_;
      return true;
    }
    return false;
  }

private:
  time_t interval_microsec_;
  time_t last_timeout_microsec_;
  time_t cur_microsec_;
};

} // namespace kunlun
