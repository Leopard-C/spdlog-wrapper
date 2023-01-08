#include "log/simple_console_logger.h"
#include "log/logger_config.h"

#define LOG_DEFAULT
#include "log/logger.h"

bool InitLogger() {
    ic::log::LoggerConfig config;
    if (!config.ReadFromFile("config/log.ini")) {
        ic::log::Log("Read file config/log.ini failed.");
        return false;
    }
    ic::log::Logger::SetConfig(config);
    return true;
}

int main() {
    if (!InitLogger()) {
        return 1;
    }

    LTrace("Application start!");
    LDebug("Application start!");
    LInfo("Application start!");
    LWarn("Application start!");
    LError("Application start!");
    LCritical("Application start!");

    int a = 7;
    int b = 8;
    LInfo("ADD: {} + {} = {}", a, b, a + b);

    std::string err_msg = "open file failed";
    LError("{}", err_msg);

    err_msg = "file opened";
    LInfo("{}", err_msg);

    return 0;
}
