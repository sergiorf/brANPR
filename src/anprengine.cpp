#include "anprengine.h"
#include <qcolor.h>
#include <memory>
#include <iostream>

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
  _Processed1 = mat2QImage(img_gray);
}

QImage ANPREngine::original() const
{
  return _Original;
}

QImage ANPREngine::processed1() const
{
  return _Processed1;
}

QImage ANPREngine::mat2QImage(const cv::Mat& inMat) const
{
  switch (inMat.type())
  {
  case CV_8UC4:
    {
      QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32);
      image.bits();
      return image;
    }
  case CV_8UC3:
    {
      QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888);
      image.bits();
      return image.rgbSwapped();
    }
  case CV_8UC1:
    {
      cv::Mat rgb;
      cv::cvtColor(inMat, rgb, CV_GRAY2BGR);
      cv::cvtColor(rgb, rgb, CV_BGR2BGRA);
      QImage temp = QImage(static_cast<unsigned char*>(rgb.data), rgb.cols, rgb.rows, QImage::Format_ARGB32);
      QImage image = temp.copy();
      return image;
    }
  default:
    cerr << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
    break;
  }
  return QImage();
}
