#ifndef VIDEO_PD
#define VIDEO_PD

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/text/ocr.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <iostream>

class VideoPlayer
{
private:
    cv::VideoCapture video;
    long time_pos;
    int fps;
    cv::Mat frame;
public:
    VideoPlayer() = delete;
    VideoPlayer(const std::string& path);

    void setFps(int fps);  // Sets fps
    cv::Mat play();  // Returns next frame
    bool isPlaying();  // Returns true if opened
    cv::Mat skip(double secs);  // Skip frames according to secs
};

#endif