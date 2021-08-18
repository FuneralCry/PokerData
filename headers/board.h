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
#include "exception.h"
#include "vars.h"
#include "bboxes.h"
#include "events.h"
#include "video.h"
#include "output.h"
#include "../Poker/headers/poker.h"

namespace pd
{
    class Board
    {
    private:
        pd::OCR* ocr;
        cv::Rect cont,pot_size_cont;
        std::vector<pd::Card> cards;
        long long pot_size{NULL};
    public:
        Board() = delete;
        Board(pd::OCR* ocr,const cv::Mat& frame,const cv::Rect& cont,pd::EventGuard&);
        void update(const cv::Mat& frame,const cv::Rect& cont,pd::EventGuard&);
        void updatePot(cv::Mat&&);
        std::vector<pd::Card> getCards() const;
        long long getPotSize() const;
        cv::Rect getCont() const;
        bool isEmpty() const;

        operator std::vector<pkr::Card>() const;
    };
}

#endif