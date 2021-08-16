#ifndef TEXT_PD
#define TEXT_PD

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/text/ocr.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <map>

#define WHITE_LIST "0123456789KM:.$, "
#define BET_THR 220
#define POT_THR 200
#define STACK_THR 100
#define CONF_THR 0.5
#define NMS_THR 0.4

namespace pd
{
    namespace tess = tesseract;

    class OCR
    {
    public:
        enum class TextType
        {
            BET,
            POT,
            STACK
        };

        OCR(cv::Mat&& pot_size,cv::Mat&& stake,cv::Mat&& stack);
        ~OCR();

        std::string getText(cv::Mat&&,OCR::TextType&&);
    private:
        tess::TessBaseAPI api;
        std::map<OCR::TextType,int> thr,dilate;
        static int track_thr,track_dilate;

        static void on_thr_trackbar(int,void*);
        static void on_dilate_trackbar(int,void*);
        static void updateFrame(cv::Mat*);
        void adjustThr(cv::Mat&&,OCR::TextType) noexcept;
    };
}

#endif