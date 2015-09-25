#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

namespace brANPR
{
  void drawRotatedRect(cv::Mat& image, const std::vector<cv::RotatedRect>& rects, cv::Scalar color = cv::Scalar(255.0, 255.0, 255.0));
  void drawRotatedRect(cv::Mat& image, cv::RotatedRect rRect, cv::Scalar color = cv::Scalar(255.0, 255.0, 255.0));
  void inflate(cv::Rect& rect, double perc);
  std::string getFilename(const std::string& s);
  void trainPlates(int numPlates, int numNoPlates, const std::string& path_Plates, const std::string& path_NoPlates);
  std::string applyCountryRules(const std::string& licensePlate);
};