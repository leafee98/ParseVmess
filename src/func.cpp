#include "func.h"

#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <filesystem>
#include "include/myBase64/myBase64.h"

const std::string VMESS_SCHEMA = R"(vmess://)";

run_param::run_param() 
    : display_help(false)
    , display_link(false)
    , display_ps(false)
    {}

void print_usage() {
    std::cerr
    << "usage:\n"
    << "        parsevmess [-t <template file>] [-o <output file>] <vmess link>\n"
    << "                   [-h] [-p] [-e]\n"
    << "    -t, --template <template file>\n"
    << "              Specify where the template file is.\n"
    << "              Use /etc/v2ray/config_template.json if not specified.\n"
    << "    -o, --output <output file>\n"
    << "              Specify where to output the filled the config.\n"
    << "              Use /etc/v2ray/config.json if not specified.\n"
    << "              If specified dash(-), will print to stdout.\n"
    << "    -e, --export\n"
    << "              Export the vmess link used by <output file> now.\n"
    << "              Take care that this option is only display the vmess link\n"
    << "              stored as comment in <output file>.\n"
    << "    -p, --ps\n"
    << "              Display the ps of vmess link now using.\n"
    << "              Take care that this option is only display the ps\n"
    << "              stored as comment in <output file>.\n"
    << "              This is DEFAULT option if no parameter specified\n"
    << "    -h, --help\n"
    << "              Display this help\n"
    << "\n"
    << "    <vmess link>\n"
    << "              Specify the vmesslink to be used.\n";
}

void display_ps(const run_param & runp) {
    static std::regex reg(R"(// ps: (.*))");
    std::ifstream fin(runp.output_file, std::ios::in);
    std::string line;
    std::getline(fin, line, '\n');
    std::smatch m;
    if (std::regex_search(line, m, reg)) {
        std::string result = m[1].str();

        if (*result.begin() == '"')
            result.erase(0, 1);
        if (*result.rbegin() == '"')
            result.erase(result.size() - 1, result.size());

        std::cout << result << '\n';
    } else
        std::cerr << "Output file format error\n"
                    "Only support displaying ps stored as comment.\n";
}

void display_link(const run_param & runp) {
    std::ifstream fin(runp.output_file, std::ios::in);
    std::string line;

    // get the second line
    std::getline(fin, line, '\n');
    std::getline(fin, line, '\n');

    line.erase(0, 3);
    std::cout << line << '\n';
}

bool parse_parameters(int argc, char * argv[], run_param & runp) {
    bool normal = true;
    try {
        for (int i = 1; i < argc; ++i) {
            if (runp.template_file.size() == 0
                    && (!strcmp(argv[i], "-t")
                            || !strcmp(argv[i], "--template"))) {
                runp.template_file = argv[++i];
            } else if (runp.output_file.size() == 0
                    && (!strcmp(argv[i], "-o")
                            || !strcmp(argv[i], "--output"))) {
                runp.output_file = argv[++i];
            } else if (runp.display_help == false
                    && (!strcmp(argv[i], "-h")
                            || !strcmp(argv[i], "--help"))) {
                runp.display_help = true;
            } else if (runp.display_link == false
                    && (!strcmp(argv[i], "-e")
                            || !strcmp(argv[i], "--export"))) {
                runp.display_link = true;
            } else if (runp.display_ps == false
                    && (!strcmp(argv[i], "-p")
                            || !strcmp(argv[i], "--ps"))) {
                runp.display_ps = true;
            } else {
                if (std::strncmp(argv[i], argv[i], VMESS_SCHEMA.size()) == 0
                        && runp.vmess_link.size() == 0) {
                    runp.vmess_link = argv[i];
                } else {
                    std::cerr << "invalid or duplicate parameter: "
                            << argv[i] << "\nuse parseVmess --help for help\n";
                    normal = false;
                }
            }
        }

        if (runp.template_file.size() == 0)
            runp.template_file = "/etc/v2ray/config_template.json";
        if (runp.output_file.size() == 0)
            runp.output_file = "/etc/v2ray/config.json";

        // if no action specified, display ps
        if (runp.vmess_link.size() == 0) {
            runp.display_ps = true;
        }
    } catch (const std::out_of_range & oor) {
        normal = false;
        std::cerr << "invalid parameters, use 'parseVmess --help' for help\n";
    }
    return normal;
}

std::string check_files(const run_param & runp) {
    std::string check_result;

    std::filesystem::path template_path(runp.template_file);
    if (std::filesystem::exists(template_path)) {
        std::ifstream file_input(runp.template_file, std::ios::in);
        if (! file_input.good()) {
            check_result.append("template file not readable. template path: ")
                    .append(runp.template_file).append("\n");
        }
        file_input.close();
    } else {
        check_result.append("template file not exists. template path: ")
                .append(runp.template_file).append("\n");
    }

    if (runp.output_file == "-")
        return check_result;

    std::filesystem::path output_path(runp.output_file);
    if (std::filesystem::exists(output_path)) {
        std::ofstream file_output(runp.output_file, std::ios::in);
        if (! file_output.good()) {
            check_result.append("output file is not writable. output path: ")
                    .append(runp.output_file).append("\n");
        }
        file_output.close();
    }

    return check_result;
}

Json::Value decode_vmess(const std::string & vmess_link) {
    if (strncmp(VMESS_SCHEMA.c_str(), vmess_link.c_str(),
                VMESS_SCHEMA.size()) == 0) {
        std::string vmess_content =
                myBase64::decode(vmess_link.substr(VMESS_SCHEMA.size()));
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

    auto is_escape_unicode = [](std::string::const_iterator _begin,
            std::string::const_iterator _end) {
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
        if (*it == '\\'
                && is_escape_unicode(it,
                        (it + 6 < str.end() ? it + 6 : str.end()))
                ) {
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

void write_output(const run_param & runp, const Json::Value & output_json,
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

    if (runp.output_file == "-") {
        std::cerr << "// ps: " << vmess_ps << '\n';
        std::cout << "// " << runp.vmess_link << '\n';
        std::cout << output_str;
    } else {
        std::ofstream file_output(runp.output_file, std::ios::out);
        std::cerr << "// ps: " << vmess_ps << '\n';
        file_output << "// ps: " << vmess_ps << '\n';
        file_output << "// " << runp.vmess_link << '\n';
        file_output << output_str;
        file_output.close();
    }
}