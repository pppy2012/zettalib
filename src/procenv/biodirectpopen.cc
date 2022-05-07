/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include "biodirectpopen.h"
#include "errno.h"
#include <cstring>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace kunlun {
static void closeFdFromVec(std::vector<int> &vec) {
  for (auto iter = vec.begin(); iter != vec.end(); iter++) {
    ::close(*iter);
  }
  return;
}

int BiodirectPopen::getWriteFd() const {
  if (write_to_child_) {
    return stdin_parent_write_fd[1];
  }
  return -1;
}
int BiodirectPopen::getReadStdOutFd() const {
  if (read_from_child_) {
    return stdout_parent_read_fd[0];
  }
  return -1;
}
int BiodirectPopen::getReadStdErrFd() const {
  if (read_from_child_) {
    return stderr_parent_read_fd[0];
  }
  return -1;
}

FILE *BiodirectPopen::getWriteFp() const {
  if (write_to_child_) {
    return write_to_fp_;
  }
  return nullptr;
}
FILE *BiodirectPopen::getReadStdOutFp() const {
  if (read_from_child_) {
    return read_from_stdout_fp_;
  }
  return nullptr;
}
FILE *BiodirectPopen::getReadStdErrFp() const {
  if (read_from_child_) {
    return read_from_stderr_fp_;
  }
  return nullptr;
}

void BiodirectPopen::closeAllFd() {
  for (int i = 0; i < 2; i++) {
    if (stdout_parent_read_fd[i] >= 0) {
      ::close(stdout_parent_read_fd[i]);
    }
    if (stderr_parent_read_fd[i] >= 0) {
      ::close(stderr_parent_read_fd[i]);
    }
    if (stdin_parent_write_fd[i] >= 0) {
      ::close(stdin_parent_write_fd[i]);
    }
  }
}
bool BiodirectPopen::Launch(const char *mode, bool block) {
  // parse mode
  const char *pt = mode;
  while (*pt != '\0') {
    switch (*pt) {
    case 'r': {
      read_from_child_ = true;
      break;
    }
    case 'w': {
      write_to_child_ = true;
      break;
    }
    case 'c': {
      set_close_on_exe_ = true;
      break;
    }
    default: {
      setErr("unknown mode char");
      return false;
    }
    }
    pt++;
  }
  if (!read_from_child_ && !write_to_child_) {
    setErr("Launch biodirectpopen faild, mode is not valid, must be 'rwc'");
    return false;
  }

  wait_status_ = block;
  return popenImpl();
}

