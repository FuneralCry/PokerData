// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../../headers/observe.h"

pd::Observe::Observe(const std::string& path, const unsigned int fps) : fps(fps)
{
    initscr();
    scrollok(stdscr,true);
    this->start_time = std::chrono::system_clock::now();
    // Cut intro
    video = new VideoPlayer(path);
    this->progress = new pd::ProgressBar(video);
    this->progress->update();
    cv::Mat frame;
    this->video->setFps(1);
    do
    {
        frame = this->video->play();
    }
    while(not isFirstFrame(frame));
    // Clear picture from animations
    frame = video->skip(2);
    cv::imwrite("first.jpg",frame);
    // Set user fps
    this->video->setFps(fps);
    std::vector<cv::Rect> players;
    cv::Rect button,board;
    // Find all informative objects to initialize game
    std::vector<pd::Obj> bboxes(getBboxes(frame));
    for(pd::Obj bbox : bboxes)
    {
        switch(bbox.second)
        {
            case (int)pd::Indices::hud:
                players.push_back(bbox.first);
                break;
            case (int)pd::Indices::button:
                button = bbox.first;
                break;
            case (int)pd::Indices::board:
                board = bbox.first;
                break;
        }
    }
    // Specifying center
    this->center = (board.tl() + board.br()) / 2;
    // Sort players in clockwise order
    enumerate_players(players);
    for(cv::Rect player : players)
        this->players.push_back(pd::Player(frame,player,this->new_game_time >= NEW_GAME_TIME_LIMIT));
    // Order players from SB to BU
    if(not button.empty())
    {
        auto button_player(whoseRect(button));
        std::rotate(this->players.begin(),button_player+1,this->players.end());
    }
    // Create board
    this->board = new pd::Board(frame,board,this->event);
    // Create game
    std::vector<pkr::Player> pkr_players;
    std::vector<pkr::Card> pkr_board(*this->board);
    for(pd::Player p : this->players)
        pkr_players.push_back(p);
    this->game = new pkr::Game(pkr_players,pkr_board);
    this->state = this->game->getState();
    pd::createLogEntry("Start up has been finished. Current state is: " + pkr::SatesOut[this->state], "INFO",this->video->getTime());
}

void pd::Observe::start()
{
    cv::Mat frame;
    while(this->video->isPlaying())
    {
        try
        {
            this->progress->update();
            // Get next frame
            frame = this->video->play();
            // Get bounding boxes of objects and their types
            std::vector<pd::Obj> bboxes(pd::getBboxes(frame));
            // Prepare containers for boxes
            std::vector<cv::Rect> players,stakes;
            // Initialy all players are folded. If we find a card belonging to player we will mark it as non-folded
            for(pd::Player& p : this->players)
                p.folded = true;
            // Parse bounding objects
            for(pd::Obj bbox : bboxes)
            {
                switch(bbox.second)
                {
                    // Only collect players boxes (updating is quite expensive so we do it according to board events)
                    case (int)pd::Indices::hud:
                        players.push_back(bbox.first);
                        break;
                    // Update board class instantly
                    case (int)pd::Indices::board:
                        this->board->update(frame,bbox.first,this->event);
                        break;
                    // Collect stakes bounding boxes for future processing
                    case (int)pd::Indices::stake:
                        stakes.push_back(bbox.first);
                        break;
                    case (int)pd::Indices::card:
                        // Find players with card
                        auto p_it(std::find_if(this->players.begin(),this->players.end(),[bbox](pd::Player p) { return isIntersects(bbox.first,p.getCont()); }));
                        // Consider him as non-folded
                        p_it->folded = false;
                        break;
                }
            }

            switch(this->event.get())
            {
                case pd::Events::NU:
                    // Search for folded players
                    checkFolded();
                    // Process stakes
                    processStakes(stakes);
                    break;
                case pd::Events::NEW_BOARD_CARD:
                    // Search for folded players
                    checkFolded();
                    // Process stakes
                    processStakes(stakes);
                    event_NEW_BOARD_CARD();
                    break;
                case pd::Events::NEW_GAME:
                    // Don't checkFolded and processStakes because board is clearing
                    event_NEW_GAME(players);
                    break;
            }
        }
        catch(pd::InterimFrame& ex)
        {
            pd::createLogEntry(ex.what(),"WARNING",this->video->getTime());
        }
        catch(std::exception& ex)
        {
            pd::createLogEntry(ex.what(),"ERROR",this->video->getTime());
        }
    }
}

pd::Observe::~Observe()
{
    delete game,board;
}