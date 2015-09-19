#include "anprengine.h"
#include <qcolor.h>
#include <memory>
#include <iostream>
#include "utils.h"
#include <time.h>
#include <random>

using namespace std;
using namespace cv;

#define YELLOW CV_RGB(255, 255, 0)
#define GREEN  CV_RGB(0, 255, 0)
#define BLUE   CV_RGB(0, 0, 255)
#define RED    CV_RGB(255, 0, 0)

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
    vector<vector<Point>> contours;
    vector<RotatedRect> rects;
    findContours(processed, contours, rects);


    // Draw blue contours on a white image
    cv::Mat result;
    input.copyTo(result);
    cv::drawContours(result, contours,
      -1, // draw all contours
      BLUE, 1); // with a thickness of 1

    for (int i = 0; i < rects.size(); i++){
      //For better rect cropping for each possible box
      //Make floodfill algorithm because the plate has white background
      //And then we can retrieve more clearly the contour box
      circle(result, rects[i].center, 3, GREEN, -1);
      //get the min size between width and height
      auto minSize = min(rects[i].size.width, rects[i].size.height) * 0.5;
      //initialize rand and get 5 points around center for floodfill algorithm
      srand(time(nullptr));
      //Initialize floodfill parameters and variables
      Mat mask;
      mask.create(input.rows + 2, input.cols + 2, CV_8UC1);
      mask = Scalar::all(0);
      const Scalar loDiff{30, 30, 30}, upDiff{30, 30, 30};
     // const Scalar loDiff{ 20, 20, 20 }, upDiff{ 20, 20, 20 };
      const auto connectivity = 4;
      const auto newMaskVal = 255;
      const auto numSeeds = 10;
      Rect ccomp;
      int flags = connectivity + (newMaskVal << 8) + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;
      for (int j = 0; j < numSeeds; j++){
        Point seed;
        seed.x = rects[i].center.x + rand() % static_cast<int>(minSize) - (minSize / 2);
        seed.y = rects[i].center.y + rand() % static_cast<int>(minSize) - (minSize / 2);
        circle(result, seed, 1, YELLOW, -1);
        auto area = floodFill(input, mask, seed, BLUE, &ccomp, loDiff, upDiff, flags);
      }
      imshow("MASK", mask);
      cvWaitKey(0);

      //Check new floodfill mask match for a correct patch.
      //Get all points detected for get Minimal rotated Rect
      vector<Point> pointsInterest;
      Mat_<uchar>::iterator itMask = mask.begin<uchar>();
      Mat_<uchar>::iterator end = mask.end<uchar>();
      for (; itMask != end; ++itMask)
      if (*itMask == 255)
        pointsInterest.push_back(itMask.pos());
      RotatedRect minRect = minAreaRect(pointsInterest);
      if (verifySizes(minRect))
      {
        drawRotatedRect(input, minRect, RED);
      }
    }
    imshow("result",result);

    
    drawRotatedRect(processed, rects, YELLOW);
    drawRotatedRect(input, rects, YELLOW);
    _Original = mat2QImage(input);
    _Processed1 = mat2QImage(processed);
  }

  void ANPREngine::findContours(const InputOutputArray& src, vector<vector<Point>>&  contours, 
    vector<RotatedRect>& boundingRects) const
  {
    //Find contours of possibles plates
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
        boundingRects.push_back(mr);
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
