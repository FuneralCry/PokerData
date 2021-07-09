#include "../../headers/observe.h"


void pd::Observe::event_NEW_GAME(std::vector<cv::Rect> players)
{
    cv::Mat frame(video->skip(0.5));  // Skip 0.5 seconds to clear picture from animations
    for(cv::Rect rect : players)
    {
        auto p(std::find_if(this->players.begin(),this->players.end(),[rect](const pd::Player& p){ return isIntersects(p.getCont(),rect); }));  // Find player according to rect
        p->update(frame,rect);  // Update it
        std::cout << p->stack << '\n';
    }
    eraseEliminated();  // Delete eliminated players
    std::rotate(this->players.begin(),this->players.begin()+1,this->players.end());  // Shift players to keep order from SB to BU
    std::cout << *game;
    delete game;  // Delete old game
    std::vector<pkr::Player> pkr_players;
    for(pd::Player player : this->players)
        pkr_players.push_back(player);  // Create new players
    game = new pkr::Game(pkr_players,std::vector<pkr::Card>());  // Create new game
}

void pd::Observe::event_NEW_BOARD_CARD()
{
    if(this->board->getCards().size() < 3)
        return;
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
            std::vector<pd::Card> pd_cards;
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
    std::cout << *game;
}