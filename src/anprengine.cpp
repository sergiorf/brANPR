#include "anprengine.h"
#include <qcolor.h>
#include <memory>
#include <iostream>
#include "utils.h"
#include <time.h>
#include <random>
#include "DetectRegions.h"
#include "OCR.h"

using namespace std;
using namespace cv;

#define YELLOW CV_RGB(255, 255, 0)
#define GREEN  CV_RGB(0, 255, 0)
#define BLUE   CV_RGB(0, 0, 255)
#define RED    CV_RGB(255, 0, 0)

namespace brANPR
{
  ANPREngine::ANPREngine()
  {
    _Settings = {
      {
        "C:\\dev\\brANPR\\src\\OCR.xml",
        "C:\\dev\\brANPR\\train\\OCR\\",
        "c:\\tmp\\",
        false,
        false,
      } };
  }

  void ANPREngine::run(const string& filename)
  {
    reset();
    _Original = mat2QImage(imread(filename));
    DetectRegions detectRegions;
    detectRegions.setFilename(filename);
    detectRegions.showSteps = false;
    detectRegions.saveRegions = false;
    Mat segmented;
    vector<Plate> posible_regions = detectRegions.run(segmented);
    _Segmented = mat2QImage(segmented);

    //SVM for each plate region to get valid car plates
    //Read file storage.
    FileStorage fs;
    fs.open("C:\\dev\\brANPR\\src\\SVM.xml", FileStorage::READ);
    Mat SVM_TrainingData;
    Mat SVM_Classes;
    fs["TrainingData"] >> SVM_TrainingData;
    fs["classes"] >> SVM_Classes;
    //Set SVM params
    CvSVMParams SVM_params;
    SVM_params.svm_type = CvSVM::C_SVC;
    SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;
    SVM_params.degree = 0;
    SVM_params.gamma = 1;
    SVM_params.coef0 = 0;
    SVM_params.C = 1;
    SVM_params.nu = 0;
    SVM_params.p = 0;
    SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01);
    //Train SVM
    CvSVM svmClassifier(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params);

    //For each possible plate, classify with svm if it's a plate or no
    vector<Plate> plates;
    for (int i = 0; i < posible_regions.size(); i++)
    {
      Mat img = posible_regions[i].plateImg;
      Mat p = img.reshape(1, 1);
      p.convertTo(p, CV_32FC1);

      int response = (int)svmClassifier.predict(p);
      if (response == 1)
        plates.push_back(posible_regions[i]);
    }

    cout << "Num plates detected: " << plates.size() << "\n";

    //For each plate detected, recognize it with OCR
    OCR ocr(_Settings.OCR);
    ocr.filename = getFilename(filename);
    Mat input_image;
    input_image = imread(filename, 1);
    vector<Mat> proc_plates;
    for (int i = 0; i < plates.size(); i++)
    {
      Plate plate = plates[i];
      Mat composite;
      if (ocr.run(_Settings.OCR, &plate, composite))
      {
        string licensePlate = applyCountryRules(plate.str());
        cout << "================================================\n";
        cout << "License plate number: " << licensePlate << "\n";
        cout << "================================================\n";
        rectangle(input_image, plate.position, Scalar(0, 0, 200));
        putText(input_image, licensePlate, Point(plate.position.x, plate.position.y),
          CV_FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 0, 0), 2);
      }
      proc_plates.push_back(composite);
      Mat tmp;
      hconcat(proc_plates, tmp);
      _ProcessedPlates = mat2QImage(tmp);
    }
    _PlateDetected = mat2QImage(input_image);
    // imshow("Plate Detected", input_image);
  }

  QImage ANPREngine::original() const
  {
    return _Original;
  }

  QImage ANPREngine::processed_plates() const
  {
    return _ProcessedPlates;
  }

  QImage ANPREngine::segmented() const
  {
    return _Segmented;
  }

  QImage ANPREngine::plate_detected() const
  {
    return _PlateDetected;
  }

  QImage ANPREngine::mat2QImage(const cv::Mat& inMat) const
  {
    switch (inMat.type())
    {
    case CV_8UC4:
    {
                  QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32);
                  image.bits();
                  return image;
    }
    case CV_8UC3:
    {
                  QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888);
                  image.bits();
                  return image.rgbSwapped();
    }
    case CV_8UC1:
    {
                  cv::Mat rgb;
                  cv::cvtColor(inMat, rgb, CV_GRAY2BGR);
                  cv::cvtColor(rgb, rgb, CV_BGR2BGRA);
                  auto temp = QImage(static_cast<unsigned char*>(rgb.data), rgb.cols, rgb.rows, QImage::Format_ARGB32);
                  auto image = temp.copy();
                  return image;
    }
    default:
      cerr << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
      break;
    }
    return QImage();
  }

  void ANPREngine::setSettings(const ANPRSettings& settings)
  {
    _Settings = settings;
  }

  brANPR::ANPRSettings ANPREngine::getSettings() const
  {
    return _Settings;
  }

  void ANPREngine::reset()
  {
    _Original = QImage();
    _Segmented = QImage();
    _ProcessedPlates = QImage();
    _PlateDetected = QImage();
  }
}