#ifndef CARD_PD
#define CARD_PD

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "vars.h"
#include "exception.h"
#include "../Poker/headers/poker.h"

namespace pd
{
    class Card
    {
    private:
        cv::Rect contour;
        int value{-1};
        int suit{-1};
    public:
        Card(cv::Rect,std::vector<int>) noexcept(false);
        int getValue() const;
        int getSuit() const;

        operator pkr::Card() const;
        operator std::string() const;
        friend bool operator==(const pd::Card&, const pd::Card&);
    };
}

#endif