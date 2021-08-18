#include "../headers/output.h"

WINDOW* pd::ProgressBar::win = nullptr;
VideoPlayer* pd::ProgressBar::video = nullptr;

Timer* pd::Logger::timer = nullptr;

pd::ProgressBar::ProgressBar(VideoPlayer* video)
{
    if(video == nullptr)
        throw std::invalid_argument("VideoPlayer* is nullptr");
    if(stdscr == nullptr)
        initscr();
    this->win = newwin(3,COLS-2,LINES-3,1);
    box(this->win,0,0);
    if(this->video == nullptr)
        this->video = video;
}

void pd::ProgressBar::update()
{
    wmove(this->win,1,1);
    long duration(this->video->getDuration());
    long time(this->video->getTime());
    int percentage(100 * time / duration);
    std::string progress;
    progress += '[' + tohhmmss(time) + '/' + tohhmmss(duration) + ']' + ' ';
    int bar_len(COLS-progress.size()-4);
    progress += std::string(bar_len*percentage/100,'#') + std::string(bar_len-bar_len*percentage/100,' ');
    wprintw(this->win,progress.c_str());
    refresh();
    wrefresh(this->win);
}

void pd::Logger::createLogEntry(std::string message, std::string tag)
{
    if(timer == nullptr)
        throw std::invalid_argument("Timer* hasn't been initialized");
    std::string log = '[' + tohhmmss(timer->getTime()) + ']' + '[' + tag + "] " + message + '\n';
    printw(log.c_str());
    refresh();
}
