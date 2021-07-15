#include "../headers/exception.h"

pd::InterimFrame::InterimFrame(std::string_view obj_type) throw() : err_message{obj_type}
{
    this->err_message = "Can't find required bounding boxes in '" + this->err_message + "'";
}

pd::InterimFrame::~InterimFrame() throw() {}

const char* pd::InterimFrame::what() const throw() { return this->err_message.c_str(); }