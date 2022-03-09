/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
namespace kunlun
{
  void procDaemonize()
  {
    pid_t pid;

    if ((pid = fork()) != 0)
    {
      // only keep the child process
      _exit(0);
    }

    // renew session, detach from term
    setsid();

    // ignore unconsernd signal;
    struct sigaction sig;
    sig.sa_handler = SIG_IGN;
    sig.sa_flags = 0;
    sigemptyset(&sig.sa_mask);
    sigaction(SIGINT, &sig, NULL);
    sigaction(SIGHUP, &sig, NULL);
    sigaction(SIGQUIT, &sig, NULL);
    sigaction(SIGPIPE, &sig, NULL);
    sigaction(SIGTTOU, &sig, NULL);
    sigaction(SIGTTIN, &sig, NULL);
    sigaction(SIGTERM, &sig, NULL);

    if ((pid = fork()) != 0)
    {
      _exit(0);
    }

    umask(0);

 /* The reason why we comment such fragement below related to trac ticket #xxx */
 // 
 //   // close fd
 //   struct rlimit rl;
 //   getrlimit(RLIMIT_NOFILE, &rl);
 //   if (rl.rlim_max == RLIM_INFINITY)
 //   {
 //     rl.rlim_max = 1024;
 //   }
 //   unsigned int i = 3; // we preserve the 0,1,2 fd
 //   for (; i < rl.rlim_max; i++)
 //   {
 //     close(i);
 //   }

    return;
  }
  /**
   * Service invoke this procedure means the process
   * can not be stoped by it self except the signal
   * unignorable
   * */
  void procInvokeKeepalive()
  {
    // child return after fork.
    bool needRestart = true;
    while (needRestart)
    {

      pid_t pid;
      if ((pid = fork()) < 0)
      {
        perror("kunlun::proc_invoke_keepalive: ");
        return;
      }
      if (pid == 0)
      {
        /*child proc*/
        return;
      }

      /*parent proc*/

      // parent waitpid and determain the receipt of the exitstatus.
      int wstatus = 0;

      waitpid(pid, &wstatus, 0);
      /* for all state change of the child process,we all do the restart */

      needRestart = true;

      // TODO : for specical situation, exit whole process?
      // needRestart = false;
      sleep(1);
    }
    return;
  }
} // namespace kunlun
