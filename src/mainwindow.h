#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_Folder_triggered();
    void on_nextBtn_clicked();
    void on_prevBtn_clicked();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    void runEngine(int index);
    Ui::MainWindow *ui;
    QVector<QString> _images;
    int _currptr;
};

#endif // MAINWINDOW_H
