#include "fills.hpp"
#include "func.h"
#include <json/json.h>
#include <iostream>
#include <string>

int main(int argc, char * argv[]) {
    run_param runp;

    if (! parse_parameters(argc, argv, runp))
        return EXIT_FAILURE;

    if (runp.display_help) {
        print_usage();
        return EXIT_SUCCESS;
    }

    if (runp.display_link) {
        display_link(runp);
        return EXIT_SUCCESS;
    }

    if (runp.display_ps) {
        display_ps(runp);
        return EXIT_SUCCESS;
    }

    std::string check_result = check_files(runp);
    if (check_result.size() > 0) {
        std::cerr << check_result;
        return EXIT_FAILURE;
    }

    Json::Value vmess_json = decode_vmess(runp.vmess_link);
    if (vmess_json == Json::Value::nullSingleton) {
        std::cerr << "unsupported schema or not the vmess 2rd version\n";
        return EXIT_FAILURE;
    }

    Json::Value template_json = read_template(runp.template_file);
    Json::Value output_json = fill_config(template_json, vmess_json);
    write_output(runp, output_json, vmess_json);
    
    return EXIT_SUCCESS;
}
