/*
  Copyright (c) 2019-2021 ZettaDB inc. All rights reserved.

  This source code is licensed under Apache 2.0 License,
  combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/
#include "config_deal.h"
#include "error.h"
#include "stdlib.h"
#include "string.h"
#include <sys/stat.h>
#ifdef ZETTA_LIB_COMPILER
#include "tool_func/tool_func.h"
#else
#include "zettalib/tool_func.h"
#endif
#include <memory>
#include <sys/types.h>
#include <vector>
using namespace kunlun;
using namespace std;
bool KConfig::parse() {
  struct stat sb;
  int ret = stat(file_path_.c_str(), &sb);
  if (ret) {
    setErr("Config file path: %s is invalid, %s", file_path_.c_str(),
           strerror(errno));
    return false;
  }
  FILE *fp = fopen(file_path_.c_str(), "r");
  if (fp == nullptr) {
    setErr("fopen() config file %s failed, %s", file_path_.c_str(),
           strerror(errno));
    return false;
  }
  char line_buff[4096] = {'\0'};
  while (line_buff == fgets(line_buff, 4096, fp)) {
    std::string tmp = line_buff;
    tmp = kunlun::trim(tmp);
    if (tmp.empty()) {
      // empty line
      continue;
    }
    size_t pos1 = tmp.find_first_of("#");
    if (pos1 == 0) {
      // Comments block identifier
      bzero((void *)(line_buff), 4096);
      continue;
    }

    auto lc = kunlun::StringTokenize(tmp, "=");
    if (lc.size() != 2) {
      setErr("missformate config item: %s", tmp.c_str());
      return false;
    }
    key_value_[kunlun::trim(lc[0])] = kunlun::trim(lc[1]);
  }
  if (key_value_.size() == 0) {
    setErr("There is no avilable config item in the given config file %s",
           file_path_.c_str());
    fclose(fp);
    return false;
  }
  fclose(fp);
  return true;
}

std::string KConfig::get_config(const char * key){
  std::string key_str = key;
  auto iter = key_value_.find(key_str);
  if(iter != key_value_.end()){
    return iter->second;
  }
  return "";
}
bool MysqlCfgItem::PasrseItem(const char *item) {

  const char *location = strstr(item, "=");

  if (location == nullptr) {
    is_boolean_ = true;
    key_ = item;
    value_ = "";
    if (key_.empty()) {
      setErr("key is empty");
      return false;
    }
  } else {
    is_boolean_ = false;
    size_t prefix_len = (size_t)(location - item);
    key_.assign(item, prefix_len);
    key_ = kunlun::trim(key_);

    value_.assign(item + prefix_len + 1, strlen(item) - prefix_len);
    value_ = kunlun::trim(value_);
    value_ = kunlun::trim(value_, "'\"");

    if (key_.empty()) {
      setErr("key is empty");
      return false;
    }
    if (value_.empty()) {
      setErr("value is empty");
      return false;
    }
  }
  return true;
}
bool MysqlCfgItem::PasrseItem(const std::string &item_str) {

  const char *item = item_str.c_str();
  const char *location = strstr(item, "=");

  if (location == nullptr) {
    is_boolean_ = true;
    key_ = item;
    value_ = "";
    if (key_.empty()) {
      setErr("key is empty");
      return false;
    }
  } else {
    is_boolean_ = false;
    size_t prefix_len = (size_t)(location - item);
    key_.assign(item, prefix_len);
    key_ = kunlun::trim(key_);

    value_.assign(item + prefix_len + 1, strlen(item) - prefix_len);
    value_ = kunlun::trim(value_);
    value_ = kunlun::trim(value_, "'\"");

    if (key_.empty()) {
      setErr("key is empty");
      return false;
    }
    if (value_.empty()) {
      setErr("value is empty");
      return false;
    }
  }
  return true;
}
const std::string &MysqlCfgItem::get_key() const { return key_; }
const std::string &MysqlCfgItem::get_value() const { return value_; }

bool MysqlCfgFileParser::Parse() {
  struct stat sb;
  int ret = stat(etc_file_path_.c_str(), &sb);
  if (ret) {
    setErr("Etc file path: %s is invalid, %s", etc_file_path_.c_str(),
           strerror(errno));
    return false;
  }
  FILE *fp = fopen(etc_file_path_.c_str(), "r");
  if (fp == nullptr) {
    setErr("fopen() etc file %s failed, %s", etc_file_path_.c_str(),
           strerror(errno));
    return false;
  }
  char line_buff[4096] = {'\0'};
  while (line_buff == fgets(line_buff, 4096, fp)) {
    std::string tmp = line_buff;
    tmp = kunlun::trim(tmp);
    if (tmp.empty()) {
      // empty line
      continue;
    }
    size_t pos1 = tmp.find_first_of("#");
    size_t pos2 = tmp.find_first_of("[");
    if (pos1 == 0 || pos2 == 0) {
      // Comments or config block identifier
      bzero((void *)(line_buff), 4096);
      continue;
    }
    MysqlCfgItem item;
    bool ret = item.PasrseItem(tmp);
    if (!ret) {
      setErr("%s", item.getErr());
      fclose(fp);
      return false;
    }
    cfg_item_map_[item.get_key()] = item.get_value();
    bzero((void *)(line_buff), 4096);
  }
  if (cfg_item_map_.size() == 0) {
    setErr("There is no avilable config item in the given etc file");
    fclose(fp);
    return false;
  }
  fclose(fp);
  return true;
}

bool MysqlCfgFileParser::Has_Cfg(const char *key) {
  return cfg_item_map_.find(key) != cfg_item_map_.end();
}
bool MysqlCfgFileParser::Has_Cfg(const std::string &key) {
  return cfg_item_map_.find(key) != cfg_item_map_.end();
}

const char *MysqlCfgFileParser::get_cfg_value(const char *key) {
  auto iter = cfg_item_map_.find(key);
  return (iter->second).c_str();
}
const char *MysqlCfgFileParser::get_cfg_value(const std::string &key) {
  auto iter = cfg_item_map_.find(key);
  return (iter->second).c_str();
}
