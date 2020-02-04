#include "include/myBase64/myBase64.h"
#include "fills.hpp"
#include <json/json.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

const std::string VMESS_SCHEMA = R"(vmess://)";

std::string template_file;
std::string output_file;
std::string vmess_link;


void print_usage();

bool parse_parameters(int argc, char * argv[]);
std::string check_files(const std::string & template_file, const std::string & outpt_file);

Json::Value decode_vmess(const std::string & vmess_link);
Json::Value read_template(const std::string & template_file);
void write_output(const std::string & output_file, const Json::Value & output_json);


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
    } else
        std::cout << "/* vmess content: \n" << vmess_json << "*/\n";

    Json::Value template_json = read_template(template_file);
    Json::Value output_json = fill_config(template_json, vmess_json);
    write_output(output_file, output_json);
    
    
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
            << "    -v, --vmess <vmess link>\n"
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
            check_result.append("template file is not readable. template path: ").append(template_file).append("\n");
        }
        file_input.close();
    } else {
        check_result.append("template file not exists. template path: ").append(template_file).append("\n");
    }

    if (output_file == "-")
        return check_result;

    std::filesystem::path output_path(output_file);
    if (std::filesystem::exists(output_path)) {
        std::ofstream file_output(template_file, std::ios::in);
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

void write_output(const std::string & output_file, const Json::Value & output_json) {
    if (output_file == "-") {
        std::cout << output_json << '\n';
    } else {
        std::ofstream file_output(output_file, std::ios::out);
        std::string output_content = output_json.toStyledString();
        file_output.write(output_content.c_str(), output_content.size());
        file_output.close();
    }
}
