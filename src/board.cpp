// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../headers/board.h"

pd::Board::Board(pd::OCR* ocr,const cv::Mat& frame, const cv::Rect& cont) : ocr(ocr), cont(cont)
{
    this->update(frame,cont);
}

pd::Board::operator std::vector<pkr::Card>() const
{
    std::vector<pkr::Card> res;
    for(pd::Card c : this->cards)
        res.push_back(c);
    
    return res;
}

std::vector<pd::Card> pd::Board::getCards() const{ return this->cards; }

long long pd::Board::getPotSize() const { return this->pot_size; }

cv::Rect pd::Board::getCont() const { return this->cont; }

bool pd::Board::isEmpty() const { return this->cards.size() == 0; }

void pd::Board::updatePot(cv::Mat&& pot_mat)
{
    std::string pot_str(this->ocr->getText(std::move(pot_mat),pd::OCR::TextType::POT));
    boost::erase_all(pot_str,".");
    boost::erase_all(pot_str,",");
    std::regex r("\\d+");
    std::smatch m;
    if(std::regex_search(pot_str,m,r))
        pot_str = m[0].str();
    this->pot_size = std::stoll(pot_str);
    pd::Logger::createLogEntry(m[0].str(),"POT");
}

void pd::Board::update(const cv::Mat& frame, const cv::Rect& cont)
{
    this->cont = cont;
    cv::Mat board(frame(cont));
    // Find objects on the board
    std::vector<pd::Obj> bboxes(pd::Models::getBboxes(frame(cont)));
    // Contour of card and elements inside
    std::vector<std::pair<cv::Rect,std::vector<int>>> cards;
    for(pd::Obj rect : bboxes)
    {
        if(rect.second == (int)pd::Indices::card)
            cards.push_back(std::make_pair(rect.first,std::vector<int>()));
        if(rect.second == (int)pd::Indices::pot_size)
            pot_size_cont = rect.first;
    }
    // Check if there are suitable amount of cards and pot size has been found
    assert(cards.size() <= 5 and not pot_size_cont.empty());
    // If there are more cards then eralier...
    if(cards.size() > this->cards.size())
    {
        // For all cards find elements inside
        for(auto& card : cards)
        {
            for(pd::Obj bbox : bboxes)
            {
                if(pd::isIntersects(bbox.first,card.first))
                    card.second.push_back(bbox.second);
            }
        }
        // Processing
        for(auto& card : cards)
        {
            // Create instance of pd::Card
            pd::Card c(card.first,card.second);
            // If there is one inside the container...
            if(std::find(this->cards.begin(),this->cards.end(),c) == this->cards.end())
            {
                // push it to container
                this->cards.push_back(c);
            }
        }
        // Update pot size
        updatePot(board(pot_size_cont));
    }
}