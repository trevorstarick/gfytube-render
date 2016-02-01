//
// Created by Trevor Starick on 2016-02-01.
//

#ifndef GFYTUBE_RENDER_HELPERS_H
#define GFYTUBE_RENDER_HELPERS_H

#include <iostream>
#include <string>
#include <map>
#include <regex>
#include <fstream>

#include <curl/curl.h>

extern std::string pwd;

class Helpers {
public:
    Helpers();
    ~Helpers();
    std::string exec(const char* cmd);
    std::vector<std::string> split(const std::string& input, const std::string& regex);
    std::string replace(const std::string& input, const std::string& regex, const std::string& replace);
    std::string mergeAV(std::string start, std::string audio, std::string video);
    float getMediaLength(std::string filename);
    void init();
private:
    CURL *curl = curl_easy_init();
};

#endif //GFYTUBE_RENDER_HELPERS_H