// only use pipe2()
bool BiodirectPopen::popenImpl() {

  int flag = 0;
  std::vector<int> invalid_fd;

  if (set_close_on_exe_) {
    flag = O_CLOEXEC;
  }

  if (read_from_child_) {
    // make sure the fd made by pipe2 is bigger than 2
    for (;;) {
      if (pipe2(stdout_parent_read_fd, flag) < 0) {
        setErr("%s", strerror(errno));
        return false;
      }
      if (stdout_parent_read_fd[0] <= 2 || stdout_parent_read_fd[1] <= 2) {
        invalid_fd.push_back(stdout_parent_read_fd[0]);
        invalid_fd.push_back(stdout_parent_read_fd[1]);
        continue;
      }
      break;
    }

    for (;;) {
      if (pipe2(stderr_parent_read_fd, flag) < 0) {
        setErr("%s", strerror(errno));
        return false;
      }
      if (stderr_parent_read_fd[0] <= 2 || stderr_parent_read_fd[1] <= 2) {
        invalid_fd.push_back(stderr_parent_read_fd[0]);
        invalid_fd.push_back(stderr_parent_read_fd[1]);
        continue;
      }
      break;
    }
  }

  if (write_to_child_) {
    for (;;) {
      if (pipe2(stdin_parent_write_fd, flag) < 0) {
        setErr("%s", strerror(errno));
        return false;
      }
      if (stdin_parent_write_fd[0] <= 2 || stdin_parent_write_fd[1] <= 2) {
        invalid_fd.push_back(stdin_parent_write_fd[0]);
        invalid_fd.push_back(stdin_parent_write_fd[1]);
        continue;
      }
      break;
    }
  }
  // close the redundent fd
  closeFdFromVec(invalid_fd);

  // do fork
  child_pid_ = fork();
  if (child_pid_ < 0) {
    if (read_from_child_) {
      ::close(stdout_parent_read_fd[0]);
      ::close(stdout_parent_read_fd[1]);
      ::close(stderr_parent_read_fd[0]);
      ::close(stderr_parent_read_fd[1]);
    }
    if (write_to_child_) {
      ::close(stdin_parent_write_fd[0]);
      ::close(stdin_parent_write_fd[1]);
    }
    setErr("Biodirectionall popen do fork() faild");
    return false;
  }

  if (child_pid_ > 0) {
    // parent proc
    if (read_from_child_) {
      read_from_stdout_fp_ = fdopen(stdout_parent_read_fd[0], "r");
      if (read_from_stdout_fp_ == nullptr) {
        closeAllFd();
        setErr("biodirectional popen invoke fdopen() faild");
        return false;
      }
      ::close(stdout_parent_read_fd[1]);

      read_from_stderr_fp_ = fdopen(stderr_parent_read_fd[0], "r");
      if (read_from_stderr_fp_ == nullptr) {
        closeAllFd();
        setErr("biodirectional popen invoke fdopen() faild");
        return false;
      }
      ::close(stderr_parent_read_fd[1]);
    }

    if (write_to_child_) {
      write_to_fp_ = fdopen(stdin_parent_write_fd[1], "w");
      if (write_to_fp_ == nullptr) {
        closeAllFd();
        setErr("biodirectional popen invoke fdopen() faild");
        return false;
      }
      ::close(stdin_parent_write_fd[0]);
    }

    if (wait_status_) {
      // will block
      return waitStatus();
    }
    return true;
  }
  // child proc

  signal(SIGPIPE, SIG_DFL);
  if (read_from_child_) {
    dup3(stdout_parent_read_fd[1], STDOUT_FILENO, flag);
    dup3(stderr_parent_read_fd[1], STDERR_FILENO, flag);

    // clean fd_cloexec flag
    fcntl(STDOUT_FILENO, F_SETFD, 0);
    fcntl(STDERR_FILENO, F_SETFD, 0);
  }
  if (write_to_child_) {
    dup3(stdin_parent_write_fd[0], STDIN_FILENO, flag);

    // clean fd_cloexec flag
    fcntl(STDIN_FILENO, F_SETFD, 0);
  }

  const char *argvs[4];
  argvs[0] = "sh";
  argvs[1] = "-c";
  argvs[2] = command_.c_str();
  argvs[3] = nullptr;

  /*execv*/
  execv("/bin/sh", (char *const *)(argvs));

  setErr("Can not reach here after execv()");
  /* can not reach here */
  return false;
}

int BiodirectPopen::get_chiled_status() const {
  if (wait_status_) {
    return child_return_code_;
  }
  return -100;
}

bool BiodirectPopen::HasErrorOutput() {
  std::string output = "";
  char buff[2048] = {'\0'};
  while (fgets(buff, 2048, read_from_stderr_fp_)) {
    output.append(buff);
    bzero((void *)buff, 2048);
  }
  setErr("%s",output.c_str());
  return !output.empty();

}

bool BiodirectPopen::waitStatus() {
  pid_t ret;
  int status;
  if (child_pid_ >= 0) {
    ret = waitpid(child_pid_, &status, 0);
    if (ret != child_pid_) {
      setErr("waitpid() failed: %s", strerror(errno));
      return false;
    }
    // inspect the status;
    if (WIFEXITED(status)) {
      child_return_code_ = WEXITSTATUS(status);
      return true;
    } else if (WIFSIGNALED(status)) {
      child_return_code_ = WTERMSIG(status);
      if (WCOREDUMP(status)) {
        setErr("child is coredump related to the signal %d",
               child_return_code_);
      } else {
        setErr("child is terminited by the signal %d", child_return_code_);
      }
      return false;
    }
    setErr("child unknow error");
    return false;
  }
  setErr("child pid is invalid, means fork() failed");
  return false;
}

BiodirectPopen::~BiodirectPopen() {
  int status;
  if (child_pid_ >= 0) {
    waitpid(child_pid_, &status, 0);
  }
  closeAllFd();
}
} // namespace kunlun
