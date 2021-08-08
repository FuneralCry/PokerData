// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../headers/observe.h"
#include "../headers/bboxes.h"
#include "../headers/text.h"
#include "../headers/vars.h"

#define VIDEO_PATH "../res/example.mp4"
#define FPS 1


int main()
{

    std::string path(VIDEO_PATH);
    pd::Observe obs(path,FPS);
    obs.start();

    return 0;
}
