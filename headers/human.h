#ifndef HUMAN_PD
#define HUMAN_PD

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <ncurses.h>
#include "output.h"

namespace pd
{
    std::string askUser(cv::Mat&&);
}

#endif