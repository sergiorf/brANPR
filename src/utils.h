#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>

namespace brANPR
{
  void drawRotatedRect(cv::Mat& image, const std::vector<cv::RotatedRect>& rects, cv::Scalar color = cv::Scalar(255.0, 255.0, 255.0));
  void drawRotatedRect(cv::Mat& image, cv::RotatedRect rRect, cv::Scalar color = cv::Scalar(255.0, 255.0, 255.0));
  void inflate(cv::Rect& rect, double perc);
  std::string getFilename(const std::string& s);
  void trainPlates(int numPlates, int numNoPlates, const std::string& path_Plates, const std::string& path_NoPlates);
  std::string applyCountryRules(const std::string& licensePlate);

  inline std::string& ltrim(std::string& s)
  {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
  }

  inline std::string& rtrim(std::string& s)
  {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
  }

  inline std::string& trim(std::string& s)
  {
    return ltrim(rtrim(s));
  }
};
