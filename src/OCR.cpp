#include "OCR.h"

const char OCR::strCharacters[] = {'0','1','2','3','4','5','6','7','8','9','B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'R', 'S', 'T', 'V', 'W', 'X', 'Y', 'Z'};
const int OCR::numCharacters=30;

CharSegment::CharSegment(){}
CharSegment::CharSegment(Mat i, Rect p){
    img=i;
    pos=p;
}

OCR::OCR(){
    DEBUG=false;
    trained=false;
    saveSegments=false;
    charSize=20;
}
OCR::OCR(string trainFile){
    DEBUG=false;
    trained=false;
    saveSegments=false;
    charSize=20;

    //Read file storage.
    FileStorage fs;
    fs.open(trainFile, FileStorage::READ);
    Mat TrainingData;
    Mat Classes;
    fs["TrainingDataF15"] >> TrainingData;
    fs["classes"] >> Classes;

    train(TrainingData, Classes, 10);

}

Mat OCR::preprocessChar(Mat in){
    //Remap image
    int h=in.rows;
    int w=in.cols;
    Mat transformMat=Mat::eye(2,3,CV_32F);
    int m=max(w,h);
    transformMat.at<float>(0,2)=m/2 - w/2;
    transformMat.at<float>(1,2)=m/2 - h/2;

    Mat warpImage(m,m, in.type());
    warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0) );

    Mat out;
    resize(warpImage, out, Size(charSize, charSize) ); 

    return out;
}

bool OCR::verifySizes(Mat r){
    //Char sizes 46x65
    float aspect=46.0f/65.0f;
    float charAspect= (float)r.cols/(float)r.rows;
    float error=0.35;
    float minHeight=15;
    float maxHeight=28;
    //We have a different aspect ratio for number 1, and it can be ~0.2
    float minAspect=0.2;
    float maxAspect=aspect+aspect*error;
    //area of pixels
    float area=countNonZero(r);
    //bb area
    float bbArea=r.cols*r.rows;
    //% of pixel in area
    float percPixels=area/bbArea;
    if(DEBUG)
        cout << "Aspect: "<< aspect << " ["<< minAspect << "," << maxAspect << "] "  << "Area "<< percPixels <<" Char aspect " << charAspect  << " Height char "<< r.rows << "\n";
    if(percPixels < 0.85 && charAspect > minAspect && charAspect < maxAspect && r.rows >= minHeight && r.rows < maxHeight)
        return true;
    else
        return false;
}

vector<CharSegment> OCR::segment(Plate plate){
    Mat input=plate.plateImg;
    imshow("segmenting...", input);

    Mat element = getStructuringElement(MORPH_RECT, Size(17, 5));
    //  Mat element = getStructuringElement(MORPH_RECT, Size(17, 3));

    vector<CharSegment> output;
    //Threshold input image
    Mat img_threshold;
    //threshold(input, img_threshold, 60, 255, CV_THRESH_BINARY_INV);
    threshold(input, img_threshold, 150, 255, CV_THRESH_BINARY);
    if(DEBUG)
        imshow("Threshold plate", img_threshold);
    Mat img_contours;
    img_threshold.copyTo(img_contours);
    //Find contours of possibles characters
    vector< vector< Point> > contours;
    findContours(img_contours,
            contours, // a vector of contours
            CV_RETR_EXTERNAL, // retrieve the external contours
            CV_CHAIN_APPROX_NONE); // all pixels of each contours
    
    // Draw blue contours on a white image
    cv::Mat result;
    img_threshold.copyTo(result);
    cvtColor(result, result, CV_GRAY2RGB);
    cv::drawContours(result,contours,
            -1, // draw all contours
            cv::Scalar(255,0,0), // in blue
            1); // with a thickness of 1

    //Start to iterate to each contour founded
    vector<vector<Point> >::iterator itc= contours.begin();
    
    //Remove patch that are no inside limits of aspect ratio and area.    
    int i = 0;
    while (itc!=contours.end()) {        
        //Create bounding rect of object
        Rect mr= boundingRect(Mat(*itc));
        rectangle(result, mr, Scalar(0,255,0));
        //Crop image
        Mat auxRoi(img_threshold, mr);
        /*
        {
          stringstream ss;
          ss << "auxRoi[" << i++ << "] ";
          imshow(ss.str().c_str(), auxRoi);
        }
        */
        if(verifySizes(auxRoi)){
            auxRoi=preprocessChar(auxRoi);
            output.push_back(CharSegment(auxRoi, mr));
            rectangle(result, mr, Scalar(0,125,255));
        }
        ++itc;
    }
    if(DEBUG)
        cout << "Num chars: " << output.size() << "\n";
    if(DEBUG)
        imshow("SEgmented Chars", result);
    return output;
}

