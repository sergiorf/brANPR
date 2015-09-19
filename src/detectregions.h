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
        void setFilename(string f);
        bool showSteps;
        vector<Plate> run();
    private:
        vector<Plate> segment(Mat input);
        bool verifySizes(RotatedRect mr);
        Mat histeq(Mat in);
};

#endif