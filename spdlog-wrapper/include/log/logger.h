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
#include "logger_config.h"
#include "simple_console_logger.h"

namespace ic {
namespace log {

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

    static Logger& GetInstance() {
        static Logger s_instance;
        return s_instance;
    }

    static std::shared_ptr<LoggerConfig> GetConfig() {
        return s_config;
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


/************************************************************************************
 * 
 *                               日志输出宏
 * 
 * 宏参数：
 *   + 1个参数：std::string 或者 const char*
 *   + N个参数：第1个参数必须是const char*
 * 
 * 使用示例：
 *  (1) std::string msg = "hello, world!";
 *      LInfo(msg);
 * 
 *  (2) const char* msg = "hello, world!";
 *      LInfo(msg);
 * 
 *  (3) std::string user_name = "Tom";
 *      int age = 19;
 *      LInfo("User '{}' logon, age: {}", user_name, age);
 * 
 *  (4) std::string err_msg = "invalid token";
 *      LError("Access denied. Error message: {}", err_msg);
 * 
 **********************************************************************************/
#define LTrace(...)    __IC_LOG_VFUNC(_IC_LOG_LTrace, __VA_ARGS__)
#define LDebug(...)    __IC_LOG_VFUNC(_IC_LOG_LDebug, __VA_ARGS__)
#define LInfo(...)     __IC_LOG_VFUNC(_IC_LOG_LInfo, __VA_ARGS__)
#define LWarn(...)     __IC_LOG_VFUNC(_IC_LOG_LWarn, __VA_ARGS__)
#define LError(...)    __IC_LOG_VFUNC(_IC_LOG_LError, __VA_ARGS__)
#define LCritical(...) __IC_LOG_VFUNC(_IC_LOG_LCritical, __VA_ARGS__)


/***********************************************************************************
 * 
 *                     以下内容为均为 辅助函数，仅供当前文件使用
 * 
 ***********************************************************************************/
#ifndef __IC_LOG__HELPER_FUNCTION_REGION
#define __IC_LOG__HELPER_FUNCTION_REGION

/**
 * @brief 内部命名空间，仅供当前文件使用
 */
namespace ic {
namespace log {
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
std::string suffix(const char* msg, const char* filename, const char* function_name, int line_number);

inline std::string suffix(const std::string& msg, const char* filename, const char* function_name, int line_number) {
    return suffix(msg.c_str(), filename, function_name, line_number);
}

} // namespace _internal
} // namespace log
} // namespace ic


#define __IC_LOG_FMT_(level_func, level_enum_value, msg, ...) \
    if (spdlog::level::level_enum::level_enum_value < ic::log::Logger::GetInstance().GetConfig()->detailed_min()) {\
        ic::log::Logger::GetInstance().GetLogger()->level_func(msg, ##__VA_ARGS__);\
    }\
    else {\
        ic::log::Logger::GetInstance().GetLogger()->level_func(\
            ic::log::_internal::suffix(msg, __FILE__, __func__, __LINE__).c_str(),\
            ##__VA_ARGS__\
        );\
    }
#define __IC_LOG_STRING_(level_enum_value, msg) \
    if (spdlog::level::level_enum::level_enum_value < ic::log::Logger::GetInstance().GetConfig()->detailed_min()) {\
        ic::log::Logger::GetInstance().GetLogger()->log(spdlog::level::level_enum::level_enum_value, msg);\
    }\
    else {\
        ic::log::Logger::GetInstance().GetLogger()->log(\
            spdlog::level::level_enum::level_enum_value,\
            ic::log::_internal::suffix(msg, __FILE__, __func__, __LINE__)\
        );\
    }

/**
 * @brief 宏函数重载
 */
#define __IC_LOG_EXPAND__(...) __VA_ARGS__
#define __IC_LOG_NARG__(...)  __IC_LOG_EXPAND__(__IC_LOG_NARG_I_(__VA_ARGS__, __IC_LOG_RESQ_N()))
#define __IC_LOG_NARG_I_(...) __IC_LOG_EXPAND__(__IC_LOG_ARG_N(__VA_ARGS__))
#define __IC_LOG_ARG_N(\
     _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
    _61,_62,_63,N,...) N
#define __IC_LOG_RESQ_N() \
    N,N,N,N,             \
    N,N,N,N,N,N,N,N,N,N, \
    N,N,N,N,N,N,N,N,N,N, \
    N,N,N,N,N,N,N,N,N,N, \
    N,N,N,N,N,N,N,N,N,N, \
    N,N,N,N,N,N,N,N,N,N, \
    N,N,N,N,N,N,N,N,1,0
#define __IC_LOG_VFUNC__(name, n) name##_##n##_
#define __IC_LOG_VFUNC_(name, n)  __IC_LOG_VFUNC__(name, n)
#define __IC_LOG_VFUNC(func, ...) __IC_LOG_EXPAND__(__IC_LOG_VFUNC_(func, __IC_LOG_NARG__(__VA_ARGS__)) (__VA_ARGS__))

/**
 * @brief 单个参数.
 */
#define _IC_LOG_LTrace_1_(msg)    __IC_LOG_STRING_(trace, msg)
#define _IC_LOG_LDebug_1_(msg)    __IC_LOG_STRING_(debug, msg)
#define _IC_LOG_LInfo_1_(msg)     __IC_LOG_STRING_(info, msg)
#define _IC_LOG_LWarn_1_(msg)     __IC_LOG_STRING_(warn, msg)
#define _IC_LOG_LError_1_(msg)    __IC_LOG_STRING_(err, msg)
#define _IC_LOG_LCritical_1_(msg) __IC_LOG_STRING_(critical, msg)

/**
 * @brief N个参数，(N>1).
 */
#define _IC_LOG_LTrace_N_(msg, ...)    __IC_LOG_FMT_(trace, trace, msg, ##__VA_ARGS__)
#define _IC_LOG_LDebug_N_(msg, ...)    __IC_LOG_FMT_(debug, debug, msg, ##__VA_ARGS__)
#define _IC_LOG_LInfo_N_(msg, ...)     __IC_LOG_FMT_(info, info, msg, ##__VA_ARGS__)
#define _IC_LOG_LWarn_N_(msg, ...)     __IC_LOG_FMT_(warn, warn, msg, ##__VA_ARGS__)
#define _IC_LOG_LError_N_(msg, ...)    __IC_LOG_FMT_(error, err, msg, ##__VA_ARGS__)
#define _IC_LOG_LCritical_N_(msg, ...) __IC_LOG_FMT_(critical, critical, msg, ##__VA_ARGS__)

#endif // __IC_LOG__HELPER_FUNCTION_REGION

#endif // IC_LOG_LOGGER_H_
