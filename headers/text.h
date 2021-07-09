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

#define WHITE_LIST "0123456789KM:.$, "
#define BET_THR 220
#define POT_THR 200
#define STACK_THR 100
#define CONF_THR 0.5
#define NMS_THR 0.4

namespace pd
{
    namespace tess = tesseract;

    enum class TextType
    {
        BET,
        POT,
        STACK
    };

    class OCR
    {
    public:
        tess::TessBaseAPI api;

        OCR();
        ~OCR();
    };

    std::string getText(cv::Mat&&,TextType&&);
    void detectText(cv::Mat&, cv::Rect&);
}

#endif