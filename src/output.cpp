#include "../headers/output.h"

WINDOW* pd::ProgressBar::win = nullptr;
VideoPlayer* pd::ProgressBar::video = nullptr;

pd::ProgressBar::ProgressBar(VideoPlayer* video)
{
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
    progress += '[' + pd::tohhmmss(time) + '/' + pd::tohhmmss(duration) + ']' + ' ';
    int bar_len(COLS-progress.size()-4);
    progress += std::string(bar_len*percentage/100,'#') + std::string(bar_len-bar_len*percentage/100,' ');
    wprintw(this->win,progress.c_str());
    refresh();
    wrefresh(this->win);
}

std::string pd::tohhmmss(long seconds)
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

void pd::createLogEntry(std::string message, std::string tag, long time)
{
    std::string log = '[' + pd::tohhmmss(time) + ']' + '[' + tag + "] " + message + '\n';
    printw(log.c_str());
    refresh();
}
