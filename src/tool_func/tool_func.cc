/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

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
} // namespace kunlun
