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

  void inflate(Rect& rect, double perc)
  {
    double h = (double)rect.height * (1 + perc);
    double w = (double)rect.width* (1 + perc);
    double x = (double)rect.x - (w - (double)rect.width) / 2;
    double y = (double)rect.y - (h - (double)rect.height) / 2;
    rect = Rect((int)x, (int)y, (int)w, (int)h);
  }

  string getFilename(const string& s) {

    char sep = '/';
    char sepExt = '.';
    size_t i = s.rfind("/", s.length());
    if (i != string::npos) {
      string fn = (s.substr(i + 1, s.length() - i));
      size_t j = fn.rfind(sepExt, fn.length());
      if (i != string::npos) {
        return fn.substr(0, j);
      }
      else{
        return fn;
      }
    }
    else{
      return "";
    }
  }

    void trainPlates(int numPlates, int numNoPlates, const string& path_Plates, const string& path_NoPlates)
    {
      Mat classes;//(numPlates+numNoPlates, 1, CV_32FC1);
      Mat trainingData;//(numPlates+numNoPlates, imageWidth*imageHeight, CV_32FC1 );

      Mat trainingImages;
      vector<int> trainingLabels;

      for (int i = 0; i < numPlates; i++)
      {

        stringstream ss(stringstream::in | stringstream::out);
        ss << path_Plates << i << ".jpg";
        Mat img = imread(ss.str(), 0);
        img = img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(1);
      }

      for (int i = 0; i < numNoPlates; i++)
      {
        stringstream ss(stringstream::in | stringstream::out);
        ss << path_NoPlates << i << ".jpg";
        Mat img = imread(ss.str(), 0);
        img = img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(0);

      }

      Mat(trainingImages).copyTo(trainingData);
      //trainingData = trainingData.reshape(1,trainingData.rows);
      trainingData.convertTo(trainingData, CV_32FC1);
      Mat(trainingLabels).copyTo(classes);

      FileStorage fs("C:\\dev\\brANPR\\src\\SVM.xml", FileStorage::WRITE);
      fs << "TrainingData" << trainingData;
      fs << "classes" << classes;
      fs.release();
    }

    string applyCountryRules(const string& licensePlate)
    {
      stringstream ss;
      int i = 0;
      if (licensePlate.size()!=7)
        return licensePlate;
      for (const char& c : licensePlate)
      {
        if (i < 3)
        {
          switch (c)
          {
              case '5': ss << 'S'; break;
              case '0': ss << 'O'; break;
              case '8': ss << 'B'; break;
              default: ss << c; break;
          }
        }
        else
        {
          switch (c)
          {
          case 'S': ss << '5'; break;
          case 'O': ss << '0'; break;
          case 'D': ss << '0'; break;
          case 'B': ss << '8'; break;
          default: ss << c; break;
          }
        }
        i++;
      }
      return ss.str();
    }
};