/**
 * @file ini.h
 * @brief ini配置文件读写.
 * @author Leopard-C (leopard.c@outlook.com)
 * @version 0.1
 * @date 2022-12-25
 * 
 * @copyright Copyright (c) 2022-present, Jinbao Chen
 */
#ifndef IC_LOG_INI_HELPER_H_
#define IC_LOG_INI_HELPER_H_
#include <map>
#include <string>

namespace ic {
namespace log {

class IniHelper {
public:
    IniHelper() = default;

    /**
     * @brief 换行符类型.
     */
    enum LineFeed {
        LF,    /* unix平台 \n       */
        CRLF,  /* windows平台 \r\n  */
        AUTO   /* 根据平台自动选择   */
    };

    /**
     * @brief 读取.ini配置文件.
     */
    bool ParseFile(const std::string& filename);

    /**
     * @brief 写入.ini配置文件.
     */
    bool WriteToFile(const std::string& filename, LineFeed line_feed = LineFeed::AUTO);

    /**
     * @brief 输出为string，用于打印、写入文件等.
     * 
     * @param  line_feed 换行符类型
     */
    std::string ToString(LineFeed line_feed = LineFeed::AUTO) const;

    /**
     * @brief 获取错误信息.
     */
    const std::string& GetLastErrorMsg() const { return err_msg_; }

    std::map<std::string, std::map<std::string, std::string>>& sections() { return sections_; }

private:
    bool ParseLine(std::string& line);

private:
    size_t curr_lineno_ = 0;
    std::string curr_section_name_;

    std::string err_msg_;
    std::map<std::string, std::map<std::string, std::string>> sections_;
};

} // namespace log
} // namespace ic

#endif // IC_LOG_INI_HELPER_
