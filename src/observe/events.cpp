#include "../../headers/observe.h"


void pd::Observe::event_NEW_GAME(std::vector<cv::Rect> players)
{
    // Get current frame
    cv::Mat frame(video->get());
    cv::Mat image = frame;
    for(cv::Rect rect : players)
        cv::rectangle(image,rect,cv::Scalar(94,23,255));
    for(auto p : this->players)
        cv::rectangle(image,p.getCont(),cv::Scalar(255,89,45));
    cv::imwrite("im.jpg",image);
    for(cv::Rect rect : players)
    {
        // Find player according to rect
        auto p(std::find_if(this->players.begin(),this->players.end(),[rect](const pd::Player& p){ return isIntersects(p.getCont(),rect); }));
        if(p != this->players.end())
            // Update it
            p->update(frame,rect);
        else
        {
            std::cerr << "Can't find player according to rect" << '\n';
            throw 1;
        }
    }
    // Delete eliminated players
    eraseEliminated();
    // Shift players to keep order from SB to BU
    std::rotate(this->players.begin(),this->players.begin()+1,this->players.end());
    std::vector<pkr::Player> pkr_players;
    // Create new players
    for(pd::Player player : this->players)
        pkr_players.push_back(player);
    // After processing image and casting players without errors we can switch event to NU
    this->event.reset();
    // Reset state
    this->state = 0;
    std::cout << *game;
    // Delete old game and create new
    delete this->game;
    this->game = new pkr::Game(pkr_players,std::vector<pkr::Card>());
}

void pd::Observe::event_NEW_BOARD_CARD()
{
    ++state;
    std::cout << "STATE: " << state << '\n';
    switch(state)
    {
        // Turn and river has one new card
        case pkr::turn:
            [[fallthrough]];
        case pkr::river:
            // Trigger game by it
            this->game->changeGameState(std::vector<pkr::Card>({(pkr::Card)*this->board->getCards().rbegin()}),this->board->getPotSize());
            break;
        // Flop has three actually)
        case pkr::flop:
        {
            std::vector<pkr::Card> pkr_cards;
            std::vector<pd::Card> pd_cards(this->board->getCards());
            for(pd::Card c : pd_cards)
                pkr_cards.push_back(c);
            // Trigger game by them all
            this->game->changeGameState(pkr_cards,this->board->getPotSize());
            break;
        }
        default:
            std::cerr << "Invalid state" << '\n';
            throw 1;
    }
    // Event is NU now
    this->event.reset();
    std::cout << *game;
}