#include "anprengine.h"
#include <opencv2/opencv.hpp>

using namespace std;

ANPREngine::ANPREngine()
{
}

void ANPREngine::run(const string& picPath)
{
    cv::Mat mat = cv::imread(picPath, CV_LOAD_IMAGE_COLOR);
}

QImage original()
{
}
