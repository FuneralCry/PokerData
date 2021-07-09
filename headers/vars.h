#ifndef VARS_PD
#define VARS_PD

#include <unordered_map>
#include <vector>
#include <utility>
#include <opencv2/core.hpp>

namespace pd
{
    //Names from voc.names
    enum class Indices
    {
        two,
        three,
        four,
        five,
        six,
        seven,
        eight,
        nine,
        ten,
        jack,
        queen,
        king,
        ace,
        pot_size,
        hud,
        card,
        spade,
        club,
        diamond,
        heart,
        stake,
        button,
        board,
        time_line,
        stack
    };

    //Translation from list above to card values
    extern std::unordered_map<int,int> FromYoloToVal;

    //Translation from list above to card suits
    extern std::unordered_map<int,int> FromYoloToSuit;

    typedef std::pair<cv::Rect,int> Obj;
}

#endif