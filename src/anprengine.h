#ifndef ANPRENGINE
#define ANPRENGINE

#include <QString>
#include <QImage>
#include <opencv2/opencv.hpp>

namespace brANPR
{
  class ANPREngine
  {
  public:
    ANPREngine();
    void run(const std::string& picPath);
    QImage original() const;
    QImage sobel() const;
    QImage processed1() const;
  private:
    QImage mat2QImage(const cv::Mat& src) const;
    void findContours(const cv::InputOutputArray& src, std::vector<std::vector<cv::Point>>&  contours, 
      std::vector<cv::RotatedRect>& boundingRects) const;
    void preprocess(cv::InputArray src, cv::OutputArray dst);
    bool verifySizes(cv::RotatedRect candidate) const;
    QImage _Original;
    QImage _Sobel;
    QImage _Processed1;
  };
};

#endif // ANPRENGINE
