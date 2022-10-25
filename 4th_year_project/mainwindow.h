#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QGraphicsScene>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio/registry.hpp>

#include <iostream>
#include <stdio.h>
#include <util.h>

//#define FRAME_PRINT
#define CONNECTION_PRINT

#define CONNECTION_MANAGER

using namespace cv;
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void print(QString string);

private:
    Ui::MainWindow *ui;
    int _framerate = 24;
    int _intervalMs = 1000/_framerate;
    int _frameCount = 0;
    #ifdef CONNECTION_MANAGER
    QTimer _cameraConnectionManager;
    #endif
    QTimer _frameUpdater;

    String _cameraPath = "/dev/video2";
    String _cameraWindowName = "Live";
    int _apiRef = cv::CAP_V4L;

    VideoCapture _vCap;
    Mat _frameMat;
    QPixmap _framePixmap;
    QGraphicsScene _frameScene;

    bool _test();
    void _onFrame();
    void _connectCamera();
    void _initCamera();
};
#endif // MAINWINDOW_H
