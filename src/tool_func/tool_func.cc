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
#include <pwd.h>
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

bool contains(const std::string &s, const std::string &substr) {
  return s.find(substr) != std::string::npos;
}
const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s, const std::string &delimter = "") {
  size_t start = 0;
  if (delimter.size() == 0) {
    start = s.find_first_not_of(WHITESPACE);
  } else {
    start = s.find_first_not_of(delimter);
  }
  return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s, const std::string &delimter = "") {
  size_t end = 0;
  if (delimter.size() == 0) {
    end = s.find_last_not_of(WHITESPACE);
  } else {
    end = s.find_last_not_of(delimter);
  }
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s, const std::string &delimter = "") {
  return rtrim(ltrim(s, delimter), delimter);
}

std::string GetBasePath(std::string path) {
  size_t pos = path.find_last_of("/");
  if (pos == std::string::npos) {
    return path;
  }
  return path.substr(0, pos);
}

std::vector<std::string> StringTokenize(std::string s, const std::string delims,
                                        size_t num) {
  size_t offset = 0;
  std::vector<std::string> tokens;

  while (true) {
    size_t i = s.find_first_not_of(delims, offset);
    if (std::string::npos == i) {
      break;
    }

    size_t j = s.find_first_of(delims, i);
    if (std::string::npos == j) {
      tokens.push_back(s.substr(i));
      offset = s.length();
      break;
    }

    if (tokens.size() + 1 == num) {
      tokens.push_back(s.substr(i));
      break;
    } else {
      tokens.push_back(s.substr(i, j - i));
      offset = j;
    }
  }
  return tokens;
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

std::string GetProcessOwnerName() {
  uid_t uid = geteuid();
  struct passwd * pwd_t = getpwuid(uid);
  if(pwd_t == nullptr){
    return "";
  }
  return std::string ((const char *)(pwd_t->pw_name));
}

} // namespace kunlun
