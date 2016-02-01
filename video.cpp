//
// Created by Trevor Starick on 2016-02-01.
//

#include "video.h"

Video::Video() {};
Video::~Video() {};

std::string extractSignature(std::string url, std::string testString) {
    url = "https:" + url.substr(1, url.length()-2);
    std::cout << url << "\n";

    auto r = cpr::Get(cpr::Url(url));

    std::string input = r.text;

    std::smatch m;
    std::string type, match, function, functionName;

    if(url.substr(url.length()-7) == "base.js") {
        type = "base";
    } else {
        type = "html5";
    }

    auto re = std::regex("\\.sig\\|\\|([a-zA-Z0-9$]+)\\(");

    while (std::regex_search(input, m, re)) {
        match = m[0];
        break;
    }

    functionName = match.substr(6);
    functionName = functionName.substr(0,functionName.length()-1);

    if(type == "base") {
        re = std::regex(functionName + "=function\\(a\\)\\{(.|\\s)*?\\}");

    } else {
        re = std::regex("function " + functionName + "(.*?)\\}");
    }

    while (std::regex_search (input, m, re)) {
        match = m[0];
        break;
    }

    function = match;

    if(type == "base") {
        re = std::regex("\\;([a-zA-Z0-9$]+)\\.");
    } else {
        re = std::regex("\\=([a-zA-Z0-9$]+)\\(");
    }

    while (std::regex_search (function, m, re)) {
        match = m[0];
        break;
    }

    auto subfunction = match.substr(1,2);

    if(type == "base") {
        re = std::regex("var " + subfunction + "(.|\\s)*?\\}\\}");
    } else {
        re = std::regex("function " + subfunction + "(.*?)\\}");
    }

    while (std::regex_search (input, m, re)) {
        match = m[0];
        break;
    }

    subfunction = match;

    std::string script = "";

    script += subfunction + ";";
    script = std::regex_replace (script, std::regex("\\n|\\t"), "");

    script += function + ";";
    script = std::regex_replace (script, std::regex("\\n|\\t"), "");

    script += functionName+"(\"" + testString +"\");";
    script = std::regex_replace (script, std::regex("\\n|\\t"), "");

    duk_context *ctx = duk_create_heap_default();
    duk_eval_string(ctx, (script).c_str());

    auto result = duk_safe_to_string(ctx, -1);

    duk_destroy_heap(ctx);

    return result;

}

void Video::download(std::string url) {
    std::string name = split(url, "/").back();
    auto tmp = split(name, "\\.").back();
    tmp = split(tmp, "?")[0];
    name = name.substr(0, name.length()-tmp.length());
    name += md5(url);
    name += "." + tmp;

    struct stat buffer;

    auto path = pwd + name;

    if(stat (path.c_str(), &buffer) == 0) {
        std::cout << "old " << url << "\n";
        return;
    } else {
        std::cout << "new " << url << "\n";
    }

    auto r = cpr::Head(cpr::Url(url));
    std::smatch m, mm;
    std::regex re;

    if(r.status_code != 200) {
        std::cerr << "Path: " << url << " returned status code " << r.status_code << "\n";
        return;
    } else {
        std::cout << r.status_code << "\t" << url << "\t" << r.url << "\n";

        re = std::regex("imgur");
        std::regex_search(r.url, m, re);

        re = std::regex("removed");
        std::regex_search(r.url, mm, re);

        std::cout << m.size() << " " << mm.size() << "\n";

        if(m.size() >= 1 && mm.size() >= 1) {
            std::cerr << "Path: " << url << " is a hidden imgur error.\n";
            return;
        }

        url = r.url;
    }

    r = cpr::Get(cpr::Url(url));

    std::ofstream video;
    video.open(pwd + name);
    video << r.text;
    video.close();
}

void Video::convert(std::string filename) {}
void Video::extractMetadata(std::string filename) {}
