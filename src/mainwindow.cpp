#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "anprengine.h"
#include "DetectRegions.h"
#include <QFileDialog>
#include <QDirIterator>
#include "OCR.h"
#include <QDebug>
#include <opencv\ml.h>
#include <opencv\cvaux.h>
#include <opencv\highgui.h>
#include "utils.h"

namespace brANPR
{
  MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _engine(new ANPREngine())
  {
    ui->setupUi(this);
    //connect(m_button, SIGNAL(clicked()), this, SLOT(mySlot()));
    _currptr = 0;
  }

  MainWindow::~MainWindow()
  {
    delete ui;
  }

  void MainWindow::on_actionOpen_Folder_triggered()
  {
    _images.clear();
    _currptr = 0;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Folder"),
      "C:\\dev\\brANPR",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDirIterator it(dir, QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
      _images.push_back(it.next());
    }
    runEngine(_currptr);
  }

  void MainWindow::on_actionTrainPlates_triggered()
  {
    brANPR::trainPlates(6, 6, "C:\\dev\\brANPR\\train\\SVM\\plates\\", "C:\\dev\\brANPR\\train\\SVM\\noplates\\");
  }

  void MainWindow::on_actionTrainOCR_triggered()
  {
    OCR::train("C:\\dev\\brANPR\\train\\OCR\\");
  }

  void MainWindow::resizeEvent(QResizeEvent *event)
  {
    QMainWindow::resizeEvent(event);
    runEngine(_currptr);
  }

  void MainWindow::runEngine(int index) const
  {
    if (_images.size() > index)
    {
      DetectRegions detectRegions;
      string filename = _images[index].toUtf8().constData();
      detectRegions.setFilename(filename);
      detectRegions.showSteps = false;
      detectRegions.saveRegions = false;
      vector<Plate> posible_regions = detectRegions.run();

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
      OCR ocr("C:\\dev\\brANPR\\src\\OCR.xml");
      ocr.saveSegments = true;
      ocr.DEBUG = false;
      ocr.filename = brANPR::getFilename(filename);
      Mat input_image;
      input_image = imread(_images[index].toUtf8().constData(), 1);
      for (int i = 0; i < plates.size(); i++){
        Plate plate = plates[i];
        if (ocr.run(&plate))
        {
          string licensePlate = plate.str();
          cout << "================================================\n";
          cout << "License plate number: " << licensePlate << "\n";
          cout << "================================================\n";
          rectangle(input_image, plate.position, Scalar(0, 0, 200));
          putText(input_image, licensePlate, Point(plate.position.x, plate.position.y), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 200), 2);
          if (false){
            imshow("Plate Detected seg", plate.plateImg);
            cvWaitKey(0);
          }
        }
      }
      imshow("Plate Detected", input_image);
      
      /*
      _engine->run(_images[index].toUtf8().constData());
      ui->label_1->setPixmap(QPixmap::fromImage(_engine->original()).scaled(ui->label_1->size()));
      ui->label_2->setPixmap(QPixmap::fromImage(_engine->sobel()).scaled(ui->label_2->size()));
      ui->label_3->setPixmap(QPixmap::fromImage(_engine->processed1()).scaled(ui->label_3->size()));
      ui->label_4->setPixmap(QPixmap::fromImage(_engine->original()).scaled(ui->label_4->size()));
      ui->pictPath->setText(_images[index]);
      */
    }
  }

  void MainWindow::on_nextBtn_clicked()
  {
    _currptr = (_currptr + 1) % _images.size();
    runEngine(_currptr);
  }

  void MainWindow::on_prevBtn_clicked()
  {
    _currptr = (_currptr - 1) % _images.size();
    runEngine(_currptr);
  }
};