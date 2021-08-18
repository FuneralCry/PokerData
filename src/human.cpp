#include "../headers/human.h"

WINDOW* pd::Human::win = nullptr;

std::string pd::Human::getstring()
{
    std::string input;
    // let the terminal do the line editing
    nocbreak();
    echo();
    // this reads from buffer after <ENTER>, not "raw" 
    // so any backspacing etc. has already been taken care of
    int ch = wgetch(win);
    while ( ch != '\n' )
    {
        input.push_back( ch );
        ch = wgetch(win);
    }
    return input;
}

std::string pd::Human::askUser(cv::Mat&& img)
{
    if(win == nullptr)
        throw std::invalid_argument("WINDOW hasn't been initialized");
    if(stdscr == nullptr)
        initscr();
    std::string win_name("Help request");
    cv::namedWindow(win_name);
    cv::imshow(win_name,img);
    cv::waitKey(1000);
    wprintw(win,"> ");
    std::string input(getstring());
    cv::destroyWindow(win_name);
    
    return input;
}