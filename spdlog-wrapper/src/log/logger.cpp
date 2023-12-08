#include "logger.h"
#include "logger_config.h"
#include "simple_console_logger.h"
#include <spdlog/common.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace ic {
namespace log {

std::shared_ptr<LoggerConfig> Logger::s_config;

void Logger::SetConfig(const LoggerConfig& config) {
    s_config = std::make_shared<LoggerConfig>(config);
}

void Logger::SetConfig(std::shared_ptr<LoggerConfig> config) {
    s_config = config;
}

Logger::Logger() {
    /* 至少有1个sink */
    if (s_config->console_configs().empty() &&
        s_config->daily_file_configs().empty() &&
        s_config->rotating_file_configs().empty())
    {
        s_config->add_console_config({});
    }

    std::vector<spdlog::sink_ptr> sinks;
    spdlog::level::level_enum min_level = spdlog::level::err;

    /* 控制台 */
    for (auto& config : s_config->console_configs()) {
        if (config.level() < min_level) {
            min_level = config.level();
        }
        if (config.Color()) {
            auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sink->set_color_mode(spdlog::color_mode::always);
            sink->set_level(config.level());
            sink->set_pattern(config.pattern());
            sinks.push_back(sink);
        }
        else {
            auto sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
            sink->set_level(config.level());
            sink->set_pattern(config.pattern());
            sinks.push_back(sink);
        }
    }
    /* 每日日志 */
    for (auto& config : s_config->daily_file_configs()) {
        if (config.level() < min_level) {
            min_level = config.level();
        }
        /* 每天0点0分，创建新的日志文件 */
        auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(config.GetFilename(), 0, 0);
        sink->set_level(config.level());
        sink->set_pattern(config.pattern());
        sinks.push_back(sink);
    }
    /* 滚动日志 */
    for (auto& config : s_config->rotating_file_configs()) {
        if (config.level() < min_level) {
            min_level = config.level();
        }
        auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            config.GetFilename(), config.max_file_size(), config.max_files_count());
        sink->set_level(config.level());
        sink->set_pattern(config.pattern());
        sinks.push_back(sink);
    }

    logger = std::make_shared<spdlog::logger>(s_config->name(), std::begin(sinks), std::end(sinks));
    logger->set_level(min_level);
    logger->flush_on(s_config->flush_on());
    spdlog::register_logger(logger);
    spdlog::flush_every(std::chrono::seconds(s_config->flush_every()));
    SPDLOG_LOGGER_DEBUG(logger, "SpdDebug");
}

Logger::~Logger() {
    spdlog::drop_all();
}

namespace _internal {

std::string suffix(const char* msg, const char* filename, const char* function_name, int line_number) {
    auto config = ic::log::Logger::GetConfig();
    std::string result;
    result.reserve(128);
    result += msg;
    result += " <";
    result += (config->detailed_filename_type() == LoggerConfig::DetailedFilenameType::FullPath) ?
              filename :
#ifdef _WIN32
              (strrchr(filename, '\\') ? (strrchr(filename, '\\') + 1) : filename);
#else
              (strrchr(filename, '/') ? (strrchr(filename, '/') + 1) : filename);
#endif
    result += "> <";
    result += function_name;
    result += "> <";
    result += std::to_string(line_number);
    result += ">";
    return result;
}

} // namespace _internal

} // namespace log
} // namespace ic
