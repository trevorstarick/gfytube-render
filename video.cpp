//
// Created by Trevor Starick on 2016-02-01.
//

#include "video.h"

Video::Video() {};
Video::~Video() {};

std::string extractSignature(std::string url, std::string testString) {
    url = "https:" + url.substr(1, url.length()-2);

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

std::string Video::download(std::string url) {
    std::string name = split(url, "/").back();
    name = split(name, "\\?")[0];

    struct stat buffer;

    auto path = pwd + name;

    if(stat (path.c_str(), &buffer) == 0) {
        return "";
    }

    auto r = cpr::Head(cpr::Url(url));
    std::smatch m, mm;
    std::regex re;

    if(r.status_code != 200) {
        std::string err = "Path: " + url + " returned status code " + std::to_string(r.status_code);
        return err;
    } else {
        re = std::regex("imgur");
        std::regex_search(r.url, m, re);

        re = std::regex("removed");
        std::regex_search(r.url, mm, re);

        if(m.size() >= 1 && mm.size() >= 1) {
            std::cerr << "Path: " << url << " is a hidden imgur error.\n";
            return "url is imgur hidden";
        }

        url = r.url;
    }

    r = cpr::Get(cpr::Url(url));

    std::ofstream video;
    video.open(pwd + "tmp/" + name);
    video << r.text;
    video.close();

    return "";
}

std::string Video::convertToMp4(std::string filename) {
    std::string path = pwd + "tmp/" + filename;
    std::string ext  = split(filename, "\\.").back();
    std::string name = filename.substr(0, filename.length() - ext.length() - 1);

    std::vector<std::string> args = {
            "-y",
            "-v", "error",
            "-i", path,
            "-c:v", "libx264",
            "-pix_fmt yuv420p",
            "-b:v", "500K",
            "-vf \"scale=trunc(iw/2)*2:trunc(ih/2)*2\"",
            "-threads", "0",
            "-an",
            pwd + "video/" + name + ".mp4"
    };

    std::string argsString = "";

    for(auto x:args) {
        argsString += " ";
        argsString += x;
    }

    std::string cmd = "ffmpeg";
    cmd += argsString;

    exec(cmd.c_str());

    return "";
}

std::string Video::convert(std::string filename) {

//    std::thread t1(&Video::convertToWebm, this, filename);
    std::thread t2(&Video::convertToMp4, this, filename);

//    t1.join();
    t2.join();

    return "";
}

std::string Video::loop(std::string filename, int loopCount) {
    std::string concatStr = "";

    for(int i = 0; i < loopCount; i++) {
        concatStr += "file '" + pwd + "video/" + filename + ".mp4'\n";
    }

    std::ofstream file;
    file.open(pwd + "tmp/" + filename + ".txt");
    file << concatStr;
    file.close();

    std::string cmd = "ffmpeg";
    cmd += " -y";
    cmd += " -v error";
    cmd += " -f concat";
    cmd += " -i ";
    cmd += pwd + "tmp/" + filename + ".txt";
    cmd += " -c copy ";
    cmd += pwd + "tmp/" + filename + ".mp4";

    exec(cmd.c_str());

    return "";
}