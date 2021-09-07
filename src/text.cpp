// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../headers/text.h"

int pd::OCR::track_dilate = 0;
int pd::OCR::track_thr = 0;
std::string pd::OCR::adj_win_name = "Adjust frame";

pd::OCR::OCR(cv::Mat&& pot_size, cv::Mat&& stake, cv::Mat&& stack)
{
    adjustThr(std::move(pot_size),pd::OCR::TextType::POT);
    this->thr.insert({pd::OCR::TextType::POT,track_thr});
    this->dilate.insert({pd::OCR::TextType::POT,track_dilate});
    
    adjustThr(std::move(stake),pd::OCR::TextType::BET);
    this->thr.insert({pd::OCR::TextType::BET,track_thr});
    this->dilate.insert({pd::OCR::TextType::BET,track_dilate});

    adjustThr(std::move(stack),pd::OCR::TextType::STACK);
    this->thr.insert({pd::OCR::TextType::STACK,track_thr});
    this->dilate.insert({pd::OCR::TextType::STACK,track_dilate});

    this->api.Init(NULL,"eng");
    this->api.SetVariable("tessedit_char_whitelist",WHITE_LIST);
    this->api.SetVariable("debug_file", "/dev/null");
    this->api.SetPageSegMode(tess::PageSegMode::PSM_SINGLE_LINE);

    this->model_num = pd::Models::addModel(cv::dnn::readNetFromDarknet("../res/yolo/yolov3-voc_digits.cfg","../res/yolo/model_digits.weights"));
}

std::string pd::OCR::useTesseract(const cv::Mat& frame)
{
    this->api.SetImage(frame.data,frame.cols,frame.rows,4,4*frame.cols);
    char* textc(this->api.GetUTF8Text());
    std::string text(textc);
    delete[] textc;

    return text;
}

std::vector<std::vector<pd::Obj>> groupBboxes(std::vector<pd::Obj> bboxes, const int begin=0)
{
    static auto are_x_merged([](const pd::Obj& a,const pd::Obj& b) { return std::abs(a.first.br().x-b.first.tl().x) < a.first.width; });
    static auto are_y_aligned([](const pd::Obj& a,const pd::Obj& b) { return std::abs(a.first.y - b.first.y) < a.first.height/2; } );
    static auto are_neighbors([&](const pd::Obj& a,const pd::Obj& b){ return are_x_merged(a,b) and are_y_aligned(a,b); });
    std::vector<std::vector<pd::Obj>> groups;
    std::vector<pd::Obj> group{bboxes[begin]};
    
    for(int i(begin); i < bboxes.size()-1; ++i)
    {
        if(are_neighbors(bboxes[i],bboxes[i+1]))
        {
            group.push_back(bboxes[i+1]);
            bboxes.erase(bboxes.begin()+i);
            --i;
        }
        else
        {
            auto gr(groupBboxes(bboxes,i+1));
            for(auto& x : gr)
                groups.push_back(x);
        }
    }
    groups.push_back(group);

    return groups;
}

char yolo2digit(const pd::Obj& x) { return '0' + (x.second + 1) % 10; }

std::string yolo2numbers(const std::vector<pd::Obj>& x)
{
    std::string res;
    for(const pd::Obj& a : x)
        res += yolo2digit(a);
    return res;
}

std::string pd::OCR::useYolo(const cv::Mat& frame)
{
    std::vector<Obj> bboxes(pd::Models::getBboxes(frame,this->model_num));
    std::sort(bboxes.begin(),bboxes.end(),[](const pd::Obj& a,const pd::Obj& b) { return a.first.x < b.first.x; });
    std::vector<std::vector<pd::Obj>> groups(groupBboxes(bboxes));
    std::string* numbers = new std::string[groups.size()];
    std::transform(groups.begin(),groups.end(),numbers,yolo2numbers);
    std::string res(*std::max_element(numbers,numbers+groups.size(),[](const std::string& a,const std::string& b){ return std::stoll(a) > std::stoll(b); }));
    delete[] numbers;

    return res;
}

pd::OCR::~OCR()
{
    this->api.End();
}

void pd::OCR::on_thr_trackbar(int pos, void* frames) { track_thr = pos; updateFrame(reinterpret_cast<cv::Mat*>(frames)); }
void pd::OCR::on_dilate_trackbar(int pos, void* frames) { track_dilate = pos; updateFrame(reinterpret_cast<cv::Mat*>(frames)); }

void pd::OCR::updateFrame(cv::Mat* frames)
{
    cv::threshold(frames[0],frames[1],track_thr,255,cv::THRESH_BINARY);
    auto kernel(cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2*track_dilate+1,2*track_dilate+1),cv::Point(track_dilate,track_dilate)));
    cv::dilate(frames[1],frames[1],kernel);
    cv::imshow(adj_win_name,frames[1]);
}

void pd::OCR::adjustThr(cv::Mat&& frame, OCR::TextType type) noexcept
{
    cv::cvtColor(frame,frame,cv::COLOR_BGR2GRAY);
    cv::resize(frame,frame,cv::Size(),2,2);
    cv::namedWindow(this->adj_win_name);
    cv::Mat origin_filtered[2];
    origin_filtered[0] = frame;
    cv::createTrackbar("Threshold",this->adj_win_name,NULL,255,on_thr_trackbar,&origin_filtered);
    cv::createTrackbar("Dilation",this->adj_win_name,NULL,10,on_dilate_trackbar,&origin_filtered);
    cv::imshow(this->adj_win_name,frame);
    cv::waitKey(0);
    cv::destroyWindow(this->adj_win_name);
    cv::imwrite("tettt.jpg",frame);
}

std::string pd::OCR::getText(cv::Mat&& frame, OCR::TextType&& type)
{
    cv::cvtColor(frame,frame,cv::COLOR_BGR2GRAY);
    cv::cvtColor(frame,frame,cv::COLOR_GRAY2RGBA);
    cv::resize(frame,frame,cv::Size(),2,2);
    cv::threshold(frame,frame,this->thr[type],255,cv::THRESH_BINARY);
    auto kernel(cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2*this->dilate[type]+1,2*this->dilate[type]+1),cv::Point(this->dilate[type],this->dilate[type])));
    cv::dilate(frame,frame,kernel);
    cv::bitwise_not(frame,frame);

    if(type == OCR::TextType::STACK)
        return useTesseract(frame);
    else
        return useYolo(frame);
}