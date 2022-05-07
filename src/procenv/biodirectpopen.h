/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#ifndef _BIODIRECT_POPEN_H_
#define _BIODIRECT_POPEN_H_

#include <cstring>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifdef ZETTA_LIB_COMPILER
#include "error/errorcup.h"
#else
#include "zettalib/errorcup.h"
#endif

namespace kunlun {
// biodirectional popen implementation
class BiodirectPopen : public kunlun::ErrorCup {
public:
  explicit BiodirectPopen(const char *command)
      : command_(command), read_from_child_(true), write_to_child_(false),
        set_close_on_exe_(true), wait_status_(true) {
    for (int i = 0; i < 2; i++) {
      stdout_parent_read_fd[i] = -1;
      stderr_parent_read_fd[i] = -1;
      stdin_parent_write_fd[i] = -1;
    }
    write_to_fp_ = nullptr;
    read_from_stdout_fp_ = nullptr;
    read_from_stderr_fp_ = nullptr;
  };
  ~BiodirectPopen();

private:
  bool popenImpl();
  void pclose();
  bool waitStatus();

public:
  bool Launch(const char *mode, bool block = true);
  int readOutputLineByLine(char *, size_t);
  bool HasErrorOutput();

  // file descriptor
  int getWriteFd() const;
  int getReadStdOutFd() const;
  int getReadStdErrFd() const;

  // FILE stream
  FILE *getWriteFp() const;
  FILE *getReadStdOutFp() const;
  FILE *getReadStdErrFp() const;

  int get_chiled_status() const;

  void closeAllFd();

private:
  // forbide copy
  BiodirectPopen(const BiodirectPopen &rht) = delete;
  BiodirectPopen &operator=(const BiodirectPopen &rht) = delete;

private:
  std::string command_;
  bool read_from_child_;
  bool write_to_child_;
  bool set_close_on_exe_;
  // Get the child status or not
  bool wait_status_;
  int child_return_code_;
  int stdout_parent_read_fd[2];
  int stderr_parent_read_fd[2];
  int stdin_parent_write_fd[2];

  FILE *write_to_fp_;
  FILE *read_from_stdout_fp_;
  FILE *read_from_stderr_fp_;
  pid_t child_pid_;
};

} // namespace kunlun
#endif /*_BIODIRECT_POPEN_H_*/
