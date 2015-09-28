#pragma once

#include <vector>
#include "Plate.h"

using namespace std;
using namespace cv;

namespace brANPR
{
  struct DetectRegionsSettings
  {
    bool saveSegments;
    bool showSteps;
  };

  class DetectRegions
  {
  public:
    DetectRegions(const DetectRegionsSettings& settings);
    string filename;
    string filename_notpath;
    void setFilename(const string& f);
    vector<Plate> run(Mat& result);
  private:
    vector<Plate> segment(const Mat& input, Mat& resut) const;
    bool verifySizes(RotatedRect mr) const;
    Mat histeq(Mat in) const;
    bool saveRegions;
    bool showSteps;
  };
};
