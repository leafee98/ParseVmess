#include "include/myBase64/myBase64.h"
#include "include/jsoncpp/json.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

const std::string FLAG_TEXT = R"(vmess://)";

std::string templateFile;
std::string outputFile;

int main() {
    //load settings
    std::fstream settingsFile("./settings.json", std::ios::in | std::ios::binary);
    Json::Value settings;
    settingsFile >> settings;
    settingsFile.close();

    templateFile = settings["templateFile"].asString();
    outputFile = settings["outputFile"].asString();
    std::cout << "settings:\n" << "  templatePath: " << templateFile << std::endl;
    std::cout << "  outputPath: " << outputFile << std::endl << std::endl << "input vmess link:\n";

    // decode vmess link from stdin
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
    
    std::fstream tmpInputFile(templateFile, std::ios::in | std::ios::binary);
    tmpInputFile >> configJson;
    tmpInputFile.close();

    configJson["outbounds"][0]["settings"]["vnext"][0]["address"] = vmessJson["add"];
    configJson["outbounds"][0]["settings"]["vnext"][0]["port"] = std::stoi(vmessJson["port"].asString());
    configJson["outbounds"][0]["settings"]["vnext"][0]["users"][0]["id"] = vmessJson["id"];
    configJson["outbounds"][0]["settings"]["vnext"][0]["users"][0]["alterId"] = std::stoi(vmessJson["aid"].asString());

    Json::Value streamSettings;
    streamSettings["network"] = vmessJson["net"];
    streamSettings["security"] = vmessJson["tls"];
    if (vmessJson["tls"] != Json::Value::null) {
        streamSettings["tlsSettings"]["allowInsecure"] = true;
        streamSettings["tlsSettings"]["host"] = vmessJson["host"];
    }

    std::string switchNet = vmessJson["net"].asString();
    if (switchNet == "tcp") {
        Json::Value tcpSettings;
        tcpSettings["header"]["type"] = vmessJson["type"];
        tcpSettings["header"]["host"] = vmessJson["host"];
        tcpSettings["path"] = vmessJson["path"];
        streamSettings["tcpSettings"] = tcpSettings;
    } else if (switchNet == "kcp") {

    } else if (switchNet == "ws") {
        Json::Value wsSettings;
        wsSettings["header"]["host"] = vmessJson["host"];
        wsSettings["path"] = vmessJson["path"];
        streamSettings["wsSettings"] = wsSettings;
    } else if (switchNet == "h2") {

    } else if (switchNet == "quic") {

    }

    configJson["outbounds"][0]["streamSettings"] = streamSettings;

    std::fstream tmpoutputFile(outputFile, std::ios::out);
    tmpoutputFile << configJson;
    tmpoutputFile.close();
    std::cout << std::endl << "outputPath: " << outputFile << std::endl;

    std::cout << "done!" << std::endl;
    return 0;
}