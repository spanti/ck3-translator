#pragma once

#include <curl/curl.h>
#include <json.hpp>
#include <pthread.h>
#include <thread>
#include <regex>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <mutex>

#define MAX_RETRY 5

#define BACKTIME_MS 100
#define BACKTIME_S 2

std::string translateText(const std::string &text,
                          const std::string &sourceLanguage,
                          const std::string &targetLanguage,
                          const std::string &apiKey);
std::string gptAPI(const std::string &text,
                   const std::string &targetLanguage,
                   const std::string &apiKey);
                          // 翻译文本
size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                     std::string *output);

