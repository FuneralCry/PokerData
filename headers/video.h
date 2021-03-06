#ifndef VIDEO_PD
#define VIDEO_PD

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/text/ocr.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <iostream>
#include <stdexcept>
#include "timer.h"

class VideoPlayer : public Timer
{
private:
    cv::VideoCapture video;
    long duration;
    double fps;
    cv::Mat frame;
public:
    VideoPlayer() = delete;
    VideoPlayer(const std::string& path);

    void setFps(double fps);  // Sets fps
    cv::Mat play();  // Returns next frame
    bool isPlaying() const;  // Returns true if opened
    cv::Mat skip(double secs);  // Skip frames according to secs
    cv::Mat get() const;  // Returns current frame
    long getDuration() const;
    long getTime() const override;
};

#endif