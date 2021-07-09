// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../../headers/observe.h"

pd::Observe::Observe(const std::string& path, const unsigned int fps) : fps(fps)
{
    // Cut intro
    video = new VideoPlayer(path);
    cv::Mat frame;
    this->video->setFps(1);
    do
    {
        frame = this->video->play();
    }
    while(not isFirstFrame(frame));
    frame = video->skip(2);  // Clear picture from animations
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
    this->center = cv::Point(board.x+board.width,board.y+board.height);
    // Sort players in clockwise order
    enumerate_players(players);
    for(cv::Rect player : players)
        this->players.push_back(pd::Player(frame,player));
    // Order players from SB to BU
    if(not button.empty())
    {
        auto button_player(whoseRect(button));
        std::rotate(this->players.begin(),button_player+1,this->players.end());
    }
    // Create board
    this->board = new pd::Board(frame,board);
    // Create game
    std::vector<pkr::Player> pkr_players;
    std::vector<pkr::Card> pkr_board(*this->board);
    for(pd::Player p : this->players)
        pkr_players.push_back(p);
    this->game = new pkr::Game(pkr_players,pkr_board);
    this->state = this->game->getState();
    std::cout << "STATE: " << state << '\n';
}

void pd::Observe::start()
{
    cv::Mat frame;
    while(this->video->isPlaying())
    {
        frame = this->video->play();
        // Get bounding boxes of objects and their types
        std::vector<pd::Obj> bboxes(pd::getBboxes(frame));
         // Prepare containers forboxes
        std::vector<cv::Rect> players,stakes;
        // Prepare variable for board event
        Events event;
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
                    event = this->board->update(frame,bbox.first);
                    break;
                // Collect stakes bounding boxes for future processing
                case (int)pd::Indices::stake:
                    stakes.push_back(bbox.first);
                    break;
            }
        }
        // Search for folded players
        checkFolded(frame);
        // Process stakes
        processStakes(frame,stakes);

        switch(event)
        {
            case Events::NEW_BOARD_CARD:
                event_NEW_BOARD_CARD();
                break;
            case Events::NEW_GAME:
                event_NEW_GAME(players);
                break;
        }
    }
}

pd::Observe::~Observe()
{
    delete game,board;
}