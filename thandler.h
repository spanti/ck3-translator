#pragma once

#include <regex>
#include <string>
#include <json.hpp>
//文本处理api
std::string get_str_from_json(std::string &text);
std::string get_str_from_json_2(std::string &text);
void removeNewlines(std::string &str);
