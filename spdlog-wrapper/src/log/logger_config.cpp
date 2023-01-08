#include "logger_config.h"
#include "simple_console_logger.h"
#include "ini.h"
#include "util.h"

namespace ic {
namespace log {

static const spdlog::level::level_enum s_default_level = spdlog::level::trace;
static const spdlog::level::level_enum s_default_flush_on = spdlog::level::trace;
static const std::string s_default_pattern = "[%H:%M:%S.%e] [%l] %v";
static const std::string s_default_pattern_with_color = "[%H:%M:%S.%e] %^[%l]%$ %v";
static const std::string s_default_directory = "${bin}/logs/";
static const std::string s_default_name = "log";
static const int s_default_flush_every = 1;
static const size_t s_default_max_files_count = 10;
static const size_t s_default_max_file_size = 1024 * 1025 * 5; /* 5MB */

LoggerConfig::ConsoleConfig::ConsoleConfig() :
    level_(s_default_level), pattern_(s_default_pattern_with_color)
{
}

/**
 * @brief 是否带有颜色显示.
 */
bool LoggerConfig::ConsoleConfig::Color() const {
    auto pos1 = pattern_.find("%^");
    if (pos1 == std::string::npos) {
        return false;
    }
    auto pos2 = pattern_.find("%$", pos1 + 3);
    if (pos2 == std::string::npos) {
        return false;
    }
    return true;
}

LoggerConfig::FileConfig::FileConfig()
    : level_(s_default_level), name_(s_default_name),
    directory_(s_default_directory), pattern_(s_default_pattern)
{
}

LoggerConfig::DailyFileConfig::DailyFileConfig()
{
    set_name("daily");
}

LoggerConfig::RotatingFileConfig::RotatingFileConfig()
    : max_files_count_(s_default_max_files_count), max_file_size_(s_default_max_file_size)
{
    set_name("log");
}

LoggerConfig::LoggerConfig()
    : flush_on_(s_default_flush_on), flush_every_(s_default_flush_every), name_(s_default_name)
{
}

void LoggerConfig::FileConfig::set_directory(const std::string& directory) {
    directory_ = directory;
    log::util::trim(directory_);
}

void LoggerConfig::FileConfig::set_name(const std::string& name) {
    name_ = name;
    util::trim(name_);
}

void LoggerConfig::FileConfig::set_ext(const std::string& ext) {
    ext_ = ext;
    util::trim(ext_);
}

/**
 * @brief 获取实际存储目录.
 * 
 * @details 包括替换掉内置宏 ${bin}、创建目录等操作.
 */
std::string LoggerConfig::FileConfig::GetDirectory() const {
    /* 替换宏：${bin}，为可执行文件实际路径 */
    std::string directory_tmp = directory_;
    util::trim(directory_tmp);
    std::string bin_token = "${bin}";
    auto pos = directory_tmp.find(bin_token);
    if (pos == 0) {
        std::string bin_dir = util::get_bin_dir();
        const size_t bin_token_len = bin_token.length();
        if (bin_token_len < directory_tmp.length()) {
            if (directory_tmp[bin_token_len] == '/' || directory_tmp[bin_token_len] == '\\') {
                directory_tmp = bin_dir + directory_tmp.substr(bin_token_len + 1);
            }
            else {
                directory_tmp = bin_dir + directory_tmp.substr(bin_token_len);
            }
        }
        else {
            directory_tmp = bin_dir;
        }
    }
    directory_tmp = util::format_dir(directory_tmp);

    /* 如果目录不存在，就创建目录，如果创建失败，直接退出进程 */
    if (!util::create_dir(directory_tmp, true)) {
        Log("Error: Create log directory '{}' failed", directory_tmp);
        exit(100);
    }

    return directory_tmp;
}

/**
 * @brief 获取日志文件名(目录+文件名称+后缀，如`/var/log/app/log_20220101.txt`).
 */
std::string LoggerConfig::FileConfig::GetFilename() const {
    std::string filename = GetDirectory();
    if (name_.empty()) {
        filename += s_default_name;
    }
    else {
        filename += name_;
    }
    filename += util::format_ext(ext_);
    return filename;
}

#define CHECK_KEY_VALUES(keys) \
    for (size_t i = 0; i < sizeof(keys) / sizeof(char*); ++i) {\
        if (key_values.find(keys[i]) == key_values.end()) {\
            Log("Error: Missing key '{}'", keys[i]);\
            return false;\
        }\
        else if (key_values[keys[i]].empty()) {\
            Log("Error: Value of key '{}' is empty", keys[i]);\
            return false;\
        }\
    }

#define GET_NAME() \
    name_ = key_values["name"]

#define GET_EXT() \
    ext_ = key_values["ext"]

#define GET_DIRECTORY() \
    directory_ = key_values["directory"]

#define GET_LEVEL() \
    {\
        auto tmp = spdlog::level::from_str(key_values["level"]);\
        if (tmp == spdlog::level::off) {\
            Log("Error: Value of key 'level' is invalid");\
            return false;\
        }\
        level_ = tmp;\
    }

#define GET_PATTERN() \
    pattern_ = key_values["pattern"]

#define GET_MAX_FILES_COUNT() \
    {\
        auto tmp = key_values["max_files_count"];\
        for (auto c : tmp) {\
            if (c < '0' || c > '9') {\
                Log("Error: Value of key 'max_files_count' is invalid");\
                return false;\
            }\
        }\
        max_files_count_ = std::stoul(tmp);\
    }

#define GET_MAX_FILE_SIZE() \
    {\
        uint64_t tmp;\
        if (!util::parse_filesize(key_values["max_file_size"], tmp)) {\
            Log("Error: Value of key 'max_file_size' is invalid");\
            return false;\
        }\
        max_file_size_ = static_cast<size_t>(tmp);\
    }

#define GET_FLUSH_EVERY() \
    {\
        auto tmp = key_values["flush_every"];\
        for (auto c : tmp) {\
            if (c < '0' || c > '9') {\
                Log("Error: Value of key 'flush_every' is invalid");\
                return false;\
            }\
        }\
        flush_every_ = std::stoul(tmp);\
    }

#define GET_FLUSH_ON() \
    {\
        auto tmp = spdlog::level::from_str(key_values["flush_on"]);\
        if (tmp == spdlog::level::off) {\
            Log("Error: Value of key 'flush_on' is invalid");\
            return false;\
        }\
        flush_on_ = tmp;\
    }

/**
 * @brief 从键值对读取配置信息.
 */
bool LoggerConfig::ConsoleConfig::Parse(std::map<std::string, std::string>& key_values) {
    static const char* s_console_config_keys[] = { "level", "pattern" };
    CHECK_KEY_VALUES(s_console_config_keys);
    GET_LEVEL();
    GET_PATTERN();
    return true;
}

bool LoggerConfig::FileConfig::Parse(std::map<std::string, std::string>& key_values) {
    static const char* s_file_config_keys[] = { "name", "ext", "directory", "level", "pattern" };
    CHECK_KEY_VALUES(s_file_config_keys);
    GET_NAME();
    GET_EXT();
    GET_DIRECTORY();
    GET_LEVEL();
    GET_PATTERN();
    return true;
}

bool LoggerConfig::DailyFileConfig::Parse(std::map<std::string, std::string>& key_values) {
    return FileConfig::Parse(key_values);
}

bool LoggerConfig::RotatingFileConfig::Parse(std::map<std::string, std::string>& key_values) {
    static const char* s_rotating_file_config_keys[] = { "max_files_count", "max_file_size" };
    CHECK_KEY_VALUES(s_rotating_file_config_keys);
    GET_MAX_FILES_COUNT();
    GET_MAX_FILE_SIZE();
    return FileConfig::Parse(key_values);
}

bool LoggerConfig::ParseBasic(std::map<std::string, std::string>& key_values) {
    static const char* s_basic_keys[] = { "name", "flush_every", "flush_on" };
    CHECK_KEY_VALUES(s_basic_keys);
    GET_NAME();
    GET_FLUSH_EVERY();
    GET_FLUSH_ON();
    return true;
}

/**
 * @brief 从.ini配置文件读取配置信息.
 */
bool LoggerConfig::ReadFromFile(const std::string& filename) {
    IniHelper ini;
    if (!ini.ParseFile(filename)) {
        Log("Error: Parse file '{}' failed. {}", filename, ini.GetLastErrorMsg());
        return false;
    }

    auto& sections = ini.sections();
    if (sections.find("basic") == sections.end()) {
        Log("Error: Missing section 'basic' in file '{}'", filename);
        return false;
    }
    auto& basic = sections["basic"];
    if (!ParseBasic(basic)) {
        Log("Error: Parse section 'basic' failed in file '{}'", filename);
        return false;
    }

    for (auto& p : sections) {
        if (p.first == "basic") {
            continue;
        }
        else if (util::starts_with(p.first, "console")) {
            ConsoleConfig config;
            if (!config.Parse(p.second)) {
                Log("Error: Parse section '{}' failed in file '{}'", p.first, filename);
                return false;
            }
            console_configs_.push_back(config);
        }
        else if (util::starts_with(p.first, "daily")) {
            DailyFileConfig config;
            if (!config.Parse(p.second)) {
                Log("Error: Parse section '{}' failed in file '{}'", p.first, filename);
                return false;
            }
            daily_file_configs_.push_back(config);
        }
        else if (util::starts_with(p.first, "rotating")) {
            RotatingFileConfig config;
            if (!config.Parse(p.second)) {
                Log("Error: Parse section '{}' failed in file '{}'", p.first, filename);
                return false;
            }
            rotating_file_configs_.push_back(config);
        }
        else {
            Log("Error: Unknown section '{}' in file '{}'", p.first, filename);
            return false;
        }
    }

    return true;
}

static std::string level_to_string(spdlog::level::level_enum level) {
    auto tmp = spdlog::level::to_string_view(level);
    return std::string(tmp.data(), tmp.size());
}

/**
 * @brief 序列化.
 */
std::map<std::string, std::string> LoggerConfig::ConsoleConfig::Serialize() const {
    std::map<std::string, std::string> result;
    result["level"] = level_to_string(level_);
    result["pattern"] = pattern_;
    return result;
}

std::map<std::string, std::string> LoggerConfig::FileConfig::Serialize() const {
    std::map<std::string, std::string> result;
    result["name"] = name_;
    result["ext"] = ext_;
    result["directory"] = directory_;
    result["level"] = level_to_string(level_);
    result["pattern"] = pattern_;
    return result;
}

std::map<std::string, std::string> LoggerConfig::DailyFileConfig::Serialize() const {
    return FileConfig::Serialize();
}

std::map<std::string, std::string> LoggerConfig::RotatingFileConfig::Serialize() const {
    auto result = FileConfig::Serialize();
    result["max_files_count"] = std::to_string(max_files_count_);
    result["max_file_size"] = util::format_filesize(max_file_size_, 2);
    return FileConfig::Serialize();
}

/**
 * @brief 序列化.
 */
std::map<std::string, std::map<std::string, std::string>> LoggerConfig::Serialize() const {
    std::map<std::string, std::map<std::string, std::string>> result;
    // basic
    {
        std::map<std::string, std::string> basic;
        basic["name"] = name_;
        basic["flush_every"] = std::to_string(flush_every_);
        basic["flush_on"] = level_to_string(flush_on_);
        result.emplace("basic", basic);
    }
    // console
    for (size_t i = 0, count = console_configs_.size(); i < count; ++i) {
        auto name = "console" + (count == 1 ? std::string() : std::string("-" + std::to_string(i+1)));
        auto value = console_configs_[i].Serialize();
        result.emplace(name, value);
    }
    // daily
    for (size_t i = 0, count = daily_file_configs_.size(); i < count; ++i) {
        auto name = "daily" + (count == 1 ? std::string() : std::string("-" + std::to_string(i+1)));
        auto value = daily_file_configs_[i].Serialize();
        result.emplace(name, value);
    }
    // rotating
    for (size_t i = 0, count = rotating_file_configs_.size(); i < count; ++i) {
        auto name = "rotating" + (count == 1 ? std::string() : std::string("-" + std::to_string(i+1)));
        auto value = rotating_file_configs_[i].Serialize();
        result.emplace(name, value);
    }
    return result;
}

/**
 * @brief 初始化为默认配置.
 */
void LoggerConfig::InitDefault() {
    *this = LoggerConfig();
    console_configs_.push_back({});
    daily_file_configs_.push_back({});
    rotating_file_configs_.push_back({});
}

/**
 * @brief 将配置信息打印到控制台.
 */
void LoggerConfig::DumpToConsole() const {
    IniHelper ini;
    ini.sections() = this->Serialize();
    std::cout << ini.ToString();
}

/**
 * @brief 将配置信息写入文件.
 */
bool LoggerConfig::WriteToFile(const std::string& filename, LineFeed line_feed/* = LineFeed::AUTO*/) {
    IniHelper ini;
    ini.sections() = this->Serialize();
    if (!ini.WriteToFile(filename, IniHelper::LineFeed(line_feed))) {
        Log("Error: Write logger config to file failed. {}", ini.GetLastErrorMsg());
        return false;
    }
    return true;
}

/**
 * @brief 将默认配置(示例)写入配置文件.
 */
bool LoggerConfig::WriteDefaultConfigToFile(const std::string& filename, LineFeed line_feed/* = LineFeed::AUTO*/) {
    LoggerConfig config;
    config.InitDefault();
    return config.WriteToFile(filename, line_feed);
}

void LoggerConfig::set_name(const std::string& name) {
    name_ = name;
    util::trim(name_);
    if (name_.empty()) {
        name_ = "log";
    }
}

} // namespace log
} // namespace ic
