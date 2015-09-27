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
    while (it.hasNext())
    {
      _images.push_back(it.next());
    }
    runEngine(_currptr);
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
};