#include "../../headers/observe.h"

std::string bool2string(bool val) { return val ? "true" : "false"; }

void pd::Observe::event_NEW_GAME(std::vector<cv::Rect> players)
{
    this->new_game_time += (double)1/(double)this->fps;
    pd::Logger::createLogEntry("Forcibly complete new game fase on this iteration: " + bool2string(this->new_game_time >= NEW_GAME_TIME_LIMIT),
        "INFO");
    // Get current frame
    cv::Mat frame(video->get());
    pd::Logger::createLogEntry("Updating players...", "INFO");
    for(cv::Rect rect : players)
    {
        // Find player according to rect
        auto p(std::find_if(this->players.begin(),this->players.end(),[rect](const pd::Player& p){ return isIntersects(p.getCont(),rect); }));
        if(p != this->players.end())
        {
            // Update it
            p->update(frame,rect,this->new_game_time >= NEW_GAME_TIME_LIMIT);
            pd::Logger::createLogEntry(p->status(),"INFO");
        }
        else
            throw std::invalid_argument("Can't find player according to rect");
    }
    // Delete eliminated players
    eraseEliminated();
    // Shift players to keep order from SB to BU
    std::rotate(this->players.begin(),this->players.begin()+1,this->players.end());
    std::vector<pkr::Player> pkr_players;
    // Create new players
    for(pd::Player player : this->players)
        pkr_players.push_back(player);
    // After processing frame and casting players without errors we can switch event to NU
    this->event.reset();
    // Reset state
    this->state = 0;
    this->output << *game << std::endl;
    // Delete old game and create new
    delete this->game;
    this->game = new pkr::Game(pkr_players,std::vector<pkr::Card>());
    this->new_game_time = 0;
}

void pd::Observe::event_NEW_BOARD_CARD()
{
    ++this->state;
    this->was_bet = false;
    switch(state)
    {
        // Turn and river has one new card
        case pkr::turn:
            [[fallthrough]];
        case pkr::river:
            // Trigger game by it
            this->game->changeGameState(std::vector<pkr::Card>({(pkr::Card)*this->board->getCards().rbegin()}));
            break;
        // Flop has three actually)
        case pkr::flop:
        {
            std::vector<pkr::Card> pkr_cards;
            std::vector<pd::Card> pd_cards(this->board->getCards());
            for(pd::Card c : pd_cards)
                pkr_cards.push_back(c);
            // Trigger game by them all
            this->game->changeGameState(pkr_cards);
            break;
        }
        default:
            throw std::invalid_argument("Invalid state");
    }
    pd::Logger::createLogEntry("Current state now is: " + pkr::SatesOut[this->state], "INFO");
    pd::Logger::createLogEntry("Board: " + this->game->getBoard(), "INFO");
    // Event is NU now
    this->event.reset();
}