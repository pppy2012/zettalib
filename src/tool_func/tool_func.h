/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#pragma once
#include <cassert>
#include <string>
#include <vector>

namespace kunlun {
int GetIpFromInterface(const char *interface_name, char *addr);
// File/Dir related
int GetFileListFromPath(const char *path, std::vector<std::string> &file_names);
std::string ConvertToAbsolutePath(const char *path);
std::string StringReplace(const std::string &orig, const std::string &sbstr,
                          const std::string &dest);
std::string GetBasePath(std::string);
std::vector<std::string>
StringTokenize(std::string s, const std::string delims, size_t num = 0);
bool CheckFileExists(const char *path);
bool contains ( const std::string& s , const std::string& substr ) ;

std::string ltrim(const std::string &s, const std::string &delimter = "");
std::string rtrim(const std::string &s, const std::string &delimter = "");
std::string trim(const std::string &s, const std::string &delimter = "");

template <typename... Args>
std::string string_sprintf(const char *format, Args... args) {
  int length = std::snprintf(nullptr, 0, format, args...);
  assert(length >= 0);

  char *buf = new char[length + 1];
  std::snprintf(buf, length + 1, format, args...);

  std::string str(buf);
  delete[] buf;
  return str;
}
std::string GetProcessOwnerName();

} // namespace kunlun
