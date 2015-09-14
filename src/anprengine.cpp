#include "anprengine.h"
#include <opencv2/opencv.hpp>

using namespace std;
//using namespace cv;

ANPREngine::ANPREngine()
{
}

void ANPREngine::run(const string& picPath)
{
   cv::imread("test", CV_LOAD_IMAGE_COLOR);
 //cv::Mat mat = cv::imread(picPath, CV_LOAD_IMAGE_COLOR);
}

/*
QImage original()
{
}
*/
