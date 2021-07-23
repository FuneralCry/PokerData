#ifndef OUTPUT_PD
#define OUTPUT_PD

#include <ncurses.h>
#include <chrono>
#include <ctime>
#include <boost/algorithm/string.hpp>
#include "video.h"

namespace pd
{
    std::string tohhmmss(long seconds);

    void createLogEntry(std::string message,std::string tag);

    class ProgressBar
    {
    private:
        static WINDOW* win;
        static VideoPlayer* video;
    public:
        ProgressBar(VideoPlayer*);
        void update();
    };
}

#endif