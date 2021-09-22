// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../../headers/observe.h"

pd::Observe::Observe(const std::string& path, const unsigned int fps) : fps(fps)
{
    initscr();
    std::cout.setstate(std::ios::failbit);
    std::cerr.setstate(std::ios::failbit);
    std::clog.setstate(std::ios::failbit);
    this->output.open("lol",std::fstream::in | std::fstream::trunc | std::fstream::out);
    pd::Models::addModel(cv::dnn::readNetFromDarknet(PATH_TO_CFG,PATH_TO_WEIGHTS));
    scrollok(stdscr,true);
    this->start_time = std::chrono::system_clock::now();
    // Cut intro
    video = new VideoPlayer(path);
    this->markupTerminal(0.8);
    pd::Logger::init(this->log_win,video);
    this->progress = new pd::ProgressBar(video,this->progress_win,video->getDuration());
    this->progress->update();
    cv::Mat frame;
    this->video->setFps(1);
    do
    {
        frame = this->video->play();
        this->progress->update();
    }
    while(not isFirstFrame(frame));
    // Clear picture from animations
    frame = video->skip(2);
    // Set user fps
    this->video->setFps(fps);
    std::vector<cv::Rect> players;
    cv::Rect button,board,stake,hud,pot_size;
    // Find all informative objects to initialize game
    std::vector<pd::Obj> bboxes(pd::Models::getBboxes(frame));
    for(pd::Obj bbox : bboxes)
    {
        switch(bbox.second)
        {
            case (int)pd::Indices::hud:
                players.push_back(bbox.first);
                hud = bbox.first;
                break;
            case (int)pd::Indices::button:
                button = bbox.first;
                break;
            case (int)pd::Indices::board:
                board = bbox.first;
                break;
            case (int)pd::Indices::stake:
                stake = bbox.first;
                break;
            case (int)pd::Indices::pot_size:
                pot_size = bbox.first;
                break;
        }
    }
    pd::Logger::createLogEntry("Please, adjust filters for all text blocks. Adjust parameters until text on the pictures becomes readable. Press any key to apply.","MANUAL");
    this->ocr = new pd::OCR(frame(pot_size),frame(stake),frame(hud));
    // Specifying center
    this->center = (board.tl() + board.br()) / 2;
    // Sort players in clockwise order
    enumerate_players(players);
    pd::Logger::createLogEntry("Please, enter players nickname manualy","MANUAL");
    for(cv::Rect player : players)
        this->players.push_back(pd::Player(ocr,frame,player,true,"foo")); // askUser(frame(player))
    // Create board
    this->board = new pd::Board(ocr,frame,board,this->event);
    // Order players from SB to BU
    if(not button.empty())
    {
        auto button_player(whoseRect(button));
        std::rotate(this->players.begin(),++button_player,this->players.end());
    }
    else
    {
        cv::Mat copy_frame(frame);
        for(int i(0); i < this->players.size(); ++i)
        {
            cv::rectangle(copy_frame,this->players[i].getCont(),cv::Scalar(255,35,145),2);
            cv::putText(
                copy_frame,
                std::to_string(i),
                (this->players[i].getCont().tl()+this->players[i].getCont().br())/2,
                cv::FONT_HERSHEY_COMPLEX,
                1,
                cv::Scalar(0,0,255));
        }
        int button_num(std::stoi(pd::askUser(copy_frame,"Can't find button. Please, enter button player number manualy")));
        std::rotate(this->players.begin(),this->players.begin()+button_num+1,this->players.end());
    }
    // Create game
    std::vector<pkr::Player> pkr_players;
    std::vector<pkr::Card> pkr_board(*this->board);
    for(pd::Player p : this->players)
        pkr_players.push_back(p);
    this->game = new pkr::Game(pkr_players,pkr_board);
    this->state = this->game->getState();
    pd::Logger::createLogEntry("Start up has been finished. Current state is: " + pkr::SatesOut[this->state], "INFO");
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
            std::vector<pd::Obj> bboxes(pd::Models::getBboxes(frame));
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
            pd::Logger::createLogEntry(ex.what(),"WARNING");
        }
        catch(std::exception& ex)
        {
            pd::Logger::createLogEntry(ex.what(),"CRITICAL");
            exit(EXIT_FAILURE);
        }
    }
}

pd::Observe::~Observe()
{
    delete game,board;
}