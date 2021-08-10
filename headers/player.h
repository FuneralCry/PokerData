#ifndef PLAYER_PD
#define PLAYER_PD

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <cassert>
#include <regex>
#include "bboxes.h"
#include "vars.h"
#include "card.h"
#include "text.h"
#include "exception.h"
#include "../Poker/headers/poker.h"

namespace pd
{
    class Player
    {
    private:
        cv::Rect cont;
        std::vector<pd::Card> hand;
        std::string nickname;
    public:
        long long stack{NULL};
        bool active{false};
        bool folded;

        Player(const cv::Mat& frame, const cv::Rect& cont,bool allow_fold,std::string nickname);
        void update(const cv::Mat& frame, const cv::Rect& cont,bool allow_fold);
        cv::Rect getCont() const;
        std::string status() const;
        
        operator pkr::Player() const;
    };
}

#endif