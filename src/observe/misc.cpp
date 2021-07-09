#include "../../headers/observe.h"

double pd::Observe::angle(cv::Point a, cv::Point b)
{
    a -= this->center;
    b -= this->center;
    double cos((double)a.dot(b)/(cv::norm(a)*cv::norm(b)));
    double sin((double)a.cross(b)/(cv::norm(a)*cv::norm(b)));
    if(sin >= 0)
        return std::acos(cos);
    else
        return M_PI*2-std::acos(cos);
}


void pd::Observe::enumerate_players(std::vector<cv::Rect>& players) // Enumerates players in order they are sitting at the table
{
    std::vector<std::pair<double,int>> angles; // Angles between vectors ponting at first player and at others
    std::vector<cv::Rect> res;
    cv::Point first(players[0].x,players[0].y);
    double x_s(0),y_s(0);
    for(int i(0); i < players.size(); ++i)
        angles.push_back(std::make_pair(angle(first,players[i].tl()),i));
    std::sort(angles.begin(),angles.end());
    for(int i(0); i < angles.size(); ++i)
        res.push_back(players[angles[i].second]);
    players = std::move(res);
}

bool pd::Observe::isFirstFrame(cv::Mat frame)
{
    std::vector<pd::Obj> bboxes(pd::getBboxes(frame));
    for(pd::Obj bbox : bboxes)
    {
        if(bbox.second == (int)pd::Indices::stake)
            return true;
    }
    return false;
}

void pd::Observe::eraseEliminated()
{
    this->players.erase(
        std::remove_if(this->players.begin(),this->players.end(),[](const pd::Player& p){ return not p.active; }),
        this->players.end());
    for(pd::Player& p : this->players)
        p.active = false;
}

std::vector<pd::Player>::iterator pd::Observe::whoseRect(cv::Rect rect)
{
    if(isIntersects(rect,this->board->getCont()))
        return this->players.end();
    double min_angle(std::numeric_limits<double>::max());
    std::vector<pd::Player>::iterator min_player;
    cv::Point stake_center((rect.br()+rect.tl())*0.5);
    for(auto p_it(this->players.begin()); p_it < this->players.end(); ++p_it)
    {
        cv::Rect player_rect(p_it->getCont());
        cv::Point player_center((player_rect.br()+player_rect.tl())*0.5);
        double angle(std::min(this->angle(player_center,stake_center),this->angle(stake_center,player_center)));
        if(angle < min_angle)
        {
            min_angle = angle;
            min_player = p_it;
        }
    }

    return min_player;
}