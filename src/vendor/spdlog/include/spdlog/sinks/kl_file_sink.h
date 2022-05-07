// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
// Created by kunlundb 2022-05-07

#pragma once

#include <spdlog/common.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/chrono.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/os.h>
#include <spdlog/details/circular_q.h>
#include <spdlog/details/synchronous_factory.h>

#include <dirent.h>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <string>

namespace spdlog {
namespace sinks {

/*
 * Rotating file sink based on date.
 * If truncate != false , the created file will be truncated.
 * If max_files > 0, retain only the last max_files and delete previous.
 */
template<typename Mutex>
class kl_file_sink final : public base_sink<Mutex> {
public:
    // create daily file sink which rotates on given time
    kl_file_sink(filename_t base_filename, size_t max_size, int rotation_hour, int rotation_minute, bool truncate = false, 
        uint16_t max_files = 0, const file_event_handlers &event_handlers = {})
        : base_filename_(std::move(base_filename))
        , rotation_h_(rotation_hour)
        , rotation_m_(rotation_minute)
        , file_helper_{event_handlers}
        , truncate_(truncate)
        , max_size_(max_size)
        , max_files_(max_files)
        , filenames_q_()
    {
        if (rotation_hour < 0 || rotation_hour > 23 || rotation_minute < 0 || rotation_minute > 59)
        {
            throw_spdlog_ex("daily_file_sink: Invalid rotation time in ctor");
        }

        file_no_ = get_file_no_(base_filename_);
        auto now = log_clock::now();
        auto filename = daily_filename_(base_filename_, now_tm(now));
        file_helper_.open(filename, truncate_);
        current_size_ = file_helper_.size();
        rotation_tp_ = next_rotation_tp_();

        if (max_files_ > 0)
        {
            init_filenames_q_();
        }
    }

    filename_t filename()
    {
        std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        return file_helper_.filename();
    }

protected:
    void sink_it_(const details::log_msg &msg) override
    {
        memory_buf_t formatted;
        base_sink<Mutex>::formatter_->format(msg, formatted);
        current_size_ += formatted.size();

        auto time = msg.time;
        bool should_rotate = false;
        if(time >= rotation_tp_) {
            should_rotate = true;
            file_helper_.close();
            file_no_ = 0;
            auto filename = daily_filename_(base_filename_, now_tm(time));
            file_helper_.open(filename, truncate_);
            current_size_ = file_helper_.size();
            rotation_tp_ = next_rotation_tp_();
        } else if(current_size_ >= max_size_) {
            should_rotate = true;
            file_helper_.close();
            file_no_++;
            auto filename = daily_filename_(base_filename_, now_tm(time));
            file_helper_.open(filename, truncate_);
            current_size_ = file_helper_.size();
            rotation_tp_ = next_rotation_tp_();
        }

        file_helper_.write(formatted);

        // Do the cleaning only at the end because it might throw on failure.
        if (should_rotate && max_files_ > 0)
        {
            delete_old_();
        }
    }

    void flush_() override
    {
        file_helper_.flush();
    }

private:
    void init_filenames_q_()
    {
        using details::os::path_exists;

        filenames_q_ = details::circular_q<filename_t>(static_cast<size_t>(max_files_));
        std::vector<filename_t> filenames;
        auto now = log_clock::now();
        while (filenames.size() < max_files_)
        {
            auto filename = daily_filename_(base_filename_, now_tm(now));
            if (!path_exists(filename))
            {
                break;
            }
            filenames.emplace_back(filename);
            now -= std::chrono::hours(24);
        }
        for (auto iter = filenames.rbegin(); iter != filenames.rend(); ++iter)
        {
            filenames_q_.push_back(std::move(*iter));
        }
    }

    tm now_tm(log_clock::time_point tp)
    {
        time_t tnow = log_clock::to_time_t(tp);
        return spdlog::details::os::localtime(tnow);
    }

