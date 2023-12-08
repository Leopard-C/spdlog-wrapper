/**
 * @file logger_config.h
 * @brief 日志配置.
 * @author Leopard-C (leopard.c@outlook.com)
 * @version 0.1
 * @date 2022-12-26
 * 
 * @copyright Copyright (c) 2022-present, Jinbao Chen.
 */
#ifndef IC_LOG_LOGGER_CONFIG_H_
#define IC_LOG_LOGGER_CONFIG_H_
#ifndef SPDLOG_COMPILED_LIB
    #define SPDLOG_COMPILED_LIB
#endif
#include <map>
#include <vector>
#include <spdlog/spdlog.h>

namespace ic {
namespace log {

class LoggerConfig {
public:
    /**
     * @brief 换行符类型.
     */
    enum class LineFeed {
        LF,    /* unix平台 \n       */
        CRLF,  /* windows平台 \r\n  */
        AUTO   /* 根据平台自动选择   */
    };

    /**
     * @brief 日志中出现的的文件名类型.
     */
    enum class DetailedFilenameType {
        FullPath,
        NameOnly
    };

public:
    /**
     * @brief 控制台日志配置.
     */
    struct ConsoleConfig {
    public:
        ConsoleConfig();

        /**
         * @brief 从键值对中读取配置.
         */
        bool Parse(std::map<std::string, std::string>& key_values);

        /**
         * @brief 序列化.
         */
        std::map<std::string, std::string> Serialize() const;

        void set_level(spdlog::level::level_enum level) { level_ = level; }
        void set_pattern(const std::string& pattern) { pattern_ = pattern; }

        spdlog::level::level_enum level() const { return level_; }
        const std::string& pattern() const { return pattern_; }

        /**
         * @brief 是否带有颜色显示.
         */
        bool Color() const;

    protected:
        /** 日志级别 */
        spdlog::level::level_enum level_;
        /** 样式 */
        std::string pattern_;
    };

    /**
     * @brief 文件日志配置.
     */
    struct FileConfig {
    public:
        FileConfig();

        /**
         * @brief 从键值对中读取配置.
         */
        bool Parse(std::map<std::string, std::string>& key_values);

        /**
         * @brief 序列化.
         */
        std::map<std::string, std::string> Serialize() const;

        /**
         * @brief 获取实际存储目录.
         *
         * @details 包括替换掉内置宏 ${bin}、创建目录等操作.
         */
        std::string GetDirectory() const;

        /**
         * @brief 获取日志文件名(目录+文件名称+后缀，如`/var/log/app/log_20220101.txt`).
         */
        std::string GetFilename() const;

        void set_level(spdlog::level::level_enum level) { level_ = level; }
        void set_pattern(const std::string& pattern) { pattern_ = pattern; }
        void set_directory(const std::string& directory);
        void set_name(const std::string& name);
        void set_ext(const std::string& ext);

        spdlog::level::level_enum level() const { return level_; }
        const std::string& pattern() const { return pattern_; }
        const std::string& directory() const { return directory_; }
        const std::string& name() const { return name_; }
        const std::string& ext() const { return ext_; }

    protected:
        /** 日志级别 */
        spdlog::level::level_enum level_;
        /** 存储目录 */
        std::string directory_;
        /** 日志文件名称(不包含目录，不包括后缀) */
        std::string name_;
        /** 日志文件名后缀 */
        std::string ext_;
        /** 格式 */
        std::string pattern_;
    };

    /**
     * @brief 每日日志.
     * 
     * @details 每天的日志记录在一个独立的文件中.
     */
    struct DailyFileConfig : public FileConfig {
    public:
        DailyFileConfig();
        bool Parse(std::map<std::string, std::string>& key_values);
        std::map<std::string, std::string> Serialize() const;
    };

    /**
     * @brief 滚动日志.
     * 
     * @details 日志存满后，新日志覆盖旧日志.
     */
    struct RotatingFileConfig : public FileConfig {
    public:
        RotatingFileConfig();

        /**
         * @brief 从键值对中读取配置.
         */
        bool Parse(std::map<std::string, std::string>& key_values);

