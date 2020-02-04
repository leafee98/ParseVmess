//
// There are five functions(fill_tcp_http, fill_kcp, fill_ws, fill_h2, fill_quic)
// refer to https://github.com/boypt/vmess2json. The license of origin 
// repository is MIT.
//

#pragma once

#include <json/json.h>
#include <sstream>

Json::Value load_template(const std::string & type);
Json::Value & fill_config(Json::Value & config, const Json::Value & vmess);
Json::Value & fill_basic(Json::Value & config, const Json::Value & vmess);
Json::Value & fill_streamSetting(Json::Value & config, const Json::Value & vmess);

Json::Value & fill_tcp_http(Json::Value & config, const Json::Value & vmess);
Json::Value & fill_kcp(Json::Value & config, const Json::Value & vmess);
Json::Value & fill_ws(Json::Value & config, const Json::Value & vmess);
Json::Value & fill_h2(Json::Value & config, const Json::Value & vmess);
Json::Value & fill_quic(Json::Value & config, const Json::Value & vmess);

Json::Value & fill_config(Json::Value & config, const Json::Value & vmess) {
    fill_basic(config, vmess);
    fill_streamSetting(config, vmess);
    return config;
}

Json::Value & fill_basic(Json::Value & config, const Json::Value & vmess) {
    Json::Value & vnext = config["outbounds"][0]["settings"]["vnext"];
    vnext[0]["address"] = vmess["add"];
    vnext[0]["port"] = std::stoi(vmess["port"].asString());
    vnext[0]["users"][0]["id"] = vmess["id"];
    vnext[0]["users"][0]["alterId"] = std::stoi(vmess["aid"].asString());

    Json::Value & streamSettings = config["out"];
    streamSettings["network"] = vmess["net"];
    if (vmess["tls"] == "tls") {
        streamSettings["security"] = "tls";
        streamSettings["tlsSettings"]["allowInsecure"] = true;
        streamSettings["tlsSettings"]["serverName"] = vmess["host"];
    }

    return config;
}

Json::Value & fill_streamSetting(Json::Value & config, const Json::Value & vmess) {
    std::string vmessNet = vmess["net"].asString();
    config["outbounds"][0]["streamSettings"]["network"] = vmessNet;
    if (vmessNet == "tcp") {
        if (vmess["type"] == "http")
            return fill_tcp_http(config, vmess);
        return config;
    } else if (vmessNet == "kcp") {
        return fill_kcp(config, vmess);
    } else if (vmessNet == "ws") {
        return fill_ws(config, vmess);
    } else if (vmessNet == "h2") {
        return fill_h2(config, vmess);
    } else if (vmessNet == "quic") {
        return fill_quic(config, vmess);
    } else {
        // alert error.
        return config;
    }
}

Json::Value & fill_tcp_http(Json::Value & config, const Json::Value & vmess) {
    Json::Value tcps(load_template("http"));

    tcps["header"]["type"] = vmess["type"];

    if (vmess["host"] != "") {
        std::string host;
        std::stringstream sstream(vmess["host"].asString());
        for (int i = 0; std::getline(sstream, host, ','); ++i)
            tcps["header"]["request"]["headers"]["Host"][i] = host;
    }

    if (vmess["path"] != "")
        tcps["header"]["request"]["path"][0] = vmess["path"];

    config["outbounds"][0]["streamSettings"]["tcpSettings"] = tcps;
    return config;
}

Json::Value & fill_kcp(Json::Value & config, const Json::Value & vmess) {
    Json::Value kcps(load_template("kcp"));
    
    kcps["header"]["type"] = vmess["type"];

    config["outbounds"][0]["streamSettings"]["kcpSettings"] = kcps;
    return config;
}

Json::Value & fill_ws(Json::Value & config, const Json::Value & vmess) {
    Json::Value wss(load_template("ws"));

    wss["path"] = vmess["path"];
    wss["headers"]["Host"] = vmess["host"];

    config["outbounds"][0]["streamSettings"]["wsSettings"] = wss;
    return config;
}

Json::Value & fill_h2(Json::Value & config, const Json::Value & vmess) {
    Json::Value h2s(load_template("h2"));

    h2s["path"] = vmess["path"];
    h2s["host"][0] = vmess["host"];

    config["outbounds"][0]["streamSettings"]["httpSettings"] = h2s;
    return config;
}

Json::Value & fill_quic(Json::Value & config, const Json::Value & vmess) {
    Json::Value quics(load_template("quic"));

    quics["header"]["type"] = vmess["type"];
    quics["security"] = vmess["host"];
    quics["key"] = vmess["path"];

    config["outbounds"][0]["streamSettings"]["quicSettings"] = quics;
    return config;
}

Json::Value load_template(const std::string & type) {
    static std::string http_template = R"(
        {
            "header": {
                "type": "http",
                "request": {
                    "version": "1.1",
                    "method": "GET",
                    "path": [
                        "/"
                    ],
                    "headers": {
                        "Host": [
                            "www.cloudflare.com",
                            "www.amazon.com"
                        ],
                        "User-Agent": [
                            "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.75 Safari/537.36",
                            "Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36",
                            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36",
                            "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:57.0) Gecko/20100101 Firefox/57.0"
                        ],
                        "Accept": [
                            "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8"
                        ],
                        "Accept-language": [
                            "zh-CN,zh;q=0.8,en-US;q=0.6,en;q=0.4"
                        ],
                        "Accept-Encoding": [
                            "gzip, deflate, br"
                        ],
                        "Cache-Control": [
                            "no-cache"
                        ],
                        "Pragma": "no-cache"
                    }
                }
            }
        }
    )";

    static std::string kcp_template = R"(
        {
            "mtu": 1350,
            "tti": 50,
            "uplinkCapacity": 12,
            "downlinkCapacity": 100,
            "congestion": false,
            "readBufferSize": 2,
            "writeBufferSize": 2,
            "header": {
                "type": "wechat-video"
            }
        }
    )";

    static std::string ws_template = R"(
        {
            "connectionReuse": true,
            "path": "/path",
            "headers": {
                "Host": "host.host.host"
            }
        }
    )";

    static std::string h2_template = R"(
        {
            "host": [
                "host.com"
            ],
            "path": "/host"
        }
    )";

    static std::string quic_template = R"(
        {
            "security": "none",
            "key": "",
            "header": {
                "type": "none"
            }
        }
    )";

    Json::Value result_json;
    if (type == "http") {
        std::stringstream(http_template) >> result_json;
    } else if (type == "kcp") {
        std::stringstream(kcp_template) >> result_json;
    } else if (type == "ws") {
        std::stringstream(ws_template) >> result_json;
    } else if (type == "h2") {
        std::stringstream(h2_template) >> result_json;
    } else if (type == "quic") {
        std::stringstream(quic_template) >> result_json;
    } else {
        std::stringstream("") >> result_json;
    }
    return result_json;
}