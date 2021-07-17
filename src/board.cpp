// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../headers/board.h"

namespace pd
{
    pd::Board::Board(const cv::Mat& frame, const cv::Rect& cont, pd::EventGuard& event)
    {
        this->cont = cont;
        this->update(frame,cont,event);
    }

    Board::operator std::vector<pkr::Card>() const
    {
        std::vector<pkr::Card> res;
        for(pd::Card c : this->cards)
            res.push_back(c);
        
        return res;
    }

    std::vector<pd::Card> Board::getCards() const{ return this->cards; }

    long long Board::getPotSize() const { return this->pot_size; }

    cv::Rect Board::getCont() const { return this->cont; }

    bool Board::isEmpty() const { return this->cards.size() == 0; }

    void Board::updatePot(cv::Mat&& pot_mat)
    {
        std::string pot_str(pd::getText(std::move(pot_mat),pd::TextType::POT));
        boost::erase_all(pot_str,".");
        boost::erase_all(pot_str,",");
        std::regex r("\\d+");
        std::smatch m;
        if(std::regex_search(pot_str,m,r))
            pot_str = m[0].str();
        this->pot_size = std::stoll(pot_str);
    }

    void Board::update(const cv::Mat& frame, const cv::Rect& cont, pd::EventGuard& event)
    {
        this->cont = cont;
        cv::Mat board(frame(cont));
        std::vector<pd::Obj> bboxes(pd::getBboxes(frame(cont)));
        std::vector<std::pair<cv::Rect,std::vector<int>>> cards; // Contour of card and elements inside
        for(pd::Obj rect : bboxes)
        {
            if(rect.second == (int)pd::Indices::card)
                cards.push_back(std::make_pair(rect.first,std::vector<int>()));
            if(rect.second == (int)pd::Indices::pot_size)
                pot_size_cont = rect.first;
        }
        assert(cards.size() <= 5 & not pot_size_cont.empty());
        if(cards.size() < 3 and cards.size() > 0)
            throw pd::InterimFrame("Board");
        else if(cards.size() > this->cards.size())  // If there are more cards then eralier...
        {
            event.update(pd::Events::NEW_BOARD_CARD);
            for(auto& card : cards)  // Prepare cards contours for processing
            {
                for(pd::Obj bbox : bboxes)
                {
                    if(pd::isIntersects(bbox.first,card.first))
                        card.second.push_back(bbox.second);
                }
            }
            for(auto& card : cards)  // Processing
            {
                pd::Card c(card.first,card.second);  // Create instance of pd::Card
                if(std::find(this->cards.begin(),this->cards.end(),c) == this->cards.end())  // If there is one inside the container...
                {
                    std::cout << "NEW CARD: " << pkr::CardValuesOut[c.getValue()] << pkr::CardSuitsOut[c.getSuit()] << '\n';
                    this->cards.push_back(c);  // push it to container
                }
            }
            // Update pot size
            updatePot(board(pot_size_cont));
        }
        else if(cards.size() < this->cards.size())  // If board has been cleared...
        {
            event.update(pd::Events::NEW_GAME);
            this->cards.clear();
            updatePot(board(pot_size_cont));  // update pot size
        }
        else
            event.update(pd::Events::NU);  // Otherwise do nothing and return NULL event
    }
}