#ifndef ANPRENGINE
#define ANPRENGINE

#include <QString>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "OCR.h"
#include "detectregions.h"

namespace brANPR
{
  struct ANPRSettings
  {
    OCRSettings OCR;
    DetectRegionsSettings DetectRegions;
  };

  class ANPREngine
  {
  public:
    ANPREngine();
    vector<std::string> run(const std::string& picPath);
    void reset();
    QImage original() const;
    QImage segmented() const;
    QImage processed_plates() const;
    QImage plate_detected() const;
    ANPRSettings getSettings() const;
    void setSettings(const ANPRSettings& settings);
  private:
    QImage mat2QImage(const cv::Mat& src) const;
    void findContours(const cv::InputOutputArray& src, std::vector<std::vector<cv::Point>>& contours,
      std::vector<cv::RotatedRect>& boundingRects) const;
    void preprocess(cv::InputArray src, cv::OutputArray dst);
    bool verifySizes(cv::RotatedRect candidate) const;
    QImage _Original;
    QImage _Segmented;
    QImage _ProcessedPlates;
    QImage _PlateDetected;
    ANPRSettings _Settings;
  };
};

#endif // ANPRENGINE
