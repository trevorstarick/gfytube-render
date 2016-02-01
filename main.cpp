#include <iostream>

#include "helpers.h"

#include "audio.h"
#include "video.h"

std::string pwd = "/Users/trevorstarick/Projects/gfytube-render/data/";

Helpers helpers;
Audio audio;
Video video;

void importFromGifsound(std::string url) {
    std::map<std::string, std::string> queryString {};

    std::string params;
    std::vector<std::string> splitParams;

    std::string key, value;
    std::vector<std::string> keyValue;

    params = helpers.split(url, "gifsound.com/")[1];
    params.replace(params.find("?"), 1, "");

    splitParams = helpers.split(params, "&");

    for(auto param : splitParams) {
        keyValue = helpers.split(param, "=");
        key = keyValue[0];
        value = param.replace(0, key.length()+1, "");

        queryString[key] = value;
    }

    if(queryString.find("gif") != queryString.end()) {
        // simple gif lookup
    } else if(queryString.find("gifv") != queryString.end()) {
        // imgur lookup
        queryString["gif"] = "i.imgur.com/" + queryString["gifv"] + ".webm";
    } else if(queryString.find("gfycat") != queryString.end()) {
        // gfycat lookup
        queryString["gif"] = "http://gfycat.com/cajax/get/" + queryString["gfycat"];
    } else {
        // throw error
        exit(2);
    }

    if(queryString.find("sound") != queryString.end()) {
        // old scool audio now &v
        queryString["v"] = helpers.split(queryString["sound"], "=")[1];
    }

    if(queryString.find("start") != queryString.end()) {
        // old scool seek now &s
        queryString["s"] = queryString["start"];
    }


    if(queryString["gif"].substr(0,4) != "http") {
        queryString["gif"] = "http://" + queryString["gif"];
    }

    std::string start = queryString["s"];

    // todo: set getAudio and getVideo to parallel

    audio.download(queryString["gif"]);
    video.download(queryString["v"]);

//    mergeAV(queryString["v"], queryString["gif"]);
}

void test(std::string path) {
//    std::cout << path << "\n";

    std::ifstream ifs(path);
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    auto urls = helpers.split(content, "\n");

    for(auto url:urls) {
        importFromGifsound(url);
    }
}


int main(int argc, char* argv[]) {
    bool isNSFW                 = false;
    unsigned int seekPosition   = 0;
    unsigned int loops          = 0;

    std::string videoURL        = "";
    std::string audioURL        = "";

//    path = argv[0];

//    std::string filename = "dRpzxKsSEZg.mp4";
//
//    audio.convert(filename);
//    audio.extractMetadata(filename);
//
//    return 0;

    for(int i = 1; i < argc; i += 2) {
        std::string currentArg(argv[i]);
        std::string key, value;

        if(currentArg[0] == '-') {
            if(currentArg[1] == '-') {
                if(currentArg == "--test") {
                    test(argv[i + 1]);
                    exit(0);
                } else {
                    key = currentArg[2];
                }
            } else {
                key = currentArg[1];
            }

            value = argv[i+1];

            if(key == "g") {
                importFromGifsound(value);
                return 0;
            }

            std::cout << key << " " << value << "\n";

        } else {
            std::cerr << "Paramater `" << currentArg << "` is missing a key.\n";
            exit(1);
        }
    }

    return 0;
}