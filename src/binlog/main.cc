/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include "hdfs_transfer.h"
#include "unistd.h"
#include "stdio.h"
#include "fcntl.h"
using namespace kunlun;
int main(int argc,char **argv){
  if(argc <2){
    printf("need abs file path\n");
    return -1;
  }
  int inputFd = open("test.file",O_RDWR);
  if (inputFd <=0){
    perror("open faild");
    return -1;
  }

  
  HdfsFile *pt = new HdfsFile(argv[1]);
  int fd = pt->OpenAsPopenFd();
  if (fd<0){
    printf("%s",pt->getErr());
    return -1;
  }
  char buf[4096];
  size_t rdcount = read(inputFd,buf,sizeof(buf));
  if(rdcount){
    pt->RemoteWriteByte(buf,rdcount);
  }
//  while(1){
    sleep(5);
 // }
  return 0;
}
