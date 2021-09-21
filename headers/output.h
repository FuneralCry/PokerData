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
    private:
        static Timer* timer;
        static WINDOW* win;

        static std::string getstring();
    public:
        static void createLogEntry(std::string message,std::string tag);
        static std::string requestInput(std::string prefix);
        static void init(WINDOW*,Timer*);
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