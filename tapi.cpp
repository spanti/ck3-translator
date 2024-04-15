#include "tapi.h"
#include "thandler.h"
#include <chrono>
#include <thread>

/// <summary>
/// each translate stage function
/// </summary>
/// <param name="text">the source of map</param>
/// <param name="targetLanguage">goal language</param>
std::string translateText(const std::string &text,
                          const std::string &sourceLanguage,
                          const std::string &targetLanguage,
                          const std::string &apiKey) {
  // google api
  std::string apiUrl = "https://api.openai.com/v1/completions";
  // std::string apiUrl =
  // "https://api.openai.com/v1/engines/davinci/completions";
  // prompt is important in translation task,which directly influence the result
  // of task
  // std::string prompt = "Translate chinese text to " + targetLanguage + ": '"
  // + text + "'";
  std::string prompt = "Please provide the only" + targetLanguage +
                       " translation for the sentence:" + text;
  CURL *curl;
  // CURLM *mcurl;
  CURLcode res;

  curl = curl_easy_init();

  std::string responseData;
  std::string goal_str;

  if (curl) {
    for (int retry = 0; retry < MAX_RETRY; retry++) {
      responseData = "";
      struct curl_slist *headers = NULL;
      headers = curl_slist_append(headers,
                                  ("Authorization: Bearer " + apiKey).c_str());
      headers = curl_slist_append(headers, "Accept-Charset: utf-8");
      // 请求头格式: application/json
      headers = curl_slist_append(
          headers,
          "Content-Type: application/json;charset=utf-8"); // charset=utf-8
      // many curl_easy_setopt - change libcurl's behavior
      // 设置报文传输选项
      curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
      // 设置报文头部
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
      /*
          // 长连接设置
          curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
          // 设置长连接的休眠时间
          curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
          // 设置心跳发送时间，心使得socket长时间保活，小于KEEPIDLE时间
          curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 20L);
          // 设置连接的超时时间，大于心跳时间
              curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);*/

      // json body parameters
      nlohmann::json pj = {
          {"model", "gpt-3.5-turbo-instruct"},
          {"max_tokens", 1024},
          {"prompt", prompt},
      };
      std::string postData = pj.dump();
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
      // 设置内容解压
      // curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
      // You have to implement a callback function that will be called by curl
      // at every batch of data received.
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      // data will be writed to responseData
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
      // For Qt,禁用SSH 证书
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      // 完成传输任务
      res = curl_easy_perform(curl);
      curl_slist_free_all(headers);
      if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        if (res == CURLE_OPERATION_TIMEDOUT) {
          // 如果是超时错误，执行指数退避
          std::this_thread::sleep_for(
              std::chrono::milliseconds(BACKTIME_MS * retry));
          continue; // 重新尝试发送请求
        }
      } else {
        if (responseData.find("error") != std::string::npos) {
          std::this_thread::sleep_for(std::chrono::seconds(BACKTIME_S * retry));
          continue; // 重新尝试发送请求
        } else {
          // 请求成功,跳出
          break;
        }
      }
    }

    // res == CURLE_OK
    curl_off_t val;
    /* check for bytes downloaded */
    // res = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &val);
    // if ((CURLE_OK == res) && (val > 0))
    //   printf("Data downloaded: %lu bytes.\n", (unsigned long)val);

    // 释放内存
    curl_easy_cleanup(curl);
    goal_str = get_str_from_json_2(responseData);
    return goal_str;
  }

  return "";
}
/// <summary>
/// use new gpt3.5-turbo-16k
/// </summary>
/// <param name="text">the source of map</param>
/// <param name="targetLanguage">goal language</param>
std::string gptAPI(const std::string &text,
                      const std::string &targetLanguage,
                   const std::string &apiKey) {
    std::string apiUrl = "https://api.openai.com/v1/chat/completions";
    // std::string apiUrl =
    // "https://api.openai.com/v1/engines/davinci/completions";
    // prompt is important in translation task,which directly influence the result
    // of task
    // std::string prompt = "Translate chinese text to " + targetLanguage + ": '"
    // + text + "'";
    std::string prompt = "Please provide the only " + targetLanguage +
                         " translation for the sentence:";
    CURL *curl;
    // CURLM *mcurl;
    CURLcode res;

    curl = curl_easy_init();

    std::string responseData;
    std::string goal_str;

    if (curl) {
        for (int retry = 0; retry < MAX_RETRY; retry++) {
            responseData = "";
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers,
                                        ("Authorization: Bearer " + apiKey).c_str());
            headers = curl_slist_append(headers, "Accept-Charset: utf-8");
            // 请求头格式: application/json
            headers = curl_slist_append(
                headers,
                "Content-Type: application/json;charset=utf-8"); // charset=utf-8
            // many curl_easy_setopt - change libcurl's behavior
            // 设置报文传输选项
            curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
            // 设置报文头部
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            /*
          // 长连接设置
          curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
          // 设置长连接的休眠时间
          curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
          // 设置心跳发送时间，心使得socket长时间保活，小于KEEPIDLE时间
          curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 20L);
          // 设置连接的超时时间，大于心跳时间
              curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);*/

            // json body parameters
            nlohmann::json pj = {
                                 {"model", "gpt-3.5-turbo-0125"},
                                 {"max_tokens", 1024},
                                 {"temperature", 1},
                                 {"max_tokens", 256},
                                 {"top_p", 1},
                                 {"frequency_penalty", 0},
                                 {"presence_penalty", 0},
                                 {"messages", {{
                                                   {"role", "system"},
                                                   {"content", prompt}},
                                                  {{"role", "user"},
                                                   {"content", text}},
                                                  }},
                                 };
            std::string postData = pj.dump();
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
            // 设置内容解压
            // curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
            // You have to implement a callback function that will be called by curl
            // at every batch of data received.
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            // data will be writed to responseData
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
            // 完成传输任务
            // For Qt,禁用SSH 证书
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            res = curl_easy_perform(curl);
            curl_slist_free_all(headers);
            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
                if (res == CURLE_OPERATION_TIMEDOUT) {
                    // 如果是超时错误，执行指数退避
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(BACKTIME_MS * retry));
                    continue; // 重新尝试发送请求
                }
            } else {
                if (responseData.find("error") != std::string::npos) {
                    std::this_thread::sleep_for(std::chrono::seconds(BACKTIME_S * retry));
                    continue; // 重新尝试发送请求
                } else {
                    // 请求成功,跳出
                    break;
                }
            }
        }

        // res == CURLE_OK
        curl_off_t val;
        /* check for bytes downloaded */
        // res = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &val);
        // if ((CURLE_OK == res) && (val > 0))
        //   printf("Data downloaded: %lu bytes.\n", (unsigned long)val);

        // 释放内存
        curl_easy_cleanup(curl);

        // get str from json
        goal_str = get_str_from_json_2(responseData);
        return goal_str;
    }

    return "";
}
size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                     std::string *output) {
  size_t totalSize = size * nmemb;
  output->append((char *)contents, totalSize);
  return totalSize;
}
