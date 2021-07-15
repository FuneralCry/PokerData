// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../headers/card.h"
namespace pd
{
    int Card::getValue() const { return value; }

    int Card::getSuit() const { return suit; }

    bool operator==(const pd::Card& a, const pd::Card& b) { return (a.getValue() == b.getValue() and a.getSuit() == b.getSuit()); }


    Card::Card(cv::Rect contour, std::vector<int> elements) : contour(contour)
    {
        for(int el : elements)
        {
            if(pd::FromYoloToVal.find(el) != pd::FromYoloToVal.end())
                value = FromYoloToVal[el];
            else if(pd::FromYoloToSuit.find(el) != pd::FromYoloToSuit.end())
                suit = FromYoloToSuit[el];
            else if(el != (int)pd::Indices::card)
            {
                std::cerr << "Invalid elemnt inside card (" << el << ')' << '\n';
                throw 1;
            }
        }
        if(suit == -1 or value == -1)
            throw pd::InterimFrame("Card");
    }

    Card::operator pkr::Card() const { return pkr::Card(value,suit); }
}