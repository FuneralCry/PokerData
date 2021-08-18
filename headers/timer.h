#ifndef TIMER_PD
#define TIMER_PD

#include <ctime>
#include <string>

std::string tohhmmss(long seconds);

class Timer
{
public:
    Timer() = default;
    virtual long getTime() const = 0;
};


#endif