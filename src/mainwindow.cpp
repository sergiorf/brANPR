#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "anprengine.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow),
_engine(new ANPREngine())
{
  ui->setupUi(this);
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

void MainWindow::resizeEvent(QResizeEvent *event)
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
    ui->label_2->setPixmap(QPixmap::fromImage(_engine->processed1()).scaled(ui->label_2->size()));
    ui->label_3->setPixmap(QPixmap::fromImage(_engine->original()).scaled(ui->label_3->size()));
    ui->label_4->setPixmap(QPixmap::fromImage(_engine->original()).scaled(ui->label_4->size()));
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