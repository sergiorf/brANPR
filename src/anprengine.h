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
private:
  QImage mat2QImage(const cv::Mat& src) const;
  QImage _Original;
};

#endif // ANPRENGINE
