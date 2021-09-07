// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../headers/card.h"
#include <algorithm>

int pd::Card::getValue() const { return value; }

int pd::Card::getSuit() const { return suit; }

pd::Card::Card(cv::Rect contour, std::vector<int> elements) : contour(contour)
{
    for(int el : elements)
    {
        if(pd::FromYoloToVal.find(el) != pd::FromYoloToVal.end())
            value = FromYoloToVal[el];
        else if(pd::FromYoloToSuit.find(el) != pd::FromYoloToSuit.end())
            suit = FromYoloToSuit[el];
    }
    if(suit == -1 or value == -1)
        throw pd::InterimFrame("pd::Card::Card(...)");
}

pd::Card::operator pkr::Card() const { return pkr::Card(value,suit); }

pd::Card::operator std::string() const { return pkr::Card(*this); }

namespace pd
{
    bool operator==(const pd::Card& a, const pd::Card& b) { return (a.getValue() == b.getValue() and a.getSuit() == b.getSuit()); }
}
