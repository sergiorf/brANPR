#include "utils.h"

using namespace cv;
using namespace std;

namespace brANPR
{
  void drawRotatedRect(Mat& image, const vector<RotatedRect>& rects, Scalar color)
  {
    for (auto rec : rects)
      drawRotatedRect(image, rec, color);
  }

  void drawRotatedRect(Mat& image, RotatedRect rRect, Scalar color)
  {
    Point2f vertices[4];
    rRect.points(vertices);
    for (auto i = 0; i < 4; i++)
      line(image, vertices[i], vertices[(i + 1) % 4], color);
  }
};