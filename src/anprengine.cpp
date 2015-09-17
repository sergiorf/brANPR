#include "anprengine.h"
#include <qcolor.h>
#include <memory>
#include <iostream>
#include "utils.h"

using namespace std;
using namespace cv;

#define YELLOW CV_RGB(255, 255, 0)

namespace brANPR
{
  ANPREngine::ANPREngine()
  {
  }

  void ANPREngine::run(const string& picPath)
  {
    auto input = imread(picPath, CV_LOAD_IMAGE_COLOR);
    Mat processed;
    preprocess(input, processed);
    vector<RotatedRect> rects;
    findContours(processed, rects);
    drawRotatedRect(processed, rects, YELLOW);
    drawRotatedRect(input, rects, YELLOW);
    _Original = mat2QImage(input);
    _Processed1 = mat2QImage(processed);
  }

  void ANPREngine::findContours(const InputOutputArray& src, vector<RotatedRect>& rects) const
  {
    //Find contours of possibles plates
    vector<vector<Point>> contours;
    cv::findContours(src,
      contours, // a vector of contours
      CV_RETR_EXTERNAL, // retrieve the external contours
      CV_CHAIN_APPROX_NONE); // all pixels of each contour
    //Start to iterate to each contour found
    vector<vector<Point>>::iterator itc = contours.begin();
    //Remove patch that has no inside limits of aspect ratio and area.
    while (itc != contours.end())
    {
      //Create bounding rect of object
      RotatedRect mr = minAreaRect(Mat(*itc));
      if (!verifySizes(mr))
      {
        itc = contours.erase(itc);
      }
      else
      {
        ++itc;
        rects.push_back(mr);
      }
    }
  }

  QImage ANPREngine::original() const
  {
    return _Original;
  }

  QImage ANPREngine::processed1() const
  {
    return _Processed1;
  }

  QImage ANPREngine::sobel() const
  {
    return _Sobel;
  }

  bool ANPREngine::verifySizes(RotatedRect candidate) const
  {
    float error = 0.4;
    //Brazil car plate size: 40x13 aspect 3.077
    const float aspect = 3.077;
    //Set a min and max area. All other patches are discarded
    const double minh = 25, maxh = 100;
    const int min = pow(minh, 2) * aspect; // minimum area
    const int max = pow(maxh, 2) * aspect; // maximum area
    //Get only patches that match to a respect ratio.
    auto rmin = aspect - aspect*error;
    auto rmax = aspect + aspect*error;
    int area = candidate.size.height * candidate.size.width;
    auto r = static_cast<float>(candidate.size.width) / static_cast<float>(candidate.size.height);
    if (r < 1)
      r = 1 / r;
    return ((area < min || area > max) || (r < rmin || r > rmax)) ? false : true;
  }

  void ANPREngine::preprocess(InputArray src, OutputArray dst)
  {
    Mat img_gray;
    Mat tmp;
    double kernel_length = 6;
    bilateralFilter(src, tmp, kernel_length, kernel_length * 2, kernel_length / 2);
    cvtColor(tmp, img_gray, CV_BGR2GRAY);
    //Detect vertical edges using Sobel filter. Car plates have high density of vertical lines
    Mat img_sobel;
    const auto xorder = 1, yorder = 0;
    Sobel(img_gray, img_sobel, CV_8U, xorder, yorder);
    convertScaleAbs(img_sobel, img_sobel);
    _Sobel = mat2QImage(img_sobel);
    threshold(img_sobel, dst, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
    //Connect all regions that have high density of edges
    auto element = getStructuringElement(MORPH_RECT, Size(17, 5));
    morphologyEx(dst, dst, CV_MOP_CLOSE, element);
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
                  auto temp = QImage(static_cast<unsigned char*>(rgb.data), rgb.cols, rgb.rows, QImage::Format_ARGB32);
                  auto image = temp.copy();
                  return image;
    }
    default:
      cerr << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
      break;
    }
    return QImage();
  }
}
