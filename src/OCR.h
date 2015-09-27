#ifndef OCR_h
#define OCR_h

#include <string.h>
#include <vector>

#include "Plate.h"

#include <cv.h>
#include <highgui.h>
#include <cvaux.h>
#include <ml.h>

using namespace std;
using namespace cv;

#define HORIZONTAL    1
#define VERTICAL    0

namespace brANPR
{
  class CharSegment
  {
  public:
    CharSegment();
    CharSegment(Mat i, Rect p);
    Mat img;
    Rect pos;
  };

  struct OCRSettings
  {
    string trainFile;
    string trainingDataPath;
    string segmentsStore;
    bool saveSegments;
    bool showSteps;
  };

  class OCR
  {
  public:
    bool DEBUG;
    bool saveSegments;
    string filename;
    static const int numCharacters;
    static const char strCharacters[];
    OCR();
    OCR(const OCRSettings& settings);
    bool run(const OCRSettings& settings, Plate* input, Mat& composite);
    int charSize;
    int classify(Mat f);
    void train(Mat trainData, Mat trainClasses, int nlayers);
    int classifyKnn(Mat f);
    void trainKnn(Mat trainSamples, Mat trainClasses, int k);
    static void train(const OCRSettings& settings);
  private:
    bool trained;
    vector<CharSegment> segment(Plate input, Mat& composite) const;
    Mat preprocessChar(const Mat& in) const;
    Mat features(Mat input, int size) const;
    Mat Preprocess(Mat in, int newSize);
    Mat getVisualHistogram(Mat* hist, int type) const;
    void drawVisualFeatures(Mat character, Mat hhist, Mat vhist, Mat lowData) const;
    Mat ProjectedHistogram(Mat img, int t) const;
    bool verifySizes(const Mat& r) const;
    CvANN_MLP ann;
    CvKNearest knnClassifier;
    int K;
  };
};

#endif
