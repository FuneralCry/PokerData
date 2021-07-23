#include "../headers/output.h"

WINDOW* pd::ProgressBar::win = nullptr;
VideoPlayer* pd::ProgressBar::video = nullptr;

pd::ProgressBar::ProgressBar(VideoPlayer* video)
{
    if(stdscr == nullptr)
        initscr();
    this->win = newwin(1,COLS,LINES-1,0);
    if(this->video == nullptr)
        this->video = video;
}

void pd::ProgressBar::update()
{
    long duration(this->video->getDuration());
    long time(this->video->getTime());
    int percentage(100 * time / duration);
    std::string progress("\r");
    progress += '[' + pd::tohhmmss(time) + '/' + pd::tohhmmss(duration) + ']' + ' ';
    int bar_len(COLS-progress.size()-1);
    progress += std::string(bar_len*percentage/100,'#');
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

void pd::createLogEntry(std::string message, std::string tag)
{
    static auto start_time(std::chrono::system_clock::now());
    long curr_time = std::chrono::duration<double>(std::chrono::system_clock::now()-start_time).count();
    std::string log = '[' + pd::tohhmmss(curr_time) + ']' + '[' + tag + "] " + message + '\n';
    printw(log.c_str());
    refresh();
}
