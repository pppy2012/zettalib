/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#pragma once

#include <map>
#include <string>
#ifdef ZETTA_LIB_COMPILER
#include "error/errorcup.h"
#else
#include "zettalib/errorcup.h"
#endif

namespace kunlun {
class KConfig : public kunlun::ErrorCup {
public:
  explicit KConfig(std::string &path) : file_path_(path){};
  ~KConfig() = default;
  bool parse();
  std::string get_config(const char *);

private:
  std::string file_path_;
  std::map<std::string, std::string> key_value_;
};

class MysqlCfgItem : public kunlun::ErrorCup {
  friend class MysqlCfgFileParser;

public:
  MysqlCfgItem() = default;
  ~MysqlCfgItem() = default;
  bool PasrseItem(const char *);
  bool PasrseItem(const std::string &);
  const std::string &get_key() const;
  const std::string &get_value() const;

private:
  std::string key_;
  std::string value_;
  bool is_boolean_;
};

class MysqlCfgFileParser : public kunlun::ErrorCup {
public:
  explicit MysqlCfgFileParser(const char *etc_file_path)
      : etc_file_path_(etc_file_path) {}

  explicit MysqlCfgFileParser(std::string &etc_file_path)
      : etc_file_path_(etc_file_path) {}
  ~MysqlCfgFileParser() = default;
  bool Parse();
  bool Has_Cfg(const char *);
  bool Has_Cfg(const std::string &);
  const char *get_cfg_value(const char *);
  const char *get_cfg_value(const std::string &);

private:
  std::string etc_file_path_;
  std::map<std::string, std::string> cfg_item_map_;
};

} // namespace kunlun
