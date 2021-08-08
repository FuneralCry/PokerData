#include "../headers/video.h"

VideoPlayer::VideoPlayer(const std::string& path)
{
    if(not video.open(path))
        throw std::logic_error("Can't open video file");
    this->fps = video.get(cv::CAP_PROP_FPS);
    this->duration = video.get(cv::CAP_PROP_FRAME_COUNT) / video.get(cv::CAP_PROP_FPS);
}

void VideoPlayer::setFps(double fps) { this->fps = fps; }

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
    
    this->frame = frame;
    
    return frame;
}

bool VideoPlayer::isPlaying() const { return video.isOpened(); }

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

    this->frame = frame;

    return frame;
}

cv::Mat VideoPlayer::get() const { return frame; }

long VideoPlayer::getDuration() const { return this->duration; }

long VideoPlayer::getTime() const { return video.get(cv::CAP_PROP_POS_FRAMES) / video.get(cv::CAP_PROP_FPS); }