// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../headers/player.h"

pd::Player::Player(const cv::Mat& frame, const cv::Rect& cont)
{
    this->cont = cont;
    this->update(frame,cont);
}

pd::Player::operator pkr::Player() const
{
    if(this->hand.size() != 2)
    {
        std::cerr << "Can't complete casting to pkr::Player because of invalid hand size (" << this->hand.size() << ')' << '\n';
        throw 1;
    }

    return pkr::Player(std::make_pair(this->hand[0],this->hand[1]),this->stack);
}

void pd::Player::update(const cv::Mat& frame, const cv::Rect& cont)
{
    active = true;
    this->cont = cont;
    cv::Mat hud(frame(cont));
    std::vector<pd::Obj> rects(pd::getBboxes(frame(cont)));
    std::vector<std::string> names;
    pd::getNames(names);
    std::vector<std::pair<cv::Rect,std::vector<int>>> hand; // Contour of card and elements inside
    cv::Rect stack_cont;
    for(pd::Obj rect : rects)
    {
        if(rect.second == (int)pd::Indices::card)
            hand.push_back(std::make_pair(rect.first,std::vector<int>()));
        if(rect.second == (int)pd::Indices::stack)
            stack_cont = rect.first;
    }
    stack_cont.width = std::min(hud.cols-stack_cont.x-1,stack_cont.width);
    assert(hand.size() == 2 or hand.size() == 0);
    if(hand.size())
    {
        for(pd::Obj rect : rects)
        {
            if(pd::isIntersects(rect.first,hand[0].first))
                hand[0].second.push_back(rect.second);
            else if(pd::isIntersects(rect.first,hand[1].first))
                hand[1].second.push_back(rect.second);
        }
        this->hand.push_back(pd::Card(hand[0].first,hand[0].second));
        this->hand.push_back(pd::Card(hand[1].first,hand[1].second));
    }
    std::string stack_str(pd::getText(hud(stack_cont),pd::TextType::STACK)),stack_d;
    std::regex dollar("\\$\\d+\\.\\d+[MK]");
    std::smatch res_d;
    if(std::regex_search(stack_str,res_d,dollar))
        stack_d = res_d[0].str().substr(1,res_d[0].str().length()-1);
    long long mult;
    if(*stack_d.rbegin() == 'K')
        mult = 1000;
    else if(*stack_d.rbegin() == 'M')
        mult = 1000000;
    stack_d.erase(stack_d.length()-1);
    this->stack = std::stod(stack_d) * mult;
    std::cout << stack << '\n';
}

cv::Rect pd::Player::getCont() const { return this->cont; }