Mat OCR::ProjectedHistogram(Mat img, int t)
{
    int sz=(t)?img.rows:img.cols;
    Mat mhist=Mat::zeros(1,sz,CV_32F);

    for(int j=0; j<sz; j++){
        Mat data=(t)?img.row(j):img.col(j);
        mhist.at<float>(j)=countNonZero(data);
    }

    //Normalize histogram
    double min, max;
    minMaxLoc(mhist, &min, &max);
    
    if(max>0)
        mhist.convertTo(mhist,-1 , 1.0f/max, 0);

    return mhist;
}

Mat OCR::getVisualHistogram(Mat *hist, int type)
{

    int size=100;
    Mat imHist;


    if(type==HORIZONTAL){
        imHist.create(Size(size,hist->cols), CV_8UC3);
    }else{
        imHist.create(Size(hist->cols, size), CV_8UC3);
    }

    imHist=Scalar(55,55,55);

    for(int i=0;i<hist->cols;i++){
        float value=hist->at<float>(i);
        int maxval=(int)(value*size);

        Point pt1;
        Point pt2, pt3, pt4;

        if(type==HORIZONTAL){
            pt1.x=pt3.x=0;
            pt2.x=pt4.x=maxval;
            pt1.y=pt2.y=i;
            pt3.y=pt4.y=i+1;

            line(imHist, pt1, pt2, CV_RGB(220,220,220),1,8,0);
            line(imHist, pt3, pt4, CV_RGB(34,34,34),1,8,0);

            pt3.y=pt4.y=i+2;
            line(imHist, pt3, pt4, CV_RGB(44,44,44),1,8,0);
            pt3.y=pt4.y=i+3;
            line(imHist, pt3, pt4, CV_RGB(50,50,50),1,8,0);
        }else{

                        pt1.x=pt2.x=i;
                        pt3.x=pt4.x=i+1;
                        pt1.y=pt3.y=100;
                        pt2.y=pt4.y=100-maxval;


            line(imHist, pt1, pt2, CV_RGB(220,220,220),1,8,0);
            line(imHist, pt3, pt4, CV_RGB(34,34,34),1,8,0);

            pt3.x=pt4.x=i+2;
            line(imHist, pt3, pt4, CV_RGB(44,44,44),1,8,0);
            pt3.x=pt4.x=i+3;
            line(imHist, pt3, pt4, CV_RGB(50,50,50),1,8,0);

        }




    }

    return imHist ;
}

void OCR::drawVisualFeatures(Mat character, Mat hhist, Mat vhist, Mat lowData){
    Mat img(121, 121, CV_8UC3, Scalar(0,0,0));
    Mat ch;
    Mat ld;
    
    cvtColor(character, ch, CV_GRAY2RGB);

    resize(lowData, ld, Size(100, 100), 0, 0, INTER_NEAREST );
    cvtColor(ld,ld,CV_GRAY2RGB);

    Mat hh=getVisualHistogram(&hhist, HORIZONTAL);
    Mat hv=getVisualHistogram(&vhist, VERTICAL);

    Mat subImg=img(Rect(0,101,20,20));
    ch.copyTo(subImg);

    subImg=img(Rect(21,101,100,20));
    hh.copyTo(subImg);

    subImg=img(Rect(0,0,20,100));
    hv.copyTo(subImg);

    subImg=img(Rect(21,0,100,100));
    ld.copyTo(subImg);

    line(img, Point(0,100), Point(121,100), Scalar(0,0,255));
    line(img, Point(20,0), Point(20,121), Scalar(0,0,255));

    imshow("Visual Features", img);

    cvWaitKey(0);
}

Mat OCR::features(Mat in, int sizeData){
    //Histogram features
    Mat vhist=ProjectedHistogram(in,VERTICAL);
    Mat hhist=ProjectedHistogram(in,HORIZONTAL);
    
    //Low data feature
    Mat lowData;
    resize(in, lowData, Size(sizeData, sizeData) );

    if(DEBUG)
        drawVisualFeatures(in, hhist, vhist, lowData);
    

    
    //Last 10 is the number of moments components
    int numCols=vhist.cols+hhist.cols+lowData.cols*lowData.cols;
    
    Mat out=Mat::zeros(1,numCols,CV_32F);
    //Asign values to feature
    int j=0;
    for(int i=0; i<vhist.cols; i++)
    {
        out.at<float>(j)=vhist.at<float>(i);
        j++;
    }
    for(int i=0; i<hhist.cols; i++)
    {
        out.at<float>(j)=hhist.at<float>(i);
        j++;
    }
    for(int x=0; x<lowData.cols; x++)
    {
        for(int y=0; y<lowData.rows; y++){
            out.at<float>(j)=(float)lowData.at<unsigned char>(x,y);
            j++;
        }
    }
    if(DEBUG)
        cout << out << "\n===========================================\n";
    return out;
}

