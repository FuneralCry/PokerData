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
#include "vars.h"

#define CONF_THRES 0.5
#define NMS_TRES 0.4
#define INTER_SCORE 0.75
#define PATH_TO_CFG "../res/yolo/yolov3-voc.cfg"
#define PATH_TO_WEIGHTS "../res/yolo/model.weights"
#define PATH_TO_NAMES "../res/yolo/voc.names"

namespace pd
{
    // Returns bounding boxes of objects and their ids
    std::vector<pd::Obj> getBboxes(cv::Mat);
    // Returns names array from voc.names
    void getNames(std::vector<std::string>&);
    // Returns true if common area of two rects is bigger than INTER_SCORE
    bool isIntersects(cv::Rect,cv::Rect);
}