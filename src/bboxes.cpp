// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../headers/bboxes.h"

std::vector<pd::Obj> pd::getBboxes(cv::Mat img)
{
    double max,min;
    std::vector<std::vector<cv::Mat>> out;
    std::vector<cv::Rect> boxes;
    std::vector<pd::Obj> res;
    std::vector<float> confs;
    std::vector<int> classes;
    cv::Point cls,mn;

    cv::cvtColor(img,img,cv::COLOR_BGR2GRAY);
    cv::cvtColor(img,img,cv::COLOR_GRAY2BGR);
    static cv::dnn::Net net(cv::dnn::readNetFromDarknet(PATH_TO_CFG,PATH_TO_WEIGHTS));
    net.setInput(cv::dnn::blobFromImage(img,1.0/255.0,cv::Size(416,416),cv::Scalar(0,0),true,false));
    net.forward(out,net.getUnconnectedOutLayersNames());
    for(std::vector<cv::Mat> el : out)
    {
        for(int i(0); i < el[0].rows; ++i)
        {
            cv::Mat row(el[0].row(i));
            cv::Mat scores(row.colRange(5,el[0].cols));
            cv::minMaxLoc(scores,&min,&max,&mn,&cls);
            if(max > CONF_THRES)
            {
                cv::Rect rect(cv::Point(int(row.at<float>(0,0)*img.cols-row.at<float>(0,2)*img.cols/2.0),int(row.at<float>(0,1)*img.rows-row.at<float>(0,3)*img.rows/2.0)),cv::Size(int(row.at<float>(0,2)*img.cols),int(row.at<float>(0,3)*img.rows)));

                boxes.push_back(rect);
                confs.push_back(max);
                classes.push_back(cls.x);
            }
        }
    }
    std::vector<int> ind;
    cv::dnn::NMSBoxes(boxes,confs,CONF_THRES,NMS_TRES,ind);
    for(int i(0); i < ind.size(); ++i)
        res.push_back(std::make_pair(boxes[ind[i]],classes[ind[i]]));

    return res;
}

void pd::getNames(std::vector<std::string>& namesArray)
{
    std::ifstream fnames;
    std::string name;
    fnames.open(PATH_TO_NAMES);
    if(fnames.is_open())
    {
        while(std::getline(fnames,name))
            namesArray.push_back(name);
    }
    fnames.close();
}

bool pd::isIntersects(cv::Rect a, cv::Rect b) 
{ 
    return ((double)(a & b).area() / (double)std::min(a.area(),b.area())) > INTER_SCORE; 
}

