#ifndef BOARD_PD
#define BOARD_PD

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <boost/algorithm/string.hpp>
#include <cassert>
#include <regex>
#include "text.h"
#include "card.h"
#include "../Poker/headers/poker.h"
#include "vars.h"
#include "bboxes.h"
#include "events.h"
#include "video.h"

namespace pd
{
    class Board
    {
    private:
        cv::Rect cont;
        std::vector<pd::Card> cards;
        cv::Rect pot_size_cont;
        long long pot_size{NULL};
    public:
        Board() = delete;
        Board(const cv::Mat& frame,const cv::Rect& cont);
        Events update(const cv::Mat& frame,const cv::Rect& cont);
        void updatePot(cv::Mat&&);
        std::vector<pd::Card> getCards() const;
        long long getPotSize() const;
        cv::Rect getCont() const;
        bool isEmpty() const;

        operator std::vector<pkr::Card>() const;
    };
}

#endif