#ifndef DetectRegions_h
#define DetectRegions_h

#include <string.h>
#include <vector>

#include "Plate.h"

#include <opencv2/opencv.hpp>
//#include <highgui.h>
//#include <cvaux.h>

using namespace std;
using namespace cv;

class DetectRegions{
    public:
        DetectRegions();
        string filename;
        string filename_notpath;
        void setFilename(const string& f);
        bool saveRegions;
        bool showSteps;
        vector<Plate> run(Mat& result);
    private:
        vector<Plate> segment(const Mat& input, Mat& resut) const;
        bool verifySizes(RotatedRect mr) const;
        Mat histeq(Mat in) const;
};

#endif