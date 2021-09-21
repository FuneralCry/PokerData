#ifndef EXCEPT_PD
#define EXCEPT_PD

#include <exception>
#include <iostream>

namespace pd
{
    class InterimFrame : public std::exception
    {
    private:
        std::string err_message;
    public:
        InterimFrame(std::string_view obj_type) throw();
        ~InterimFrame() throw();
        const char* what() const throw();
    };

    class bad_recognition : public std::exception
    {
    private:
        std::string err_message;
    public:
        bad_recognition(std::string_view obj_type) throw();
        ~bad_recognition() throw();
        const char* what() const throw();
    };
}

#endif