#include "../headers/human.h"

std::string getstring()
{
    std::string input;
    // let the terminal do the line editing
    nocbreak();
    echo();
    // this reads from buffer after <ENTER>, not "raw" 
    // so any backspacing etc. has already been taken care of
    int ch = getch();
    while ( ch != '\n' )
    {
        input.push_back( ch );
        ch = getch();
    }
    return input;
}

std::string pd::askUser(cv::Mat&& img)
{
    if(stdscr == nullptr)
        initscr();
    std::string win_name("Help request");
    cv::namedWindow(win_name);
    cv::imshow(win_name,img);
    cv::waitKey(1000);
    printw("> ");
    std::string input(getstring());
    cv::destroyWindow(win_name);
    
    return input;
}