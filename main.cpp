#include <iostream>

#include "helpers.h"

#include "audio.h"
#include "video.h"

std::string pwd;

Helpers helpers;
Audio audio;
Video video;

void displayHelp() {
    std::cout << "gfytube-cli version 1.0.0\n";
    std::cout << "=========================\n";
    std::cout << "Usage: gfytube-cli [OPTIONS]\n";
    std::cout << "\n";
    std::cout << "Required:\n";
    std::cout << "\tVideo: [-v | --video] URL\t\tSets the video source.\n";
    std::cout << "\tAudio: [-a | --audio] URL\t\tSets the audio source.\n";
    std::cout << "\n";
    std::cout << "Optional:\n";
    std::cout << "\tLoops: [-l | --loops] Int\t\tSets the number of loops.\n";
    std::cout << "\tStart: [-s | --start] Int\t\tWhere should the audio start from.\n";
    std::cout << "\tNSFW?: [-n | --nsfw]  Bool\t\tDoes the video or audio contain might be NSFW.\n";
    std::cout << "\n";
    std::cout << "Example:\n";
    std::cout << "gfytube-cli -video \"http://i.imgur.com/pVAMrn8.gifv\" -a \"http://www.youtube.com/watch?v=miymVzEpjR8\" -l 1 -s 36 --nsfw false\n";
    exit(0);
}

void fetch(std::map<std::string, std::string> queryString) {
    std::string af = "";
    std::string vf = "";

    af = audio.download(queryString["v"]);
    vf = video.download(queryString["gif"]);

    if(af != vf) {
        std::cerr << "Something broke: " << af << "\t" << vf << "\n";
        return;
    }

    queryString["gif"] = helpers.split(queryString["gif"], "/").back();
    queryString["gif"] = helpers.split(queryString["gif"], "\\?")[0];

//    af = audio.convert(queryString["v"]);
    vf = video.convert(queryString["gif"]);

    if(af != vf) {
        std::cerr << "Something broke: " << af << "\t" << vf << "\n";
        return;
    }

    queryString["gif"] = helpers.split(queryString["gif"], "\\.")[0];

    af = audio.seek(queryString["v"], queryString["s"]);
    auto audioLength = helpers.getMediaLength(pwd + "tmp/" + queryString["v"] + ".m4a");
    auto videoLength = helpers.getMediaLength(pwd + "video/" + queryString["gif"] + ".mp4");

    int loop;

    if(queryString.find("l") != queryString.end()) {
        loop = std::stoi(queryString["l"]);
    } else {
        loop = (int)ceil(audioLength / videoLength);
    }

    video.loop(queryString["gif"], loop);

    helpers.mergeAV(queryString["s"], queryString["v"], queryString["gif"]);
}

void importFromGifsound(std::string url) {
    std::map<std::string, std::string> queryString {};

    std::string params;
    std::vector<std::string> splitParams;

    std::string key, value;
    std::vector<std::string> keyValue;

    queryString["s"] = "0";

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

    fetch(queryString);
    exit(0);
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

    std::map<std::string, std::string> queryString {};

    std::string videoURL        = "";
    std::string audioURL        = "";

    if(argc == 1) {
        displayHelp();
    }

    pwd = argv[0];
    auto appname = helpers.split(argv[0], "/").back();
    pwd = helpers.replace(pwd, appname, "");

    helpers.init();

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


            if(key == "v") {
                key = "gif";
                value = helpers.replace(value, "gifv", "gif");
            }
            if(key == "a") {
                key = "v";
                value = helpers.split(value, "\\?v=")[1];
                value = helpers.split(value, "&")[0];
            }

            queryString[key] = value;
        } else {
            std::cerr << "Paramater `" << currentArg << "` is missing a key.\n";
            exit(1);
        }
    }

    if(queryString.find("h") != queryString.end()) {
        displayHelp();
    }

    if(queryString.find("gif") == queryString.end()) {
        std::cerr << "Missing video url!" << "\n";
        return 401;
    } else if(queryString.find("v") == queryString.end()) {
        std::cerr << "Missing audio url!" << "\n";
        return 402;
    } else if(queryString.find("l") == queryString.end()) {
        std::cerr << "Missing loop count!" << "\n";
    } else if(queryString.find("s") == queryString.end()) {
        std::cerr << "Missing audio seek!" << "\n";
    } else if(queryString.find("n") == queryString.end()) {
        std::cerr << "Missing nsfw flag!" << "\n";
    } else {
        fetch(queryString);
        return 0;
    }
}