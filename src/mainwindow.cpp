#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "anprengine.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QDebug>
#include <opencv\ml.h>
#include <opencv\cvaux.h>
#include <opencv\highgui.h>
#include "utils.h"
#include "OCR.h"
#include "settingsdialog.h"

namespace brANPR
{
  MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _engine(new ANPREngine())
  {
    ui->setupUi(this);
    ui->prevBtn->setEnabled(false);
    ui->nextBtn->setEnabled(false);
    _currptr = 0;
  }

  MainWindow::~MainWindow()
  {
    delete ui;
  }

  void MainWindow::on_actionOpen_File_triggered()
  {
    reset();
    _images.clear();
    _currptr = 0;
    QFileDialog dlg(this, tr("Select Image(s)"), "C:\\dev\\brANPR");
    dlg.setNameFilter(tr("Images (*.jpg)"));
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setFileMode(QFileDialog::ExistingFiles);
    QStringList fileNames;
    if (dlg.exec())
      fileNames = dlg.selectedFiles();
    foreach(const QString &str, fileNames)
    {
      qDebug() << QString(" [%1] ").arg(str);
      _images.push_back(str);
    }
    if (_images.size() > 1)
    {
      ui->prevBtn->setEnabled(true);
      ui->nextBtn->setEnabled(true);
      runEngine(_currptr);
    }
  }

  void MainWindow::on_actionOpen_Folder_triggered()
  {
    reset();
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Select Folder"),
      "C:\\dev\\brANPR",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDir dir(dirName);
    dir.setNameFilters(QStringList("*.jpg"));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    qDebug() << "Scanning: " << dir.path();
    QFileInfoList fileInfoList = dir.entryInfoList();
    for (int i = 0; i < fileInfoList.count(); i++)
    {
      QFileInfo fin(fileInfoList[i]);
      qDebug() << "Found file: " << fin.canonicalFilePath();
      _images.push_back(fin.canonicalFilePath());
    }
    if (_images.size() > 1)
    {
      ui->prevBtn->setEnabled(true);
      ui->nextBtn->setEnabled(true);
      runEngine(_currptr);
    }
  }

  void MainWindow::on_actionTrainPlates_triggered()
  {
    brANPR::trainPlates(8, 9, "C:\\dev\\brANPR\\train\\SVM\\plates\\", "C:\\dev\\brANPR\\train\\SVM\\noplates\\");
  }

  void MainWindow::on_actionTrainOCR_triggered()
  {
    OCR::train(_engine->getSettings().OCR);
  }

  void MainWindow::on_actionSettings_triggered()
  {
    SettingsDialog dlg(_engine->getSettings());
    if (dlg.exec() == QDialog::Accepted)
      _engine->setSettings(dlg.getSettings());
  }

  void MainWindow::resizeEvent(QResizeEvent* event)
  {
    QMainWindow::resizeEvent(event);
    runEngine(_currptr);
  }

  void MainWindow::runEngine(int index) const
  {
    if (_images.size() > index)
    {
      _engine->run(_images[index].toUtf8().constData());
      ui->label_1->setPixmap(QPixmap::fromImage(_engine->original()).scaled(ui->label_1->size()));
      ui->label_2->setPixmap(QPixmap::fromImage(_engine->segmented()).scaled(ui->label_2->size()));
      ui->label_3->setPixmap(QPixmap::fromImage(_engine->processed_plates()).scaled(ui->label_3->size()));
      ui->label_4->setPixmap(QPixmap::fromImage(_engine->plate_detected()).scaled(ui->label_4->size()));
      ui->pictPath->setText(_images[index]);
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

  void MainWindow::reset()
  {
    ui->prevBtn->setEnabled(false);
    ui->nextBtn->setEnabled(false);
    _images.clear();
    _currptr = 0;
    _engine->reset();
  }
};