#include "include/myBase64/myBase64.h"
#include "fills.hpp"
#include <json/json.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <memory>

const std::string VMESS_SCHEMA = R"(vmess://)";

std::string template_file;
std::string output_file;
std::string vmess_link;


void print_usage();

bool parse_parameters(int argc, char * argv[]);
std::string check_files(const std::string & template_file, const std::string & outpt_file);

Json::Value decode_vmess(const std::string & vmess_link);
Json::Value read_template(const std::string & template_file);

std::string unicode_to_utf8(const std::string & str);
void write_output(const std::string & output_file, const Json::Value & output_json,
        const Json::Value vmess_json);


int main(int argc, char * argv[]) {
    if (argc > 1) {
        if (! parse_parameters(argc, argv))
            return EXIT_FAILURE;
    } else {
        print_usage();
        return EXIT_SUCCESS;
    }

    std::string check_result = check_files(template_file, output_file);
    if (check_result.size() > 0) {
        std::cerr << check_result;
        return EXIT_FAILURE;
    }

    Json::Value vmess_json = decode_vmess(vmess_link);
    if (vmess_json == Json::Value::nullSingleton) {
        std::cerr << "unsupported schema or not the vmess 2rd version\n";
        return EXIT_FAILURE;
    }

    Json::Value template_json = read_template(template_file);
    Json::Value output_json = fill_config(template_json, vmess_json);
    write_output(output_file, output_json, vmess_json);
    
    return EXIT_SUCCESS;
}

void print_usage() {
    std::cerr
            << "usage:\n"
            << "        parseVmess [-t <template file>] [-o <output file>] <vmess link>\n"
            << "    \n"
            << "    -t, --template <template file>\n"
            << "              Specify where the template file is.\n"
            << "              Use /etc/v2ray/config_template.json if not specified.\n"
            << "    -o, --output <output file>\n"
            << "              Specify where to output the filled the config.\n"
            << "              Use /etc/v2ray/config.json if not specified.\n"
            << "              If specified dash(-), will print to stdout.\n"
            << "\n"
            << "    <vmess link>\n"
            << "              Specify the vmesslink to be used.\n";
}

bool parse_parameters(int argc, char * argv[]) {
    bool normal = true;
    try {
        template_file = "/etc/v2ray/config_template.json";
        output_file = "/etc/v2ray/config.json";
        for (int i = 1; i < argc; ++i) {
            if (std::string("-t") == argv[i] || std::string("--template") == argv[i]) {
                template_file = argv[++i];
            } else if (std::string("-o") == argv[i] || std::string("--output") == argv[i]) {
                output_file = argv[++i];
            } else {
                vmess_link = argv[i];
                if (std::strncmp(vmess_link.c_str(), VMESS_SCHEMA.c_str(), VMESS_SCHEMA.size()) != 0) {
                    std::cerr << "invalid parameter: " << argv[i] << '\n';
                    normal = false;
                }
            }
        }
    } catch (const std::out_of_range & oor) {
        normal = false;
        std::cerr << "invalid parameters, use parseVmess without any parameters for help\n";
    }
    return normal;
}

std::string check_files(const std::string & template_file, const std::string & output_file) {
    std::string check_result;

    std::filesystem::path template_path(template_file);
    if (std::filesystem::exists(template_path)) {
        std::ifstream file_input(template_file, std::ios::in);
        if (! file_input.good()) {
            check_result.append("template file is not readable. template path: ")
                    .append(template_file).append("\n");
        }
        file_input.close();
    } else {
        check_result.append("template file not exists. template path: ").append(template_file).append("\n");
    }

    if (output_file == "-")
        return check_result;

    std::filesystem::path output_path(output_file);
    if (std::filesystem::exists(output_path)) {
        std::ofstream file_output(output_file, std::ios::in);
        if (! file_output.good()) {
            check_result.append("output file is not writable. output path: ").append(output_file).append("\n");
        }
        file_output.close();
    }

    return check_result;
}

Json::Value decode_vmess(const std::string & vmess_link) {
    if (strncmp(VMESS_SCHEMA.c_str(), vmess_link.c_str(), VMESS_SCHEMA.size()) == 0) {
        std::string vmess_content = myBase64::decode(vmess_link.substr(VMESS_SCHEMA.size()));
        Json::Value vmess_json;
        std::stringstream(vmess_content) >> vmess_json;
        if (vmess_json["v"] == "2") {
            return vmess_json;
        }
    }
    return Json::Value::nullSingleton;
}

Json::Value read_template(const std::string & template_file) {
    std::fstream file_input(template_file, std::ios::in);
    std::string template_content = std::string(
            std::istreambuf_iterator<char>(file_input),
            std::istreambuf_iterator<char>());
    file_input.close();
    Json::Value template_json;
    std::stringstream(template_content) >> template_json;
    return template_json;
}

std::string unicode_to_utf8(const std::string & str) {
    std::string res;
    setlocale(LC_ALL, "");

    auto is_escape_unicode = [](std::string::const_iterator _begin, std::string::const_iterator _end) {
        if (_end - _begin != 6)
            return false;
        if (*_begin != '\\' || *(_begin + 1) != 'u')
            return false;
        
        bool result = true;
        for (auto it = _begin + 2; result && it != _end; ++it) {
            if (isdigit(*it))
                continue;
            char c = tolower(*it);
            if (c < 'a' && c > 'f')
                result = false;
        }
        return result;
    };
    auto hex_to_digit = [](char c) {
        if (isdigit(c)) return c - '0';
        else            return c - 'a' + 10;
    };
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (*it == '\\' && is_escape_unicode(it, (it + 6 < str.end() ? it + 6 : str.end()))) {
            auto t = it + 2;
            int num = 0;
            for (int i = 0; i < 4; ++i, ++t)
                num *= 16, num += hex_to_digit(*t);
            std::string tmp(MB_CUR_MAX, '\0');
            wctomb(nullptr, 0);             // try to shift squences
            int len = wctomb(&tmp[0], (wchar_t)num);

            res.append(tmp, 0, len);

            it += 5; // take care of '++it' in for loop
        } else {
            res += *it;
        }
    }

    return res;
}

void write_output(const std::string & output_file, const Json::Value & output_json,
        const Json::Value vmess_json) {
    Json::StreamWriterBuilder wbuilder;
    wbuilder["commentStyle"] = "None";
    std::unique_ptr<Json::StreamWriter> pwriter(wbuilder.newStreamWriter());

    std::stringstream ss;
    std::string output_str, vmess_ps;

    pwriter->write(output_json, &ss); output_str = ss.str(); ss.str("");
    pwriter->write(vmess_json["ps"], &ss); vmess_ps = ss.str(); ss.str("");

    output_str = unicode_to_utf8(output_str);
    vmess_ps = unicode_to_utf8(vmess_ps);

    if (output_file == "-") {
        std::cerr << "// ps: " << vmess_ps << '\n';
        std::cout << "// " << vmess_link << '\n';
        std::cout << output_str;
    } else {
        std::ofstream file_output(output_file, std::ios::out);
        std::cerr << "// ps: " << vmess_ps << '\n';
        file_output << "// ps: " << vmess_ps << '\n';
        file_output << "// " << vmess_link << '\n';
        file_output << output_str;
        file_output.close();
    }
}
