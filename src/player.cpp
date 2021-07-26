// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../headers/player.h"
#include <boost/algorithm/string.hpp>

pd::Player::Player(const cv::Mat& frame, const cv::Rect& cont, bool allow_fold)
{
    this->cont = cont;
    this->update(frame,cont,allow_fold);
}

pd::Player::operator pkr::Player() const
{
    return pkr::Player(std::make_pair(this->hand[0],this->hand[1]),this->stack);
}

void pd::Player::update(const cv::Mat& frame, const cv::Rect& cont, bool allow_fold)
{
    active = true;

    this->hand.clear();
    this->cont = cont;
    cv::Mat hud(frame(cont));
    std::vector<pd::Obj> rects(pd::getBboxes(frame(cont)));
    std::vector<std::pair<cv::Rect,std::vector<int>>> hand; // Contour of card and elements inside
    cv::Rect stack_cont;
    for(pd::Obj rect : rects)
    {
        if(rect.second == (int)pd::Indices::card)
            hand.push_back(std::make_pair(rect.first,std::vector<int>()));
        if(rect.second == (int)pd::Indices::stack)
            stack_cont = rect.first;
    }
    if(hand.size() != 2 and not allow_fold)
        throw pd::InterimFrame("void pd::Player::update(...)");
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
    else
        this->folded = true;

    std::string stack_str(pd::getText(hud(stack_cont),pd::TextType::STACK)),stack_d;
    boost::erase_all(stack_str," ");
    std::regex dollar("\\$?\\d+[.,]\\d+[MK]");
    std::smatch res_d;
    if(std::regex_search(stack_str,res_d,dollar))
        stack_d = res_d[0].str().substr(1,res_d[0].str().length()-1);
    else
        throw pd::bad_text_recognition("void pd::Player::update(...)");
    long long mult;
    if(*stack_d.rbegin() == 'K')
        mult = 1000;
    else if(*stack_d.rbegin() == 'M')
        mult = 1000000;
    stack_d.erase(stack_d.length()-1);
    this->stack = std::stod(stack_d) * mult;
}

cv::Rect pd::Player::getCont() const { return this->cont; }

std::string pd::Player::status() const
{
    std::string res;
    res += nickname.empty() ? "Foo " : nickname + ' ';
    res += "$ " + this->stack == NULL ? "null " : std::to_string(this->stack) + ' ';
    res += this->hand.size() == 2 ? (std::string)this->hand[0] + '/' + (std::string)this->hand[1] : "folded";

    return res;
}