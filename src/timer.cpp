#include "../headers/timer.h"

std::string tohhmmss(long seconds)
{
    int hours(seconds / 3600);
    seconds -= hours * 3600;
    int minutes(seconds / 60);
    seconds -= minutes * 60;
    std::string res;
    if(hours <= 9)
        res += '0';
    res += std::to_string(hours) + ':';
    if(minutes <= 9)
        res += '0';
    res += std::to_string(minutes) + ':';
    if(seconds <= 9)
        res += '0';
    res += std::to_string(seconds);

    return res;
}