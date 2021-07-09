#ifndef OBSERVER_PD
#define OBSERVER_PD

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <algorithm>
#include <numeric>
#include <limits>
#include <regex>
#include "bboxes.h"
#include "player.h"
#include "board.h"
#include "vars.h"
#include "video.h"
#include "../Poker/headers/poker.h"

namespace pd
{
    class Observe
    {
    private:
        int fps;
        pd::OCR* tess_api;
        pkr::Game* game;  // Poker engine
        std::vector<pd::Player> players; // Players container
        pd::Board* board;  // Board class
        int state;  // Current game state
        cv::Point center;  // Reference point for specifying angels for players. Basicly it is center of bottom edge of board contour

        // Misc

        // Sorts players in clockwise order
        void enumerate_players(std::vector<cv::Rect>& players);
        // Finds angle between 'center-a' and 'center-b' vectors
        double angle(cv::Point a,cv::Point b);
        // Erases all not-active players and resets 'active' label for all remaining
        void eraseEliminated();
        // Finds nearest player to the Rect
        std::vector<pd::Player>::iterator whoseRect(cv::Rect);

        // Player's actions methods

        // Reaction on a player's bet
        void playerBet(std::vector<pd::Player>::iterator player,cv::Mat&& stake);
        // Searchs for folded players
        void checkFolded(const cv::Mat& frame);
        // Extract useful information from all stakes bboxes
        void processStakes(const cv::Mat& frame,std::vector<cv::Rect> stakes);

        // Event handeler members

        // Handles NEW_GAME event
        void event_NEW_GAME(std::vector<cv::Rect> players);
        // Handles NEW_BOARD_CARD event
        void event_NEW_BOARD_CARD();

        // Frames capturing members

        VideoPlayer* video;
        // Checks if frame is non-intro frame
        bool isFirstFrame(cv::Mat);
    public:
        Observe(const std::string& path,const unsigned int fps);
        virtual ~Observe();
        void start();
    };
}

#endif