//
// Created by Trevor Starick on 2016-02-01.
//

#ifndef GFYTUBE_RENDER_AUDIO_H
#define GFYTUBE_RENDER_AUDIO_H

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

class Audio : public Helpers {
public:
    Audio();
    ~Audio();

    void download(std::string url);
    void convert(std::string filename);
    void extractMetadata(std::string filename);
private:
    std::string extractSignature(std::string url, std::string testString);
    CURL *curl = curl_easy_init();
};

#endif //GFYTUBE_RENDER_AUDIO_H
