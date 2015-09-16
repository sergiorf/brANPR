#ifndef ANPRENGINE
#define ANPRENGINE

#include <QString>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <memory>

class ANPREngine
{
public:
  ANPREngine();
  void run(const std::string& picPath);
  QImage original() const;
  QImage processed1() const;
private:
  QImage mat2QImage(const cv::Mat& src) const;
  QImage _Original;
  QImage _Processed1;
};

#endif // ANPRENGINE
