//
// Created by Trevor Starick on 2016-02-01.
//

#include "helpers.h"
#include "md5.h"
#include <unistd.h>
#include <sys/stat.h>

Helpers::Helpers() {};
Helpers::~Helpers() {};

void Helpers::init() {
    struct stat st = {0};
    std::string path = "";

    path = pwd + "gfytube";
    if (stat(path.c_str(), &st) == -1) mkdir(path.c_str(), 0700);

    pwd = path + "/";

    path = pwd + "video";
    if (stat(path.c_str(), &st) == -1) mkdir(path.c_str(), 0700);

    path = pwd + "audio";
    if (stat(path.c_str(), &st) == -1) mkdir(path.c_str(), 0700);

    path = pwd + "tmp";
    if (stat(path.c_str(), &st) == -1) mkdir(path.c_str(), 0700);

    path = pwd + "media";
    if (stat(path.c_str(), &st) == -1) mkdir(path.c_str(), 0700);
}

std::string Helpers::exec(const char* cmd) {
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}


std::string Helpers::replace(const std::string& input, const std::string& regex, const std::string& replace) {
    std::regex re(regex);

    return std::regex_replace(input, re, replace);
}

std::vector<std::string> Helpers::split(const std::string& input, const std::string& regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator
            first{input.begin(), input.end(), re, -1},
            last;
    return {first, last};
}

float Helpers::getMediaLength(std::string filename) {
    std::string cmd = "ffprobe -i " + filename + " -show_entries format=duration -v quiet -of csv=\"p=0\"";
    auto i = exec(cmd.c_str());
    return std::stof(i);
}


std::string Helpers::mergeAV(std::string start, std::string audio, std::string video) {
    std::string audioPath = pwd + "tmp/" + audio;
    std::string videoPath = pwd + "tmp/" + video;

    std::string ext = split(videoPath, "\\.").back();
    videoPath = videoPath.substr(0, videoPath.length() - ext.length() - 1);
    videoPath += ".mp4";

    ext = split(audioPath, "\\.").back();
    audioPath = audioPath.substr(0, audioPath.length() - ext.length() - 1);
    audioPath += ".m4a";

    std::string filename = md5(audio + video);;

    std::vector<std::string> args = {
            "-y",
            "-v", "error",
            "-i", audioPath,
            "-i", videoPath,
            "-map", "0",
            "-map", "1",
            "-codec", "copy",
            "-shortest",
            pwd + "media/" + filename + ".mp4"
    };

    std::string argsString = "";

    for(auto x:args) {
        argsString += " ";
        argsString += x;
    }

    std::string cmd = "ffmpeg";
    cmd += argsString;

    exec(cmd.c_str());

    std::cout << filename << "\n";

    return "";
}