void OCR::train(Mat TrainData, Mat classes, int nlayers){
    Mat layers(1,3,CV_32SC1);
    layers.at<int>(0)= TrainData.cols;
    layers.at<int>(1)= nlayers;
    layers.at<int>(2)= numCharacters;
    ann.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);

    //Prepare trainClases
    //Create a mat with n trained data by m classes
    Mat trainClasses;
    trainClasses.create( TrainData.rows, numCharacters, CV_32FC1 );
    for( int i = 0; i <  trainClasses.rows; i++ )
    {
        for( int k = 0; k < trainClasses.cols; k++ )
        {
            //If class of data i is same than a k class
            if( k == classes.at<int>(i) )
                trainClasses.at<float>(i,k) = 1;
            else
                trainClasses.at<float>(i,k) = 0;
        }
    }
    Mat weights( 1, TrainData.rows, CV_32FC1, Scalar::all(1) );
    
    //Learn classifier
    ann.train( TrainData, trainClasses, weights );
    trained=true;
}

int OCR::classify(Mat f){
    int result=-1;
    Mat output(1, numCharacters, CV_32FC1);
    ann.predict(f, output);
    Point maxLoc;
    double maxVal;
    minMaxLoc(output, 0, &maxVal, 0, &maxLoc);
    //We need know where in output is the max val, the x (cols) is the class.

    return maxLoc.x;
}

int OCR::classifyKnn(Mat f){
    int response = (int)knnClassifier.find_nearest( f, K );
    return response;
}
void OCR::trainKnn(Mat trainSamples, Mat trainClasses, int k){
    K=k;
    // learn classifier
    knnClassifier.train( trainSamples, trainClasses, Mat(), false, K );
}

bool OCR::run(Plate *input){
    
    //Segment chars of plate
    vector<CharSegment> segments=segment(*input);

    for(int i=0; i<segments.size(); i++){
        //Preprocess each char for all images have same sizes
        Mat ch=preprocessChar(segments[i].img);
        if(saveSegments){
            stringstream ss(stringstream::in | stringstream::out);
            ss << "c:\\tmp\\" << filename << "_" << i << ".jpg";
            imwrite(ss.str(),ch);
        }
        /*
        {
        stringstream ss;
        ss << "preprocesschar[" << i << "] ";
        imshow(ss.str().c_str(), ch);
        }
        */
        //For each segment Extract Features
        Mat f=features(ch,15);
        //For each segment feature Classify
        int character=classify(f);
        input->chars.push_back(strCharacters[character]);
        input->charsPos.push_back(segments[i].pos);
    }
    return segments.size()>0;
}

//const char OCR::strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'R', 'S', 'T', 'V', 'W', 'X', 'Y', 'Z' };
const int numFilesChars[] =         {  3,   0,   3,   8,   4,   5,   1,   3,   7,   6,   9,   0,   3,   0,   1,   1,   0,   1,   0,   2,   0,   2,   0,   1,   0,   2,   1,   0,   4,   0 };
//const int numFilesChars[] = { 35, 40, 42, 41, 42, 33, 30, 31, 49, 44, 30, 24, 21, 20, 34, 9, 10, 3, 11, 3, 15, 4, 9, 12, 10, 21, 18, 8, 15, 7 };

void OCR::train(const string& path)
{
  Mat classes;
  Mat trainingDataf5;
  Mat trainingDataf10;
  Mat trainingDataf15;
  Mat trainingDataf20;

  vector<int> trainingLabels;
  OCR ocr;

  for (int i = 0; i < OCR::numCharacters; i++)
  {
    int numFiles = numFilesChars[i];
    for (int j = 0; j < numFiles; j++){
      cout << "Character " << OCR::strCharacters[i] << " file: " << j << "\n";
      stringstream ss(stringstream::in | stringstream::out);
      ss << path << OCR::strCharacters[i] << "/" << j << ".jpg";
      Mat img = imread(ss.str(), 0);
      Mat f5 = ocr.features(img, 5);
      Mat f10 = ocr.features(img, 10);
      Mat f15 = ocr.features(img, 15);
      Mat f20 = ocr.features(img, 20);

      trainingDataf5.push_back(f5);
      trainingDataf10.push_back(f10);
      trainingDataf15.push_back(f15);
      trainingDataf20.push_back(f20);
      trainingLabels.push_back(i);
    }
  }
  trainingDataf5.convertTo(trainingDataf5, CV_32FC1);
  trainingDataf10.convertTo(trainingDataf10, CV_32FC1);
  trainingDataf15.convertTo(trainingDataf15, CV_32FC1);
  trainingDataf20.convertTo(trainingDataf20, CV_32FC1);
  Mat(trainingLabels).copyTo(classes);

  FileStorage fs("C:\\dev\\brANPR\\src\\OCR.xml", FileStorage::WRITE);
  fs << "TrainingDataF5" << trainingDataf5;
  fs << "TrainingDataF10" << trainingDataf10;
  fs << "TrainingDataF15" << trainingDataf15;
  fs << "TrainingDataF20" << trainingDataf20;
  fs << "classes" << classes;
  fs.release();
}

