#include "../headers/video.h"

VideoPlayer::VideoPlayer(const std::string& path)
{
    if(not video.open(path))
    {
        std::cerr << "Can't open video file" << '\n';
        throw 1;
    }
    this->fps = video.get(cv::CAP_PROP_FPS);
}

void VideoPlayer::setFps(int fps) { this->fps = fps; }

cv::Mat VideoPlayer::play()
{
    cv::Mat frame;
    for(long i(0); i < video.get(cv::CAP_PROP_FPS)/this->fps; ++i)
    {
        if(video.isOpened())
            video.read(frame);
        else
            break;
    }
    
    return frame;
}

bool VideoPlayer::isPlaying() { return video.isOpened(); }

cv::Mat VideoPlayer::skip(double secs)
{
    cv::Mat frame;
    for(long i(0); i < video.get(cv::CAP_PROP_FPS)*secs; ++i)
    {
        if(video.isOpened())
            video.read(frame);
        else
            break;
    }

    return frame;
}