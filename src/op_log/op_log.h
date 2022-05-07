/*
   Copyright (c) 2019-2022 ZettaDB inc. All rights reserved.

   This source code is licensed under Apache 2.0 License,
   combined with Common Clause Condition 1.0, as detailed in the NOTICE file.
*/

#ifndef _OP_LOG_H_
#define _OP_LOG_H_

#include <time.h>
#include <chrono>
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/kl_file_sink.h"

namespace kunlun 
{

class Op_Log {
public:
   static Op_Log* getInstance() {
      static Op_Log op_log;
      return &op_log;
   }

   /*
   * set log path, log_prefix and size (MB)
   */
   void init(const std::string& log_path, const std::string& log_prefix, int size) {
      std::string log_name= log_prefix + ".log";
      spdlog::init_thread_pool(10240, 1);
      auto kl_sink = std::make_shared<spdlog::sinks::kl_file_sink_mt>(log_path+"/"+log_name, 
                              1024*1024*size, 23, 59);

      std::vector<spdlog::sink_ptr> sinks {kl_sink};
      logger_ = std::make_shared<spdlog::async_logger>(log_path+"/"+log_name, sinks.begin(), sinks.end(), 
                  spdlog::thread_pool(),  spdlog::async_overflow_policy::overrun_oldest);
      logger_.get()->set_pattern("%Y-%m-%d %H:%M:%S.%f <%t> [%l] [%@] %v");
      spdlog::register_logger(logger_);
      spdlog::flush_every(std::chrono::seconds(5));
   }

   std::shared_ptr<spdlog::async_logger> getLogger() {
      return logger_;
   }
private:
   Op_Log() {}
   virtual ~Op_Log() {
      spdlog::drop_all();
   }
 
	Op_Log(const Op_Log&) = delete;
	Op_Log& operator=(const Op_Log&) = delete;

private:
   std::shared_ptr<spdlog::async_logger> logger_;
};

// use embedded macro to support file and line number
#define KLOG_TRACE(...) SPDLOG_LOGGER_CALL(Op_Log::getInstance()->getLogger().get(), spdlog::level::trace, __VA_ARGS__)
#define KLOG_DEBUG(...) SPDLOG_LOGGER_CALL(Op_Log::getInstance()->getLogger().get(), spdlog::level::debug, __VA_ARGS__)
#define KLOG_INFO(...) SPDLOG_LOGGER_CALL(Op_Log::getInstance()->getLogger().get(), spdlog::level::info, __VA_ARGS__)
#define KLOG_WARN(...) SPDLOG_LOGGER_CALL(Op_Log::getInstance()->getLogger().get(), spdlog::level::warn, __VA_ARGS__)
#define KLOG_ERROR(...) SPDLOG_LOGGER_CALL(Op_Log::getInstance()->getLogger().get(), spdlog::level::err, __VA_ARGS__)

}
#endif