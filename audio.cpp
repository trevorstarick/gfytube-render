//
// Created by Trevor Starick on 2016-02-01.
//

#include "audio.h"

Audio::Audio() {};
Audio::~Audio() {};

std::string Audio::download(std::string id) {
    // todo: check if data/{{id}}.mp4 exists
    // todo: check for 404's

    struct stat buffer;
    auto name = pwd + "audio/" + id + ".m4a";

    if(stat (name.c_str(), &buffer) == 0) {
        return "";
    }

    long sts = 0;

    std::string response, player, signature;
    std::string url = "https://www.youtube.com/watch?v="+id+"&gl=US&hl=en&has_verified=1&bpctr=9999999999";

    auto r = cpr::Get(cpr::Url(url));

    std::smatch m;
    std::regex re("\".*?base\\.js\"");

    while (std::regex_search (r.text, m, re)) {
        player = m[0];
        break;
    }

    url = "https://www.youtube.com/get_video_info";
    url += "?video_id=";
    url += id.c_str();

    url += "&el=info";
    url += "&ps=default";
    url += "&eurl=";
    url += "&gl=US";
    url += "&hl=en";

    r = cpr::Get(cpr::Url(url));

    std::map<std::string, std::string> paramsHash;
    r.text = curl_easy_unescape(curl, r.text.c_str(), 0, 0);

    auto temp = split(r.text, "&");

    for(auto i:temp) {
        auto v = split(i, "=");
        paramsHash[v[0]] = i.substr(v[0].length() + 1);
    }

    if(paramsHash.find("errorcode") != paramsHash.end()) {
        std::string err = "Error " + paramsHash["errorcode"]  +  ": " + paramsHash["reason"];
        return err;
    }


    // todo: download clips of youtube audio or download most popular 10h sonds

    if(std::stoi(paramsHash["length_seconds"]) > 3600) {
        std::string err = "Error length_seconds: " + paramsHash["length_seconds"];
        return err;
    }

    if(paramsHash["ptk"] == "vevo") {
        std::string cmd = "youtube-dl -f 140";
        cmd += " -o " + pwd + "audio/" + id + ".m4a";
        cmd += " https://www.youtube.com/watch?v=";
        cmd += id;

        auto i = exec(cmd.c_str());

        return "";
    } else {
        url = curl_easy_unescape(curl, paramsHash["dashmpd"].c_str(), 0, 0);
    }

    r = cpr::Get(cpr::Url(url));

    if(r.status_code == 403) {
        std::string cmd = "youtube-dl -f 140";
        cmd += " -o " + pwd + "audio/" + id + ".m4a";
        cmd += " https://www.youtube.com/watch?v=";
        cmd += id;

        auto i = exec(cmd.c_str());

        return "";
    } else if(r.status_code != 200) {
        std::cerr << "Error r" << r.status_code  <<  ": " << url << "\n";
        return std::to_string(r.status_code);
    }

    rapidxml::xml_document<> doc;
    doc.parse<0>((char*)r.text.c_str());

    auto *parent = doc.first_node("MPD")
            ->first_node("Period");

    std::vector<rapidxml::xml_node<>*> goodNodes;

    for(auto *node = parent->first_node(); node; node = node->next_sibling()) {
        std::string mimeType = std::string(node->first_attribute("mimeType")->value());
        mimeType = mimeType.substr(0, 9);

        if(mimeType == "audio/mp4") {
            goodNodes.push_back(node);
        }
    }

    int bandwithMax = 0;
    rapidxml::xml_node<> *audioNode;

    for(auto *node:goodNodes) {
        for(auto *subnode = node->first_node("Representation"); subnode; subnode = subnode->next_sibling()) {
            auto bandwidth = std::stoi(subnode->first_attribute("bandwidth")->value());

            if(bandwidth > bandwithMax) {
                bandwithMax = bandwidth;
                audioNode = subnode;
            }
        }
    }

    url = audioNode->first_node("BaseURL")->value();

    r = cpr::Get(cpr::Url(url));

    std::ofstream audio;
    audio.open(pwd + "audio/" + id + ".m4a");
    audio << r.text;
    audio.close();

    return "";
}

std::string Audio::convertToAac(std::string filename) {
    std::string path = pwd + filename;
    std::string name = split(filename, "\\.")[0];

    std::vector<std::string> args = {
            "-y",
            "-v", "error",
            "-i", path,
            "-c:a", "libfdk_aac",
            "-b:a", "320k",
            pwd + "audio/" + name + ".m4a"
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

std::string Audio::convert(std::string filename) {

    std::thread t1(&Audio::convertToAac, this, filename);
//    std::thread t2(&Audio::convertToOpus, this, filename);

    t1.join();
//    t2.join();

    return "";
}

std::string Audio::seek(std::string filename, std::string seek) {
    std::string path = pwd + "audio/" + filename + ".m4a";
    std::string name = filename;

    std::vector<std::string> args = {
            "-y",
            "-v", "error",
            "-i", path,
            "-ss", seek,
            pwd + "tmp/" + name + ".m4a"
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
