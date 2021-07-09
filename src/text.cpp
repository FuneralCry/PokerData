// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../headers/text.h"
#include <opencv2/core/ocl.hpp>

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
        int thr,kernel_size;
        tess::PageSegMode psm;
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
                psm = tess::PSM_AUTO;
                break;
        }
        cv::cvtColor(image,image,cv::COLOR_BGR2GRAY);
        cv::cvtColor(image,image,cv::COLOR_GRAY2RGBA);
        image.convertTo(image,-1,1,50);
        cv::imwrite("bet1.jpg",image);
        cv::resize(image,image,cv::Size(),2,2);
        cv::threshold(image,image,thr,255,cv::THRESH_BINARY);
        auto kernel(cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2*kernel_size+1,2*kernel_size+1),cv::Point(kernel_size,kernel_size)));
        cv::morphologyEx(image,image,cv::MORPH_OPEN,kernel);
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

    void detectText(cv::Mat& image, cv::Rect& bbox)
    {
        cv::Mat r_image;
        cv::resize(image,r_image,cv::Size(image.cols/32*32,image.rows/32*32));

        std::vector<std::vector<cv::Point>> bboxes;
        std::vector<float> confs;
        cv::dnn::TextDetectionModel_EAST dtctr("east_text_detection.pb");
        dtctr.setConfidenceThreshold(CONF_THR).setNMSThreshold(NMS_THR);

        double detScale(1.0);
        cv::Size inputSize(cv::Size(r_image.cols,r_image.rows));
        cv::Scalar detMean = cv::Scalar(123.68, 116.78, 103.94);
        bool swapRB = true;

        dtctr.setInputParams(detScale,inputSize,detMean,swapRB);
        dtctr.detect(image,bboxes);

        if(bboxes.size())
        {
            std::vector<cv::Point> pnts(bboxes[0]);
            std::sort(pnts.begin(),pnts.end(),[](cv::Point& a,cv::Point& b){ return std::pow(a.x,2)+std::pow(a.y,2) > std::pow(a.y,2)+std::pow(b.y,2); });
            bbox = cv::Rect(pnts[0],pnts[pnts.size()-1]);
        }
        else
        {
            std::cerr << "Text hasn't been found." << '\n';
            throw 1;
        }
    }
}