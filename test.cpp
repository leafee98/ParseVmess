#include <iostream>
#include <string>
#include <algorithm>
#include "include/jsoncpp/json.h"
#include "include/myBase64/myBase64.h"
using namespace std;

const std::string FLAG_TEXT = "vmess://";

int main() {
    // std::string str;
    // std::cin >> str;
    // size_t foundIndex = str.find(FLAG_TEXT);
    // if (foundIndex != std::string::npos)
    //     str.erase(foundIndex, foundIndex + FLAG_TEXT.size());
    // std::cout << myBase64::decode(str) << std::endl;

    std::string vmessLink;
    std::getline(std::cin, vmessLink);
    size_t foundIndex = vmessLink.find(FLAG_TEXT);
    if (foundIndex != std::string::npos)
        vmessLink.erase(foundIndex, foundIndex + FLAG_TEXT.size());
    std::string vmessConfig = myBase64::decode(vmessLink);

    Json::Value configJson;
    Json::Value vmessJson;
    std::stringstream sstream;

    sstream << vmessConfig;
    sstream >> vmessJson;

    std::cout << stoi(vmessJson["port"].asString()) << std::endl;

    // Json::Value root;
    // std::cin >> root;
    // std::cout << root["value"][0] << std::endl;
    // Json::Value tmp;
    // tmp = root["value"];
    // std::cout << tmp << std::endl;

    return 0;
}