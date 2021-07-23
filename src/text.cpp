// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../headers/text.h"

namespace pd
{
    pd::OCR::OCR()
    {
        api.Init(NULL,"eng");
    }

    pd::OCR::~OCR()
    {
        api.End();
    }

    std::string getText(cv::Mat&& image, TextType&& type)
    {
        std::vector<std::vector<cv::Point>> contours;
        int thr,kernel_size;
        tess::PageSegMode psm(tess::PSM_SINGLE_LINE);
        switch(type)
        {
            case TextType::STACK:
                thr = STACK_THR;
                kernel_size = 0;
                psm = tess::PSM_SINGLE_LINE;
                break;
            case TextType::POT:
                thr = POT_THR;
                kernel_size = 0;
                psm = tess::PSM_SINGLE_LINE;
                break;
            case TextType::BET:
                thr = BET_THR;
                kernel_size = 2;
                psm = tess::PSM_SINGLE_BLOCK;
                cv::Rect roi(0,std::max(0,image.rows-65),image.cols,std::min(65,image.rows));
                image = image(roi);
                break;
        }
        thr = 200;
        cv::cvtColor(image,image,cv::COLOR_BGR2GRAY);
        cv::cvtColor(image,image,cv::COLOR_GRAY2RGBA);
        image.convertTo(image,-1,1,50);
        cv::imwrite("bet1.jpg",image);
        cv::resize(image,image,cv::Size(),2,2);
        cv::threshold(image,image,thr,255,cv::THRESH_BINARY);
        auto kernel(cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2*kernel_size+1,2*kernel_size+1),cv::Point(kernel_size,kernel_size)));
        cv::morphologyEx(image,image,cv::MORPH_OPEN,kernel,cv::Point(-1,-1));
        kernel_size = 1;
        kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2*kernel_size+1,2*kernel_size+1),cv::Point(kernel_size,kernel_size));
        cv::erode(image,image,kernel);
        cv::bitwise_not(image,image);
        
        
        static pd::OCR ocr;
        ocr.api.SetImage(image.data,image.cols,image.rows,4,4*image.cols);
        ocr.api.SetVariable("tessedit_char_whitelist",WHITE_LIST);
        ocr.api.SetVariable("debug_file", "/dev/null");
        ocr.api.SetPageSegMode(psm);
        char* textc(ocr.api.GetUTF8Text());
        std::string text(textc);
        cv::imwrite("1.jpg",image);
        delete[] textc;

        return text;
    }
}