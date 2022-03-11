/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include <string>
#include <vector>
namespace kunlun {
int GetIpFromInterface(const char *interface_name, char *addr);
// File/Dir related
int GetFileListFromPath(const char *path, std::vector<std::string> &file_names);
std::string ConvertToAbsolutePath(const char *path);
std::string StringReplace(const std::string &orig, const std::string &sbstr,
                          const std::string &dest);
std::vector<std::string> StringTokenize(std::string orig,
                                        const char *seperator);
bool CheckFileExists(const char *path);

} // namespace kunlun
