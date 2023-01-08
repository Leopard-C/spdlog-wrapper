/**
 * @file logger.h
 * @brief spdlog wrapper
 * @author Leopard-C (leopard.c@outlook.com)
 * @version 0.1
 * @date 2022-12-26
 * 
 * @copyright Copyright (c) 2022-present, Jinbao Chen.
 */
#ifndef IC_LOG_LOGGER_H_
#define IC_LOG_LOGGER_H_
#ifndef SPDLOG_COMPILED_LIB
    #define SPDLOG_COMPILED_LIB
#endif
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace ic {
namespace log {

class LoggerConfig;

class Logger {
public:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    ~Logger();

public:
    /* 程序启动后，必须先调用该函数 !!! */
    static void SetConfig(const LoggerConfig& config);
    static void SetConfig(std::shared_ptr<LoggerConfig> config);

    static std::shared_ptr<LoggerConfig> GetConfig() {
        return s_config;
    }

    static Logger& GetInstance() {
        static Logger s_instance;
        return s_instance;
    }

    std::shared_ptr<spdlog::logger> GetLogger() {
        return logger;
    }

private:
    std::shared_ptr<spdlog::logger> logger;
    static std::shared_ptr<LoggerConfig> s_config;
};

} // namespace log
} // namespace ic

#ifdef _WIN32
    #define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1) : __FILE__)
#else
    #define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)
#endif

/**
 * @brief 内部命名空间，仅供当前文件使用
 */
namespace _internal {

/**
 * @brief 给每条日志添加详细信息
 * 
 * @param  msg 原始日志内容
 * @param  filename 当前文件名
 * @param  function_name 当前函数名
 * @param  line_number 当前行号
 * @return std::string 添加上述详细信息后的日志内容
 */
inline std::string suffix(const char* msg, const char* filename, const char* function_name, int line_number) {
    std::string result;
    result.reserve(128);
    result += msg;
    result += " <";
    result += filename;
    result += "> <";
    result += function_name;
    result += "> <";
    result += std::to_string(line_number);
    result += ">";
    return result;
}

} // namespace _internal

#define LOG_STRING_(level_enum_value, msg) \
    ic::log::Logger::GetInstance().GetLogger()->log(spdlog::level::level_enum_value, msg)
#define LOG_SIMPLE_(level, ...) \
    ic::log::Logger::GetInstance().GetLogger()->level(__VA_ARGS__)
#define LOG_DETAIL_(level, msg, ...) \
    ic::log::Logger::GetInstance().GetLogger()->level(\
        _internal::suffix(msg, __FILENAME__, __func__, __LINE__).c_str(),\
        ##__VA_ARGS__\
    )

/*
 * 按照详细程度划分3种模式：
 *    LOG_SIMPLE:  简单，不输出当前函数名和源文件
 *    LOG_DETAIL:  详细，输出当前函数名和源文件
 *    LOG_DEFAULT: 默认，级别低于error的简单输出，否则详细输出  (默认！)
 */
#if defined LOG_SIMPLE
  #define LTrace(...)    LOG_SIMPLE_(trace, __VA_ARGS__)
  #define LDebug(...)    LOG_SIMPLE_(debug, __VA_ARGS__)
  #define LInfo(...)     LOG_SIMPLE_(info, __VA_ARGS__)
  #define LWarn(...)     LOG_SIMPLE_(warn, __VA_ARGS__)
  #define LError(...)    LOG_SIMPLE_(error, __VA_ARGS__)
  #define LCritical(...) LOG_SIMPLE_(critical, __VA_ARGS__)
#elif defined LOG_DETAIL
  #define LTrace(msg, ...)    LOG_DETAIL_(trace, msg, ##__VA_ARGS__)
  #define LDebug(msg, ...)    LOG_DETAIL_(debug, msg, ##__VA_ARGS__)
  #define LInfo(msg, ...)     LOG_DETAIL_(info, msg, ##__VA_ARGS__)
  #define LWarn(msg, ...)     LOG_DETAIL_(warn, msg, ##__VA_ARGS__)
  #define LError(msg, ...)    LOG_DETAIL_(error, msg, ##__VA_ARGS__)
  #define LCritical(msg, ...) LOG_DETAIL_(critical, msg, ##__VA_ARGS__)
#else
  #define LTrace(...)         LOG_SIMPLE_(trace, __VA_ARGS__)
  #define LDebug(...)         LOG_SIMPLE_(debug, __VA_ARGS__)
  #define LInfo(...)          LOG_SIMPLE_(info, __VA_ARGS__)
  #define LWarn(...)          LOG_SIMPLE_(warn, __VA_ARGS__)
  #define LError(msg, ...)    LOG_DETAIL_(error, msg, ##__VA_ARGS__)
  #define LCritical(msg, ...) LOG_DETAIL_(critical, msg, ##__VA_ARGS__)
#endif

#define LTraceString(msg)    LOG_STRING_(trace, msg)
#define LDebugString(msg)    LOG_STRING_(debug, msg)
#define LInfoString(msg)     LOG_STRING_(info, msg)
#define LWarnString(msg)     LOG_STRING_(warn, msg)
#define LErrorString(msg)    LOG_STRING_(err, msg)
#define LCriticalString(msg) LOG_STRING_(critical, msg)

#endif // IC_LOG_LOGGER_H_
