// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../headers/bboxes.h"

int pd::Models::addModel(cv::dnn::Net model)
{
    if(not models.size())
        def = 0;
    models.insert({models.size(),model});

    return models.size()-1;
}

std::vector<pd::Obj> pd::Models::getBboxes(cv::Mat img, int key)
{
    if(key < 0)
        key = def;

    if(models.find(key) == models.end())
        throw std::invalid_argument("Invalid key");

    double max,min;
    std::vector<std::vector<cv::Mat>> out;
    std::vector<cv::Rect> boxes;
    std::vector<pd::Obj> res;
    std::vector<float> confs;
    std::vector<int> classes;
    cv::Point cls,mn;

    // Make image gray
    cv::cvtColor(img,img,cv::COLOR_BGR2GRAY);
    cv::cvtColor(img,img,cv::COLOR_GRAY2BGR);
    // Create blob and push it through yolo
    models[key].setInput(cv::dnn::blobFromImage(img,1.0/255.0,cv::Size(416,416),cv::Scalar(0,0),true,false));
    models[key].forward(out,models[key].getUnconnectedOutLayersNames());
    // Collect results from all three yolo layers
    for(std::vector<cv::Mat> el : out)
    {
        // Iterate over them
        for(int i(0); i < el[0].rows; ++i)
        {
            cv::Mat row(el[0].row(i));
            // Extract confidence scores and find max
            cv::Mat scores(row.colRange(5,el[0].cols));
            cv::minMaxLoc(scores,&min,&max,&mn,&cls);
            // If it bigger than threshold...
            if(max > CONF_THRES)
            {
                // Extract box coordinates
                int box_width(row.at<float>(0,2)*img.cols),
                    box_height(row.at<float>(0,3)*img.rows);
                int box_x(row.at<float>(0,0)*img.cols-box_width/2.0),
                    box_y(row.at<float>(0,1)*img.rows-box_height/2.0);
                
                // Box coordinates must not be out of bounds of image
                box_x = std::max(0,box_x);
                box_y = std::max(0,box_y);
                box_width = std::min(img.cols-box_x,box_width);
                box_height = std::min(img.rows-box_y,box_height);

                cv::Rect rect(
                    cv::Point(box_x,box_y),
                    cv::Size(box_width,box_height)
                    );
                // Push results to containers
                boxes.push_back(rect);
                confs.push_back(max);
                classes.push_back(cls.x);
            }
        }
    }
    std::vector<int> ind;
    // NMS thresholding
    cv::dnn::NMSBoxes(boxes,confs,CONF_THRES,NMS_TRES,ind);
    // Create output container box-class_id
    for(int i(0); i < ind.size(); ++i)
        res.push_back(std::make_pair(boxes[ind[i]],classes[ind[i]]));

    return res;
}

void pd::Models::setDefault(int key)
{
    if(key >= 0 and key < models.size())
        def = key;
    else
        throw std::invalid_argument("Key out of range");
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

