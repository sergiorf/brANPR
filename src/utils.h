#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

namespace brANPR
{
  void drawRotatedRect(cv::Mat& image, const std::vector<cv::RotatedRect>& rects, cv::Scalar color = cv::Scalar(255.0, 255.0, 255.0));
  void drawRotatedRect(cv::Mat& image, cv::RotatedRect rRect, cv::Scalar color = cv::Scalar(255.0, 255.0, 255.0));
};