#ifndef BBOXES_PD
#define BBOXES_PD

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include <utility>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <map>
#include "vars.h"

#define CONF_THRES 0.7
#define NMS_TRES 0.4
#define INTER_SCORE 0.5
#define PATH_TO_CFG "../res/yolo/yolov3-voc.cfg"
#define PATH_TO_WEIGHTS "../res/yolo/model.weights"
#define PATH_TO_NAMES "../res/yolo/voc.names"

namespace pd
{
    class Models
    {
    private:
        inline static std::map<int,cv::dnn::Net> models;
        inline static int def{-1};
    public:
        // Adds model to the container
        static int addModel(cv::dnn::Net model);
        // Returns bounding boxes of objects and their ids
        static std::vector<pd::Obj> getBboxes(cv::Mat,int key=-1);
        // Sets default model
        static void setDefault(int key);
    };
    // Returns names array from voc.names
    void getNames(std::vector<std::string>&);
    // Returns true if common area of two rects is bigger than INTER_SCORE
    bool isIntersects(cv::Rect,cv::Rect);
}

#endif