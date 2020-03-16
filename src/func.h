# pragma once

#include <string>
#include <json/json.h>

struct run_param {
    std::string template_file;
    std::string output_file;
    std::string vmess_link;
    bool display_help;
    bool display_ps;
    bool display_link;

    run_param();
};

extern const std::string VMESS_SCHEMA;

void print_usage();
void display_ps(const run_param & runp);
void display_link(const run_param & runp);

bool parse_parameters(int argc, char * argv[], run_param & runp);
std::string check_files(const run_param & runp);

Json::Value decode_vmess(const std::string & vmess_link);
Json::Value read_template(const std::string & template_file);

std::string unicode_to_utf8(const std::string & str);
void write_output(const run_param & runp, const Json::Value & output_json,
        const Json::Value vmess_json);
