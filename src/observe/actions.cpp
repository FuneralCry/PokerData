#include "../../headers/observe.h"

void pd::Observe::playerBet(int player_num, cv::Mat&& stake)
{
    cv::Mat stake_backup(stake);
    // Read bet size string from frame
    std::string bet_size_str(this->ocr->getText(std::move(stake),pd::OCR::TextType::BET));
    //  Clear it and extract digits
    boost::erase_all(bet_size_str,",");
    boost::erase_all(bet_size_str,".");
    std::regex r("\\d+");
    std::smatch m;
    if(std::regex_search(bet_size_str,m,r))
        this->bet_size.update(m[0].str());
    else
    {
        pd::Logger::createLogEntry("Can't recognize bet size.","ERROR");
        this->bet_size.fix(askUser(stake_backup,"Please, enter stake value manually"));
    }
    // Trigger game
    while(true)
    {
        try
        {
            if(this->players[player_num].last_stake == std::stoll(bet_size.get()))
                break;
            this->game->playerAction(player_num,pkr::Actions::bet,std::stoll(this->bet_size.get()));
            this->players[player_num].last_stake = std::stoll(this->bet_size.get());
            pd::Logger::createLogEntry(this->bet_size.get(),"BET");
            break;
        }
        catch(pkr::bad_stake ex)
        {
            pd::Logger::createLogEntry(ex.what(),"ERROR");
            this->bet_size.fix(askUser(stake_backup,"Please, enter stake value manually"));
        }
    }
}

void pd::Observe::checkFolded(const std::vector<cv::Rect>& cards)
{
    for(int i(this->game->getOrder()[0]); i < this->players.size(); i = (i+1) % (this->players.size() + 1))
    {
        int cards_num;
        if(this->game->getOrder().size() == 1)
            break;
        for(const cv::Rect& c : cards)
        {
            if(isIntersects(c,this->players[i].getCont()))
                ++cards_num;
        }
        if(cards_num == 0)
        {
            if(this->game->actAllowed(i))
            {
                this->game->playerAction(i,pkr::Actions::fold);
                pd::Logger::createLogEntry("Fold","FOLD");
                this->players[i].folded = true;
            }
        }
        else if(cards_num == 1)
            throw pd::InterimFrame("void pd::Observe::checkFolded(...)");
    }
}

std::vector<std::pair<int,cv::Rect>> pd::Observe::linkPlayers2Stakes(std::vector<cv::Rect> stakes)
{
    std::vector<std::pair<int,cv::Rect>> res;
    for(cv::Rect& s : stakes)
    {
        auto p(whoseRect(s));
        if(p == this->players.end())
            res.push_back(std::make_pair(-1,s));
        else
            res.push_back(std::make_pair(std::distance(this->players.begin(),p),s));
    }

    return res;
}

void pd::Observe::processStakes(std::vector<cv::Rect> stakes)
{
    // Get current frame
    cv::Mat frame(this->video->get());
    // If 'stake' in the middle (pot actually) has gone, then game is over and there is nothing to do
    if(std::none_of(stakes.begin(),stakes.end(),[&](cv::Rect r) { return isIntersects(this->board->getCont(),r); }))
        return;
    // Find owner of each stake
    std::vector<std::pair<int,cv::Rect>> pn_stakes(linkPlayers2Stakes(stakes));
    std::sort(pn_stakes.begin(),pn_stakes.end(),[](auto& a, auto& b) { return a.first < b.first; });
    for(auto& p : pn_stakes)
    {
        // If it is not in the middle
        if(p.first >= 0)
        {
            if(not this->was_bet)
            {
                for(int i(p.first-1); i >= 0; --i)
                    if(this->game->actAllowed(i))
                        this->game->playerAction(i,pkr::Actions::check);
            }
            if(this->game->actAllowed(p.first))
            {
                // Trigger game
                playerBet(p.first,frame(p.second));
                this->was_bet = true;
            }
        }
    }
}