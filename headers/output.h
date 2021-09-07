#ifndef OUTPUT_PD
#define OUTPUT_PD

#include <ncurses.h>
#include <chrono>
#include <ctime>
#include <boost/algorithm/string.hpp>
#include "video.h"
#include "timer.h"

namespace pd
{
    class Logger
    {
    public:
        static Timer* timer;
        static WINDOW* win;
        static void createLogEntry(std::string message,std::string tag);
    };

    class ProgressBar
    {
    private:
        static WINDOW* win;
        static Timer* timer;
        static long max_time;
    public:
        ProgressBar(Timer*,WINDOW*,long max_time);
        void update();
    };
}

#endif