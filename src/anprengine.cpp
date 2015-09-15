#include "anprengine.h"

using namespace std;
using namespace cv;

ANPREngine::ANPREngine()
{
}

void ANPREngine::run(const string& picPath)
{
  Mat input = imread(picPath, CV_LOAD_IMAGE_COLOR);
  _Original = mat2QImage(input);
  Mat img_gray;
  cvtColor(input, img_gray, CV_BGR2GRAY);
  blur(img_gray, img_gray, Size(5, 5));
}

QImage ANPREngine::original() const
{
  return _Original;
}

QImage ANPREngine::mat2QImage(const cv::Mat& src) const
{
  cv::Mat temp;
  cvtColor(src, temp, CV_BGR2RGB);
  QImage dest(static_cast<const uchar*>(temp.data), temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
  dest.bits(); // enforce deep copy
  return dest;
}