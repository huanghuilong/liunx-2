#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

// 替换为你的DeepSeek API密钥
const std::string API_KEY = "sk-319f69258bb84f1bac12f00512cbc52e";
const std::string API_URL = "https://api.deepseek.com/v1/chat/completions";

// 处理curl返回数据的回调函数
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

// 调用DeepSeek API
std::string chat_with_deepseek(const std::string& prompt) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + API_KEY).c_str());

        // 使用nlohmann/json构建请求JSON
        json request = {
            {"model", "deepseek-chat"},
            {"messages", {
                {{"role", "user"}, {"content", prompt}}
            }}
        };

        std::string json_str = request.dump();

        curl_easy_setopt(curl, CURLOPT_URL, API_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}

// 解析API响应
std::string parse_response(const std::string& json_response) {
    std::cout << "原始响应: " << json_response << std::endl;  // 添加这行
    try {
        json response = json::parse(json_response);
        return response["choices"][0]["message"]["content"].get<std::string>();
    } catch (const json::exception& e) {
        return "Error parsing response: " + std::string(e.what());
    }
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    std::cout << "DeepSeek Chat Terminal (输入 'exit' 退出)\n" << std::endl;

    while (true) {
        std::cout << "You: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        std::string response = chat_with_deepseek(input);
        std::string answer = parse_response(response);

        std::cout << "\nDeepSeek: " << answer << "\n" << std::endl;
    }

    curl_global_cleanup();
    return 0;
}