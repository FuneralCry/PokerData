#include "../../headers/observe.h"

void pd::Observe::playerBet(std::vector<pd::Player>::iterator player,cv::Mat&& stake)
{
      // Read bet size string from image
    std::string bet_size_str(pd::getText(std::move(stake),TextType::BET));
      //  Clear it and extract digits
    boost::erase_all(bet_size_str,".");
    boost::erase_all(bet_size_str,",");
    std::regex r("\\d+");
    std::smatch m;
    long long bet_size;
    if(std::regex_search(bet_size_str,m,r))
        bet_size = std::stoll(m[m.size()-1].str());
    else
    {
        std::cout << *game;
        std::cerr << "Can't recognize bet size (" << bet_size_str << ')' <<'\n';
        throw 1;
    }
    // Trigger game
    this->game->playerAction(std::distance(this->players.begin(),player),pkr::bet,bet_size);
}

void pd::Observe::checkFolded(const cv::Mat& frame)
{
    for(int i(0); i < this->players.size(); ++i)
    {
        auto bboxes(pd::getBboxes(frame(this->players[i].getCont())));
        if(std::none_of(bboxes.begin(),bboxes.end(),[](pd::Obj box) { return box.second == (int)pd::Indices::card; }))
            this->game->playerAction(i,pkr::fold);
    }
}

void pd::Observe::processStakes(const cv::Mat& frame, std::vector<cv::Rect> stakes)
{
    // If 'stake' in the middle (pot actually) has gone, then game is over and there is nothing to do
    if(std::none_of(stakes.begin(),stakes.end(),[&](cv::Rect r) { return isIntersects(this->board->getCont(),r); }))
        return;
    // Else trigger game by all stakes except one in the middle (game resolves multiple stakes by itself)
    for(cv::Rect rect : stakes)
    {
        // Find owner of stake
        auto p(whoseRect(rect));
        // If it is not in the middle
        if(p != this->players.end())
            // Trigger game
            playerBet(p,frame(rect));
    }
}