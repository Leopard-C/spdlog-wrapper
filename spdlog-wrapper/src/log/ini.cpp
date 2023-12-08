#include "ini.h"
#include <fstream>

namespace ic {
namespace log {

#define ERROR_THIS_LINE(msg) \
    err_msg_ = "[error] at line " + std::to_string(curr_lineno_) + ": " + std::string(msg);\
    return false

/**
 * @brief 去除字符串左右的空格，原地去除.
 */
static std::string& s_trim(std::string& str, const char* spaces = " \r\n\t") {
    if (!str.empty()) {
        str.erase(0, str.find_first_not_of(spaces));
        str.erase(str.find_last_not_of(spaces) + 1);
    }
    return str;
}

/**
 * @brief 读取.ini配置文件.
 */
bool IniHelper::ParseFile(const std::string& filename) {
    /* 打开文件  */
    std::ifstream ifs(filename);
    if (!ifs) {
        err_msg_ = "open filen failed";
        return false;
    }

    /* 逐行读取 */
    std::string line;
    curr_lineno_ = 0;
    curr_section_name_.clear();
    while (std::getline(ifs, line)) {
        curr_lineno_++;
        if (!ParseLine(line)) {
            return false;
        }
    }

    return true;
}

bool IniHelper::ParseLine(std::string& line) {
    s_trim(line);
    size_t start = 0;
    size_t len = line.length();
    // With BOM
    if (curr_lineno_ == 1 && len >= 3) {
        if ((unsigned char)line[0] == 0xEF &&
            (unsigned char)line[1] == 0xBB &&
            (unsigned char)line[2] == 0xBF)
        {
            start += 3;
        }
    }
    if (start >= len) {
        return true;
    }

    /* 注释 */
    if (line[start] == ';' || line[start] == '#') {
        return true;
    }

    /* 新的一节 */
    if (line[start] == '[') {
        if (line.back() != ']') {
            ERROR_THIS_LINE("section name not end with ']'");
        }
        curr_section_name_ = line.substr(start + 1, len - start - 2);
        if (curr_section_name_.empty()) {
            ERROR_THIS_LINE("section name can not be empty");
        }
        if (sections_.find(curr_section_name_) == sections_.end()) {
            sections_.emplace(curr_section_name_, std::map<std::string, std::string>());
        }
        return true;
    }
    /* 键值对 */
    else {
        auto pos1 = line.find('=', start);
        if (pos1 == std::string::npos) {
            ERROR_THIS_LINE("invalid content");
        }
        std::string name = line.substr(start, pos1 - start);
        s_trim(name, " \"\r\n\t");
        if (name.empty()) {
            ERROR_THIS_LINE("key is empty");
        }
        auto pos2 = line.find_first_of("#;", pos1 + 1);
        std::string value = line.substr(pos1 + 1, pos2 - pos1 - 1);
        s_trim(value, " \"\r\n\t");
        if (curr_section_name_.empty()) {
            ERROR_THIS_LINE("not belong to any section");
        }
        sections_[curr_section_name_].emplace(name, value);
        return true;
    }
}

/**
 * @brief 写入.ini配置文件.
 */
bool IniHelper::WriteToFile(const std::string& filename, LineFeed line_feed/* = LineFeed::AUTO*/) {
    std::ofstream ofs(filename);
    if (!ofs) {
        err_msg_ = "open file failed";
        return false;
    }
    ofs << ToString(line_feed);
    return true;
}

static std::string s_get_line_feed_str(IniHelper::LineFeed line_feed/* = LineFeed::AUTO*/) {
    if (line_feed == IniHelper::LineFeed::LF) {
        return "\n";
    }
    else if (line_feed == IniHelper::LineFeed::CRLF) {
        return "\r\n";
    }
    else {
#ifdef _WIN32
        return "\r\n";
#else
        return "\n";
#endif
    }
}

/**
 * @brief 输出为string，用于打印、写入文件等.
 * 
 * @param  line_feed 换行符类型
 */
std::string IniHelper::ToString(LineFeed line_feed/* = LineFeed::AUTO*/) const {
    const std::string line_feed_str = s_get_line_feed_str(line_feed);
    std::string result;
    result.reserve(128);
    for (auto& p1 : sections_) {
        result += '[' + p1.first + ']' + line_feed_str;
        for (auto& p2 : p1.second) {
            result += p2.first + " = " + p2.second + line_feed_str;
        }
        result += line_feed_str;  /* 两个section之间插入空行 */
    }
    return result;
}

} // namespace log
} // namespace ic
