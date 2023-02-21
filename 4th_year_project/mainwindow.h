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
#include <inference.h>
#include <detectiongraphic.h>

#define FRAME_PRINT
#define FRAME_PRINT_EVERY_MS 5000

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

    void print(const QString &text = "");
    void exportImage(const bool &verbose = true);
    QString getFormattedDate();

private slots:
    void on_centerZoomButton_clicked();
    void on_actionExport_Image_triggered();

    void on_exportButton_clicked();

    void on_actionViewText_triggered(bool checked);

    void on_actionViewOutline_triggered(bool checked);

    void on_actionViewInference_triggered(bool checked);

private:
    Ui::MainWindow *ui;
//    int _framerate = 12;
//    int _intervalMs = 1000/_framerate;
    int _frameCountTotal = 0;
    int _frameCount = 0;
    #ifdef CONNECTION_MANAGER
    QTimer _cameraConnectionManager;
    #endif
    #ifdef FRAME_PRINT
    QTimer _frameCountPrinter;
    #endif
    QTimer _frameUpdater;

    String _cameraPath = "/dev/video2";
    String _cameraWindowName = "Live";
    int _apiRef = cv::CAP_V4L;
    double _frameRate = 28, _frameIntervalMs = 1000.0/_frameRate;

    VideoCapture _vCap;
    Mat _frameMat;

    QPixmap _framePixmap;
    QGraphicsScene _frameScene;
    QSharedPointer<QGraphicsView> _outputGraphicsView;
    QMap<QString, int> *_detectionCountMap = new QMap<QString, int>;
    QList<Detection> _detections;
    QMap<QString, QList<Detection>> _detectionsByClass;

//    QMap<QString, QList<Detection>> *_detectionClassMap = new QMap<QString, QList<Detection>>;

    const QString _modelArchitecture = "YOLOv5";
    const int _infCols = 1280;
    const QString _infColsStr = QString::number(_infCols);
    const int _infRows = 704;
    const QString _infRowsStr = QString::number(_infRows);
    const QString _basePath = "/media/2TB_Crucial_SSD/repos/ai_identifier/4th_year_project";
    Inference _inf = Inference(_basePath + "/models/" + _modelArchitecture + "/" + _infColsStr + "x" + _infRowsStr + ".onnx", cv::Size(_infCols, _infRows),
                               _basePath + "/models/classes.txt", true);

    bool _inferenceEnabled = true, _drawText = true, _drawOutline = true;

    //Methods
    bool _test();
    void _onFrame();
    void _connectCamera();
    void _initCamera();
    void _printFramerate();
    void _runInference(const Mat &inputMat);
    void _appendInfoBox(const QString &text = "");
};
#endif // MAINWINDOW_H
