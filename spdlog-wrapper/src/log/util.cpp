#include "util.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#  include <windows.h>
#  include <io.h>
#  include <direct.h>
#else
#  include <dirent.h>
#  include <errno.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

namespace ic {
namespace log {
namespace util {

/**
 * @brief 获取可执行文件(完整)路径.
 */
std::string get_bin_filename() {
    char bin_filename[512] = { 0 };
#ifdef _WIN32
    int n = GetModuleFileNameA(NULL, bin_filename, 511);
    for (int i = 0; i < n; ++i) {
        if (bin_filename[i] == '\\') {
            bin_filename[i] = '/';
        }
    }
#else
    int n = readlink("/proc/self/exe", bin_filename, 511);
    if (n < 0) {
        printf("get exe filename failed. errno: %d. message: %s\n", errno, strerror(errno));
        exit(999);
    }
#endif
    bin_filename[n] = '\0';
    return bin_filename;
}

/**
 * @brief 获取可执行文件目录.
 */
std::string get_bin_dir() {
    std::string bin_filename = get_bin_filename();
    auto pos = bin_filename.rfind('/');
    if (pos == std::string::npos) {
        printf("Won't get here. get_bin_dir error.\n");
        exit(999);
        return {};
    }
    else {
        return bin_filename.substr(0, pos + 1);
    }
}

/**
 * @brief 格式化目录路径.
 * 
 * @details 以'/'结尾.
 */
std::string format_dir(std::string dir) {
    size_t len = dir.length();
#ifdef _WIN32
    for (size_t i = 0; i < len; ++i) {
        if (dir[i] == '\\') {
            dir[i] = '/';
        }
    }
#endif
    if (len > 0 && dir[len - 1] != '/') {
        dir += '/';
    }
    return dir;
}

static bool _create_dir(const std::string& dir) {
    if (access(dir.c_str(), 0) != 0) {
#ifdef _WIN32
        if (_mkdir(dir.c_str()) != 0) {
            return false;
        }
#else
        if (mkdir(dir.c_str(), 0777) != 0) {
            return false;
        }
#endif
    }
    return true;
}

/**
 * @brief 创建目录.
 * 
 * @param dir 绝对路径
 * @param parent 如果父级目录不存在，是否创建
 */
bool create_dir(std::string dir, bool parent/* = false*/) {
    dir = format_dir(dir);
    if (parent) {
        auto pos = dir.find('/', 1);
        while (pos != std::string::npos) {
            if (!_create_dir(dir.substr(0, pos))) {
                return false;
            }
            pos = dir.find('/', pos + 1);
        }
        return true;
    }
    else {
        return _create_dir(dir);
    }
}

/**
 * @brief 文件/目录是否存在.
 */
bool is_path_exist(const std::string& path) {
    return access(path.c_str(), 0) == 0;
}

/**
 * @brief 格式化后缀名称.
 * 
 * @details 为空 或者 以'.'开头(.jpg .txt)
 */
std::string format_ext(std::string ext) {
    trim(ext);
    return (ext.empty() || ext[0] == '.') ? ext : '.' + ext;
}

/**
 * @brief 去除字符串左右的空格，原地去除.
 */
std::string& trim(std::string& str) {
    if (!str.empty()) {
        str.erase(0, str.find_first_not_of(" \r\n\t"));
        str.erase(str.find_last_not_of(" \r\n\t") + 1);
    }
    return str;
}

static const uint64_t SIZE_B  = 1;
static const uint64_t SIZE_KB = 1024 * SIZE_B;
static const uint64_t SIZE_MB = 1024 * SIZE_KB;
static const uint64_t SIZE_GB = 1024 * SIZE_MB;

/**
 * @brief 解析文件大小.
 * 
 * @param[in]  str 文件大小字符串（带单位，如 1.3MB, 2GB）
 * @param[out] size 文件大小，单位字节
 * @retval true 成功
 * @retval false 失败，无效的字符串
 */
bool parse_filesize(std::string str, uint64_t& size) {
    const size_t len = trim(str).length();
    if (len == 0 || len > 16 || str[0] < '0' || str[0] > '9') {
        return false;
    }

    size_t pos;
    size_t dot_count = 0;  /* 小数点的数量 */
    for (pos = 1; pos < len; ++pos) {
        if (str[pos] < '0' || str[pos] > '9') {
            if (str[pos] != '.') {
                break;
            }
            if (++dot_count > 1) {
                return false;
            }
        }
    }

    /* 数字 */
    std::string number_str = str.substr(0, pos);
    double number = std::atof(number_str.c_str());

    /* 单位 */
    std::string unit = str.substr(pos);
    trim(unit);
    if (unit == "" || unit == "B") {
        number *= SIZE_B;
    }
    else if (unit == "K" || unit == "KB") {
        number *= SIZE_KB;
    }
    else if (unit == "M" || unit == "MB") {
        number *= SIZE_MB;
    }
    else if (unit == "G" || unit == "GB") {
        number *= SIZE_GB;
    }
    else {
        return false;
    }

    if (number > ULLONG_MAX) {
        return false;
    }

    size = static_cast<uint64_t>(number);
    return true;
}

/**
 * @brief 格式化文件大小.
 * 
 * @param  size 文件大小(bytes)
 * @param  decimals 小数位数
 * @return std::string 格式化结果
 */
std::string format_filesize(size_t size, unsigned int decimals) {
    double number = double(size);
    char unit[3] = { 0 };
    if (size < SIZE_KB) {
        unit[0] = 'B';
    }
    else {
        if (size < SIZE_MB) {
            number /= SIZE_KB;
            unit[0] = 'K';
        }
        else if (size < SIZE_GB) {
            number /= SIZE_MB;
            unit[0] = 'M';
        }
        else {
            number /= SIZE_GB;
            unit[0] = 'G';
        }
        unit[1] = 'B';
    }

    if (decimals > 6) {
        decimals = 6;  /* 最多6位小数 */
    }

    char result[36] = { 0 };
    int n = snprintf(result, sizeof(result), "%.*lf", decimals, number);
    int offset = 0;
    if (n > static_cast<int>(decimals + 1) && result[n - decimals - 1] == '.') {
        /* 带有小数，去除多余的0 */
        for (int i = n - 1; i > 0; --i) {
            if (result[i] == '0') {
                result[i] = '\0';
            }
            else {
                if (result[i] == '.') {
                    offset = i;
                }
                else {
                    offset = i + 1;
                }
                break;
            }
        }
    }
    strncpy(result + offset, unit, 2);
    return result;
}

} // namespace util
} // namespace log
} // namespace ic
