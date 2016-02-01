//
// Created by Trevor Starick on 2016-02-01.
//

#include "audio.h"

Audio::Audio() {};
Audio::~Audio() {};

void Audio::download(std::string id) {
    // todo: check if data/{{id}}.mp4 exists
    // todo: check for 404's

    struct stat buffer;
    auto name = pwd + id + ".mp4";

    if(stat (name.c_str(), &buffer) == 0) {
        std::cout << "old " << name << "\n";
        return;
    } else {
        std::cout << "new " << name << "\n";
    }

    long sts = 0;

    std::string response, player, signature;
    std::string url = "https://www.youtube.com/watch?v="+id+"&gl=US&hl=en&has_verified=1&bpctr=9999999999";

    std::cout << url << "\n";

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
        std::cerr << "Error " + paramsHash["errorcode"]  <<  ": " << paramsHash["reason"] << "\n";
        return;
    }


    // todo: download clips of youtube audio or download most popular 10h sonds

    if(std::stoi(paramsHash["length_seconds"]) > 3600) {
        std::cerr << "Error length_seconds: " << paramsHash["length_seconds"] << "\n";
        return;
    }

    if(paramsHash["ptk"] == "vevo") {
        std::cout << "woof" << "\n";
        std::string cmd = "youtube-dl -f 140";
        cmd += " -o " + pwd + id + ".mp4";
        cmd += " https://www.youtube.com/watch?v=";
        cmd += id;

        std::cout << cmd << "\n";

        auto i = exec(cmd.c_str());
        std::cout << i << "\n";
        return;
    } else {
        url = curl_easy_unescape(curl, paramsHash["dashmpd"].c_str(), 0, 0);
    }

    r = cpr::Get(cpr::Url(url));

    std::cout << r.status_code << "\n";

    if(r.status_code == 403) {
        std::cout << "fuck" << "\n";
        std::string cmd = "youtube-dl -f 140";
        cmd += " -o " + pwd + id + ".mp4";
        cmd += " https://www.youtube.com/watch?v=";
        cmd += id;

        std::cout << cmd << "\n";

        auto i = exec(cmd.c_str());
        std::cout << i << "\n";
        return;
    } else if(r.status_code != 200) {
        std::cerr << "Error r" << r.status_code  <<  ": " << url << "\n";
        return;
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
    audio.open(pwd + id + ".mp4");
    audio << r.text;
    audio.close();
}

void Audio::convert(std::string filename) {
    std::string path = pwd + filename;
    std::string name = split(filename, "\\.")[0];

    std::vector<std::string> args = {
            "-y",
            "-i", path,
            "-codec:a", "libmp3lame",
            "-qscale:a", "2",
            pwd + "audio/" + name + ".mp3"
    };

    std::string argsString = "";

    for(auto x:args) {
        argsString += " ";
        argsString += x;
    }

    std::cout << argsString << "\n";

    std::string cmd = "ffmpeg";
    cmd += argsString;

    exec(cmd.c_str());
}

void Audio::extractMetadata(std::string filename) {
    std::string name = split(filename, "\\.")[0];

    std::string cmd = "ffmpeg -i " + filename + " 2>&1 | grep Duration | awk '{print $2}' | tr -d ,";
    cmd += " >> " + pwd + "metadata/" + name + ".meta";

    exec(cmd.c_str());
}
