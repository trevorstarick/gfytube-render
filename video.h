//
// Created by Trevor Starick on 2016-02-01.
//

#ifndef GFYTUBE_RENDER_VIDEO_H
#define GFYTUBE_RENDER_VIDEO_H

#include <string>


class video {
public:
    void extractMetadata(std::string filename);
    void convert(std::string filename);
    void download(std::string url);
};


#endif //GFYTUBE_RENDER_VIDEO_H
