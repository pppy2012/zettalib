/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include <arpa/inet.h>
#include <cerrno>
#include <dirent.h>
#include <limits.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace kunlun {
std::vector<std::string> StringTokenize(std::string orig,
                                        const char *seperator) {
  std::vector<std::string> result;
  char *str = const_cast<char *>(orig.c_str());
  char *token = strtok(str, seperator);
  while (token != nullptr) {
    result.push_back(std::string(token));
    token = strtok(nullptr, seperator);
  }
  return result;
}
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
std::string ConvertToAbsolutePath(const char *path) {
  char abs_path_buff[2048] = {'\0'};
  if (realpath(path, abs_path_buff) == nullptr) {
    return "";
  }
  return std::string(abs_path_buff);
}

std::string StringReplace(const std::string &orig, const std::string &sbstr,
                          const std::string &dest) {
  std::string result = orig;
  size_t index = 0;
  if (sbstr.empty()) {
    return result;
  }
  while ((index = result.find(sbstr, index)) != std::string::npos) {
    result.replace(index, sbstr.length(), dest);
    index += dest.length();
  }
  return result;
}

bool CheckFileExists(const char *path) {
  std::string abs_path = ConvertToAbsolutePath(path);
  FILE *fp = fopen(abs_path.c_str(), "r");
  if (fp != nullptr) {
    // file exists
    fclose(fp);
    return true;
  }
  return false;
}

} // namespace kunlun
