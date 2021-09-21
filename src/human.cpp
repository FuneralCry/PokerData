#include "../headers/human.h"

namespace pd
{
    std::string askUser(const cv::Mat& img, std::string_view message)
    {
        pd::Logger::createLogEntry(static_cast<std::string>(message),"MANUAL");
        std::string win_name("Help request");
        cv::namedWindow(win_name);
        cv::imshow(win_name,img);
        cv::waitKey(1000);
        std::string input(pd::Logger::requestInput(">> "));
        cv::destroyWindow(win_name);
        
        return input;
    }
}