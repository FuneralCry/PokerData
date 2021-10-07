#ifndef RECOGNITION_PD
#define RECOGNITION_PD

#include <utility>
#include <string>
#include "buffer.h"

#define BUFF_SIZE 3

namespace pd
{
    class Recognition
    {
    private:
        Buffer<std::pair<std::string,std::string>>* miss_fix;
        std::string current;
    public:
        Recognition();
        ~Recognition();
        void update(std::string);
        void fix(std::string);
        std::string get() const;
    };
}

#endif