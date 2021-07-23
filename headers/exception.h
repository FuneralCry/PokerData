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
        explicit InterimFrame(std::string_view obj_type) throw();
        ~InterimFrame() throw();
        virtual const char* what() const throw();
    };

    class bad_text_recognition : public std::exception
    {
    private:
        std::string err_message;
    public:
        explicit bad_text_recognition(std::string_view obj_type) throw();
        ~bad_text_recognition() throw();
        virtual const char* what() const throw();
    };
}

#endif