/**
 * @file path.h
 * @brief 程序相关的路径信息.
 * @author Leopard-C (leopard.c@outlook.com)
 * @version 0.1
 * @date 2022-12-25
 * 
 * @copyright Copyright (c) 2022-present, Jinbao Chen.
 */
#ifndef IC_LOG_UTIL_H_
#define IC_LOG_UTIL_H_
#include <string>
#include <vector>

namespace ic {
namespace log {
namespace util {

/**
 * @brief 获取可执行文件(完整)路径.
 */
std::string get_bin_filename();

/**
 * @brief 获取可执行文件目录.
 */
std::string get_bin_dir();

/**
 * @brief 格式化目录路径.
 * 
 * @details 以'/'结尾.
 */
std::string format_dir(std::string dir);

/**
 * @brief 创建目录.
 * 
 * @param dir 绝对路径
 * @param parent 如果父级目录不存在，是否创建
 */
bool create_dir(std::string dir, bool parent = false);

/**
 * @brief 文件/目录是否存在.
 */
bool is_path_exist(const std::string& path);

/**
 * @brief 格式化后缀名称.
 * 
 * @details 为空 或者 以'.'开头(.jpg .txt)
 */
std::string format_ext(std::string ext);

/**
 * @brief 去除字符串左右的空格，原地去除.
 */
std::string& trim(std::string& str);

/**
 * @brief 解析文件大小.
 * 
 * @param[in]  str 文件大小字符串（带单位，如 1.3MB, 2GB）
 * @param[out] size 文件大小，单位字节
 * @retval true 成功
 * @retval false 失败，无效的字符串
 */
bool parse_filesize(std::string str, uint64_t& size);

/**
 * @brief 格式化文件大小.
 * 
 * @param  size 文件大小(bytes)
 * @param  decimals 小数位数
 * @return std::string 格式化结果
 */
std::string format_filesize(size_t size, unsigned int decimals);

/**
 * @brief 判断字符串str是否以字符串prefix开头.
 */
inline bool starts_with(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

/**
 * @brief 判断字符串str是否以字符串suffix结尾.
 */
inline bool ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

} // namespace util
} // namespace log
} // namespace ic

#endif // IC_LOG_UTIL_
