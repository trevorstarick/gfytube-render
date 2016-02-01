//
// Created by Trevor Starick on 2016-02-01.
//

#include "video.h"
#include "md5.h"


void download(std::string url) {

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
