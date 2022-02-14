/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include <arpa/inet.h>
#include <cerrno>
#include <dirent.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <limits.h>

namespace kunlun {
int GetIpFromInterface(const char *interface_name, char *addr) {
  int fd;
  struct ifreq ifr;

  // replace with your interface name
  // or ask user to input

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  if (fd == -1) {
    return errno;
  }

  // Type of address to retrieve - IPv4 IP address
  ifr.ifr_addr.sa_family = AF_INET;

  // Copy the interface name in the ifreq structure
  strncpy(ifr.ifr_name, interface_name, IFNAMSIZ - 1);

  if (ioctl(fd, SIOCGIFADDR, &ifr) != 0) {
    return errno;
  }
  close(fd);
  // display result
  sprintf(addr, "%s",
          inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
  return 0;
}

int GetFileListFromPath(const char *path,
                        std::vector<std::string> &file_names) {
  DIR *dir_ptr = nullptr;
  struct dirent *direntp = nullptr;
  if ((dir_ptr = opendir(path)) == nullptr) {
    return errno;
  } else {
    while ((direntp = readdir(dir_ptr)) != nullptr) {
      file_names.push_back(direntp->d_name);
    }
  }
  closedir(dir_ptr);
  return 0;
}
std::string ConvertToAbsolutePath(const char *path){
  char abs_path_buff[2048] = {'\0'};
  if (realpath(path,abs_path_buff) == nullptr){
    return "";
  }
  return std::string(abs_path_buff);
}


} // namespace kunlun