    log_clock::time_point next_rotation_tp_()
    {
        auto now = log_clock::now();
        tm date = now_tm(now);
        date.tm_hour = rotation_h_;
        date.tm_min = rotation_m_;
        date.tm_sec = 0;
        auto rotation_time = log_clock::from_time_t(std::mktime(&date));
        if (rotation_time > now)
        {
            return rotation_time;
        }
        return {rotation_time + std::chrono::hours(24)};
    }

    // Delete the file N rotations ago.
    // Throw spdlog_ex on failure to delete the old file.
    void delete_old_()
    {
        using details::os::filename_to_str;
        using details::os::remove_if_exists;

        filename_t current_file = file_helper_.filename();
        if (filenames_q_.full())
        {
            auto old_filename = std::move(filenames_q_.front());
            filenames_q_.pop_front();
            bool ok = remove_if_exists(old_filename) == 0;
            if (!ok)
            {
                filenames_q_.push_back(std::move(current_file));
                throw_spdlog_ex("Failed removing daily file " + filename_to_str(old_filename), errno);
            }
        }
        filenames_q_.push_back(std::move(current_file));
    }

    filename_t daily_filename_(const filename_t &filename, const tm &now_tm) {
        filename_t basename, ext;
        std::tie(basename, ext) = details::file_helper::split_by_extension(filename);
        return fmt_lib::format(
            SPDLOG_FILENAME_T("{}.{:04d}-{:02d}-{:02d}{}.{}"), basename, now_tm.tm_year + 1900, now_tm.tm_mon + 1, 
                            now_tm.tm_mday, ext, file_no_);
    }

    int get_file_no_(const filename_t &filename) {
        int file_no = 0;
        filename_t basename, ext;
        std::tie(basename, ext) = details::file_helper::split_by_extension(filename);
        auto ext_index = basename.rfind('/');
        filename_t base_path = basename.substr(0, ext_index);
        filename_t base_file = basename.substr(ext_index+1);
        
        tm ntm = now_tm(log_clock::now());
        filename_t file_prefix = fmt_lib::format(
            SPDLOG_FILENAME_T("{}.{:04d}-{:02d}-{:02d}{}"), base_file, ntm.tm_year + 1900, ntm.tm_mon + 1, 
                            ntm.tm_mday, ext);
        DIR *dirp = opendir(base_path.c_str());
        if(dirp) {
            struct dirent *dp;
            while(1) {
                dp = readdir(dirp);
                if(!dp)
                    break;
                
                if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
                    continue;
                
                if(strncmp(dp->d_name, file_prefix.c_str(), file_prefix.length()) == 0) {
                    std::string file = std::string(dp->d_name);
                    std::string s_no = file.substr(file.rfind(".")+1);
                    int no = atoi(s_no.c_str());
                    if(no > file_no)
                        file_no = no;
                }
            } 
        }

        return file_no;  
    }

    filename_t base_filename_;
    int rotation_h_;
    int rotation_m_;
    log_clock::time_point rotation_tp_;
    details::file_helper file_helper_;
    bool truncate_;
    size_t max_size_;
    uint16_t max_files_;
    size_t current_size_;
    int file_no_;
    details::circular_q<filename_t> filenames_q_;
};

using kl_file_sink_mt = kl_file_sink<std::mutex>;
using kl_file_sink_st = kl_file_sink<details::null_mutex>;

} // namespace sinks

//
// factory functions
//
template<typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> kl_logger_mt(const std::string &logger_name, const filename_t &filename, size_t size = 10*1024*1024, int hour = 0, int minute = 0,
    bool truncate = false, uint16_t max_files = 0, const file_event_handlers &event_handlers = {})
{
    return Factory::template create<sinks::kl_file_sink_mt>(logger_name, filename, size, hour, minute, truncate, max_files, event_handlers);
}

template<typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> kl_logger_st(const std::string &logger_name, const filename_t &filename, size_t size = 10*1024*1024, int hour = 0, int minute = 0,
    bool truncate = false, uint16_t max_files = 0, const file_event_handlers &event_handlers = {})
{
    return Factory::template create<sinks::kl_file_sink_st>(logger_name, filename, size, hour, minute, truncate, max_files, event_handlers);
}

} // namespace spdlog