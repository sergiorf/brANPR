#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>
#include <memory>

namespace Ui
{
  class MainWindow;
}

namespace brANPR
{
  class ANPREngine;

  class MainWindow : public QMainWindow
  {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private slots:
    void on_actionOpen_Folder_triggered();
    void on_actionTrainPlates_triggered();
    void on_actionTrainOCR_triggered();
    void on_actionSettings_triggered();
    void on_nextBtn_clicked();
    void on_prevBtn_clicked();

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void runEngine(int index) const;
    Ui::MainWindow* ui;
    std::vector<QString> _images;
    int _currptr;
    std::unique_ptr<ANPREngine> _engine;
  };
};
#endif // MAINWINDOW_H
