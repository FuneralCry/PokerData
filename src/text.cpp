// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../headers/text.h"

namespace pd
{
    int pd::OCR::track_dilate = 0;
    int pd::OCR::track_thr = 0;
    std::string pd::OCR::adj_win_name = "Adjust image";

    pd::OCR::OCR(cv::Mat&& pot_size, cv::Mat&& stake, cv::Mat&& stack)
    {
        adjustThr(std::move(pot_size),pd::OCR::TextType::POT);
        this->thr.insert({pd::OCR::TextType::POT,track_thr});
        this->dilate.insert({pd::OCR::TextType::POT,track_dilate});
        
        adjustThr(std::move(stake),pd::OCR::TextType::BET);
        this->thr.insert({pd::OCR::TextType::BET,track_thr});
        this->dilate.insert({pd::OCR::TextType::BET,track_dilate});

        adjustThr(std::move(stack),pd::OCR::TextType::STACK);
        this->thr.insert({pd::OCR::TextType::STACK,track_thr});
        this->dilate.insert({pd::OCR::TextType::STACK,track_dilate});

        this->api.Init(NULL,"eng");
        this->api.SetVariable("tessedit_char_whitelist",WHITE_LIST);
        this->api.SetVariable("debug_file", "/dev/null");
        this->api.SetPageSegMode(tess::PageSegMode::PSM_SINGLE_LINE);
    }

    pd::OCR::~OCR()
    {
        this->api.End();
    }

    void pd::OCR::on_thr_trackbar(int pos, void* frames) { track_thr = pos; updateFrame(reinterpret_cast<cv::Mat*>(frames)); }
    void pd::OCR::on_dilate_trackbar(int pos, void* frames) { track_dilate = pos; updateFrame(reinterpret_cast<cv::Mat*>(frames)); }

    void pd::OCR::updateFrame(cv::Mat* frames)
    {
        cv::threshold(frames[0],frames[1],track_thr,255,cv::THRESH_BINARY);
        auto kernel(cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2*track_dilate+1,2*track_dilate+1),cv::Point(track_dilate,track_dilate)));
        cv::dilate(frames[1],frames[1],kernel);
        cv::imshow(adj_win_name,frames[1]);
    }

    void pd::OCR::adjustThr(cv::Mat&& frame, OCR::TextType type) noexcept
    {
        cv::cvtColor(frame,frame,cv::COLOR_BGR2GRAY);
        cv::namedWindow(this->adj_win_name);
        cv::Mat origin_filtered[2];
        origin_filtered[0] = frame;
        cv::createTrackbar("Threshold",this->adj_win_name,NULL,255,on_thr_trackbar,&origin_filtered);
        cv::createTrackbar("Dilation",this->adj_win_name,NULL,255,on_dilate_trackbar,&origin_filtered);
        cv::imshow(this->adj_win_name,frame);
        cv::waitKey(0);
        cv::destroyWindow(this->adj_win_name);
    }

    std::string pd::OCR::getText(cv::Mat&& image, OCR::TextType&& type)
    {
        cv::cvtColor(image,image,cv::COLOR_BGR2GRAY);
        cv::cvtColor(image,image,cv::COLOR_GRAY2RGBA);
        cv::resize(image,image,cv::Size(),2,2);
        cv::threshold(image,image,this->thr[type],255,cv::THRESH_BINARY);
        auto kernel(cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2*this->dilate[type]+1,2*this->dilate[type]+1),cv::Point(this->dilate[type],this->dilate[type])));
        cv::dilate(image,image,kernel);
        cv::bitwise_not(image,image);

        this->api.SetImage(image.data,image.cols,image.rows,4,4*image.cols);
        char* textc(this->api.GetUTF8Text());
        std::string text(textc);
        cv::imwrite("1.jpg",image);
        delete[] textc;

        return text;
    }
}