        /**
         * @brief 序列化.
         */
        std::map<std::string, std::string> Serialize() const;

        void set_max_files_count(size_t count) { max_files_count_ = count; }
        void set_max_file_size(size_t size) { max_file_size_ = size; }

        size_t max_files_count() const { return max_files_count_; }
        size_t max_file_size() const { return max_file_size_; }

    protected:
        /** 最多多少个日志文件，如`log.1 log.2 log.3 ...` */
        size_t max_files_count_;
        /** 每个文件大小的上限 */
        size_t max_file_size_;
    };

public:
    LoggerConfig();

    /**
     * @brief 初始化为默认配置.
     */
    void InitDefault();

    /**
     * @brief 从.ini配置文件读取配置信息.
     */
    bool ReadFromFile(const std::string& filename);

    /**
     * @brief 序列化.
     */
    std::map<std::string, std::map<std::string, std::string>> Serialize() const;

    /**
     * @brief 将配置信息打印到控制台.
     */
    void DumpToConsole() const;

    /**
     * @brief 将配置信息写入文件.
     */
    bool WriteToFile(const std::string& filename, LineFeed line_feed = LineFeed::AUTO);

    /**
     * @brief 将默认配置(示例)写入配置文件.
     */
    static bool WriteDefaultConfigToFile(const std::string& filename, LineFeed line_feed = LineFeed::AUTO);

    spdlog::level::level_enum detailed_min() const { return detailed_min_; }
    DetailedFilenameType detailed_filename_type() const { return detailed_filename_type_; }
    spdlog::level::level_enum flush_on() const { return flush_on_; }
    size_t flush_every() const { return flush_every_; }
    const std::string& name() const { return name_; }
    const std::vector<ConsoleConfig>& console_configs() const { return console_configs_; }
    const std::vector<DailyFileConfig>& daily_file_configs() const { return daily_file_configs_; }
    const std::vector<RotatingFileConfig>& rotating_file_configs() const { return rotating_file_configs_; }

    void set_detailed_min(spdlog::level::level_enum detailed_min) { detailed_min_ = detailed_min; }
    void set_detailed_filename_type(DetailedFilenameType type) { detailed_filename_type_ = type; }
    void set_flush_on(spdlog::level::level_enum flush_on) { flush_on_ = flush_on; }
    void set_flush_every(size_t flush_every) { flush_every_ = flush_every; }
    void set_name(const std::string& name);
    void add_console_config(const ConsoleConfig& config) { console_configs_.push_back(config); }
    void add_daily_file_config(const DailyFileConfig& config) { daily_file_configs_.push_back(config); }
    void add_rotating_file_config(const RotatingFileConfig& config) { rotating_file_configs_.push_back(config); }

private:
    bool ParseBasic(std::map<std::string, std::string>& key_values);

private:
    /**
     * @brief 低于该级别的简洁输出（只输出日志内容），等于或高于该级别的详细输出（日志内容+函数名、文件名、行号）.
     * 
     * @details 例如
     * @details (1) detailed_min = error ，则
     * @details    trace, debug, info, warn ==> 简洁输出
     * @details    error, critical  ==> 详细输出
     * @details (2) detailed_min = trace，则任何级别都详细输出
     * @details (3) detailed_min = off，则任何级别都简洁输出
     */
    spdlog::level::level_enum detailed_min_;
    /** 打印详细日志时的文件名类型(完整路径、仅文件名) */
    DetailedFilenameType detailed_filename_type_;
    /** 刷新时机（如出现错误时刷新，立即写入文件） */
    spdlog::level::level_enum flush_on_;
    /** 每隔多长时间刷新，单位：秒 */
    size_t flush_every_;
    /** 日志记录器名称 */
    std::string name_;

    /** 所有的控制台日志配置信息 */
    std::vector<ConsoleConfig> console_configs_;
    /** 所有的每日日志配置信息 */
    std::vector<DailyFileConfig> daily_file_configs_;
    /** 所有的滚动日志配置信息 */
    std::vector<RotatingFileConfig> rotating_file_configs_;
};

} // namespace log
} // namespace ic

#endif // IC_LOG_LOGGER_CONFIG_H_
