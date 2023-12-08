/**
 * @file simple_console_logger.h
 * @brief 打印内容到控制台，替代 printf, std::cout 等方法.
 * @author Leopard-C (leopard.c@outlook.com)
 * @version 0.1
 * @date 2022-12-26
 * 
 * @copyright Copyright (c) 2022-present, Jinbao Chen.
 */
#ifndef IC_LOG_SIMPLE_CONSOLE_LOGGER_H_
#define IC_LOG_SIMPLE_CONSOLE_LOGGER_H_
#ifndef SPDLOG_COMPILED_LIB
    #define SPDLOG_COMPILED_LIB
#endif
#include <spdlog/fmt/fmt.h>

namespace ic {
namespace log {

// print to console
template <typename T, typename... Args>
void Log(const T& format, const Args&... args) {
    fmt::print(format + std::string("\n"), args...);
}

} // namespace log
} // namespace ic

#endif // IC_LOG_SIMPLE_CONSOLE_LOGGER_H_
