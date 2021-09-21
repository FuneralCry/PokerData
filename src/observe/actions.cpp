#include "../../headers/observe.h"

void pd::Observe::playerBet(int player_num, cv::Mat&& stake)
{
    cv::imwrite("bett.jpg",stake);
    cv::Mat stake_backup(stake);
    // Read bet size string from frame
    std::string bet_size_str(this->ocr->getText(std::move(stake),pd::OCR::TextType::BET));
    //  Clear it and extract digits
    boost::erase_all(bet_size_str,",");
    boost::erase_all(bet_size_str,".");
    std::regex r("\\d+");
    std::smatch m;
    long long bet_size;
    if(std::regex_search(bet_size_str,m,r))
    {
        bet_size = std::stoll(m[0].str());
    }
    else
        throw pd::bad_recognition("pd::Observe::playerBet()");
    // Trigger game
    if(bet_size != this->players[player_num].last_stake)
    {
        ask_user:
        try
        {
            this->game->playerAction(player_num,pkr::Actions::bet,bet_size);
            this->players[player_num].last_stake = bet_size;
        }
        catch(pkr::bad_stake ex)
        {
            pd::Logger::createLogEntry(ex.what(),"ERROR");
            bet_size = std::stoll(askUser(stake_backup,"Please, enter stake value manually"));
            goto ask_user;
    }
    }
    pd::Logger::createLogEntry(m[0].str(),"BET");
}

void pd::Observe::checkFolded()
{
    for(int i(0); i < this->players.size(); ++i)
    {
        if(this->game->actAllowed(i))
            if(this->players[i].folded)
                this->game->playerAction(i,pkr::Actions::fold);
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