//
// Created by Trevor Starick on 2016-02-01.
//

#ifndef GFYTUBE_RENDER_VIDEO_H
#define GFYTUBE_RENDER_VIDEO_H

#include "helpers.h"

#include "duktape.h"
#include "rapidxml.hpp"
#include "md5.h"

#include <cpr/cpr.h>
#include <curl/curl.h>

#include <string>
#include <sys/stat.h>

#include "json.hpp"
using json = nlohmann::json;

extern std::string pwd;

class Video : public Helpers {
    public:
        Video();
        ~Video();
    std::string download(std::string url);
    std::string convert(std::string filename);
    std::string extractMetadata(std::string filename);
    std::string thumbnail(std::string filename);
    std::string loop(std::string filename, int loopCount);
private:
    CURL *curl = curl_easy_init();
    std::string convertToWebm(std::string filename);
    std::string convertToMp4(std::string filename);
};

#endif //GFYTUBE_RENDER_VIDEO_H
