#include "thandler.h"

/// 翻译内容后处理，以符合要求
///
std::string get_str_from_json(std::string &text) {
  nlohmann::json j = nlohmann::json::parse(text);
  // 匹配带引号的字符串
  std::regex stringPattern("^\".*\"");
  std::regex blankpattern("^\\s+|\\s+$"); // 匹配开头或结尾的空格
  // 若回复报文不符合要求,可能出错
  std::string j_json = j.dump();
  // 调试获取其标准报文
  std::string result = j.at("choices")[0].at("text");
  // 定义正则表达式模式匹配字符串开头的 \n
  std::regex res("^\\n+");
  // 使用 regex_replace 将字符串开头的 \n 替换为空字符串
  result = std::regex_replace(result, res, "");
  // std::size_t pos = result.find("\n")
  // 移除前后的空格
  result = std::regex_replace(result, blankpattern, "");
  // 若无双引号
  if (!std::regex_match(result, stringPattern)) {
    result = "\"" + result + "\"";
  }
  return result;
}
std::string get_str_from_json_2(std::string &text) {
    nlohmann::json j = nlohmann::json::parse(text);
    // 匹配带引号的字符串
    std::regex stringPattern("^\".*\"");
    std::regex blankpattern("^\\s+|\\s+$"); // 匹配开头或结尾的空格
    // 若回复报文不符合要求,可能出错
    std::string j_json = j.dump();
    // 调试获取其标准报文
    // ["choices"][0]["message"]["content"]
    std::string result = j.at("choices")[0].at("message").at("content");
    // 定义正则表达式模式匹配字符串开头的 \n
    std::regex res("^\\n+");
    // 使用 regex_replace 将字符串开头的 \n 替换为空字符串
    result = std::regex_replace(result, res, "");
    // std::size_t pos = result.find("\n")
    // 移除前后的空格
    result = std::regex_replace(result, blankpattern, "");
    // 若无双引号
    if (!std::regex_match(result, stringPattern)) {
        result = "\"" + result + "\"";
    }
    return result;
}
void removeNewlines(std::string &str) {
  // 正则表达式移除
  std::regex blankpattern("^\\s+|\\s+$"); // 匹配开头或结尾的空格
  std::regex res("\\n");
  // 使用 regex_replace 将字符串开头的 \n 替换为空字符串
  str = std::regex_replace(str, res, "");
}
