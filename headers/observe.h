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
#include <stdlib.h>
#include <fstream>
#include "bboxes.h"
#include "player.h"
#include "board.h"
#include "vars.h"
#include "video.h"
#include "exception.h"
#include "output.h"
#include "human.h"
#include "recognition.h"
#include "../Poker/headers/poker.h"

#define NEW_GAME_TIME_LIMIT 3

namespace pd
{
    class Observe
    {
    private:
        int fps;
        double new_game_time{0};
        pd::OCR* ocr;  // OCR engine 
        pkr::Game* game;  // Poker engine
        std::vector<pd::Player> players; // Players container
        pd::Board* board;  // Board class
        pd::Recognition bet_size,pot_size;
        int state;  // Current game state
        bool was_bet{false};
        cv::Point center;  // Reference point for specifying angels for players. Basicly it is center of bottom edge of board contour
        pd::EventGuard event;
        pd::ProgressBar* progress;
        std::chrono::time_point<std::chrono::system_clock> start_time;
        WINDOW* progress_win,*log_win;
        std::fstream output;

        // Misc

        // Sorts players in clockwise order
        void enumerate_players(std::vector<cv::Rect>& players);
        // Finds angle between 'center-a' and 'center-b' vectors
        double angle(cv::Point a,cv::Point b);
        // Erases all not-active players and resets 'active' label for all remaining
        void eraseEliminated();
        // Finds nearest player to the Rect
        std::vector<pd::Player>::iterator whoseRect(cv::Rect);
        void markupTerminal(double ratio);
        bool checkStakeMiss(const cv::Rect&);

        // Player's actions methods

        // Reaction on a player's bet
        void playerBet(int player,cv::Mat&& stake);
        // Searchs for folded players
        void checkFolded(const std::vector<cv::Rect>& cards);
        // Associate player number with stake rect
        std::vector<std::pair<int,cv::Rect>> linkPlayers2Stakes(std::vector<cv::Rect> stakes);
        // Extract useful information from all stakes bboxes
        void processStakes(std::vector<cv::Rect> stakes);

        // Event handeler members

        // Handles NEW_GAME event
        void event_NEW_GAME(std::vector<cv::Rect> players);
        // Handles NEW_BOARD_CARD event
        void event_NEW_BOARD_CARD();
        void checkEvents();

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