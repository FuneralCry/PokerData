// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../headers/observe.h"
#include "../headers/bboxes.h"
#include "../headers/text.h"
#include "../headers/vars.h"

#define VIDEO_PATH "../res/example.mp4"
#define FPS 1


int main()
{
    /*
    cv::Mat i(cv::imread("11.jpg"));
    auto boxes(pd::getBboxes(i));
    std::for_each(boxes.begin(),boxes.end(),[i](pd::Obj a){cv::rectangle(i,a.first,cv::Scalar(255,11,467));});
    cv::imwrite("out.jpg",i);
    */
    std::string path(VIDEO_PATH);
    pd::Observe obs(path,FPS);
    obs.start();

    return 0;
}
