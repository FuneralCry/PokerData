#include "../headers/recognition.h"

pd::Recognition::Recognition()
{
    this->miss_fix = new Buffer<std::pair<std::string,std::string>>(BUFF_SIZE);
}

pd::Recognition::~Recognition()
{
    delete this->miss_fix;
}

void pd::Recognition::update(std::string str)
{
    auto fix_it(std::find_if(this->miss_fix->begin(),this->miss_fix->end(),[str](std::pair<std::string,std::string> x) { return x.first == str; }));
    if(fix_it == this->miss_fix->end())
        this->current = str;
    else
        this->current = fix_it->second;
}

void pd::Recognition::fix(std::string str)
{
    this->miss_fix->insert(std::make_pair(this->current,str));
    this->current = str;
}

std::string pd::Recognition::get() const { return this->current; }