#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include "video.h"
#include <QTime>

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
    void on_buttonAddFile_clicked();

    void on_buttonLightsDetector_clicked();

    void on_buttonNegatives_clicked();

    void on_buttonTestSVM_clicked();

    void on_buttonColorDetector_clicked();

    void on_buttonAutoLights_clicked();

    void on_buttonAutoColor_clicked();

private:
    Ui::MainWindow *ui;
    Video *video;
};

#endif // MAINWINDOW_H
