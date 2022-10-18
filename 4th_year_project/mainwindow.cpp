#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connectCamera();
    _frameUpdater.setInterval(_intervalMs);
    #ifdef CONNECTION_MANAGER
    _cameraConnectionManager.setInterval(1000);
    if(!_vCap.isOpened())
    {
        qDebug() << "ERROR! Camera not found! Please connect the camera to continue.";
        _frameUpdater.stop();
        _cameraConnectionManager.start();
    }
    QObject::connect(&_cameraConnectionManager, &QTimer::timeout, this, &MainWindow::connectCamera);
    #endif

    _frameUpdater.start();
    QObject::connect(&_frameUpdater, &QTimer::timeout, this, &MainWindow::onFrame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::printDebug()
{
//    vector<VideoCaptureAPIs> cameraApis = videoio_registry::getBackends();
//    for(vector<VideoCaptureAPIs>::iterator iter = cameraApis.begin()+1 ; iter != cameraApis.end(); ++iter)
//    {
//        QString name = videoio_registry::getBackendName(*iter).c_str();
//        qDebug() << name;
//    }

    vector<VideoCaptureAPIs> backendApis = videoio_registry::getBackends();
    for(vector<VideoCaptureAPIs>::iterator iter = backendApis.begin()+1 ; iter != backendApis.end(); ++iter)
    {
        QString name = videoio_registry::getBackendName(*iter).c_str();
        qDebug() << name;
    }

    qDebug() << _frameUpdater.isActive() << _frameUpdater.interval() << _framerate << _intervalMs;

    return 0;
}

void MainWindow::onFrame()
{
    ++_frameCount;
    #ifdef FRAME_PRINT
    qDebug() << "INFO: Retrieving frame..." << _frameCount;
    #endif

    // wait for a new frame from camera and store it into 'frame'
    _vCap.read(_frame);
    // check if we succeeded
    if (_frame.empty())
    {
        qDebug() << "ERROR! Camera disconnected";
        _frameUpdater.stop();
        destroyWindow(_cameraWindowName);

        #ifdef CONNECTION_MANAGER
        _cameraConnectionManager.start();
        _vCap.release();
        #endif
        return;
    }
    // show live and wait for a key with timeout long enough to show images
    imshow(_cameraWindowName, _frame);
}

void MainWindow::connectCamera()
{
    #ifdef CONNECTION_PRINT
    qDebug() << "INFO: Attempting connection..." << _vCap.isOpened();
    #endif
    if(!_vCap.isOpened())
    {
        _vCap.open(_cameraPath, _apiRef);
        if(_vCap.isOpened())
        {
            qDebug() << "INFO: Camera connected";
            _frameUpdater.start();

            #ifdef CONNECTION_MANAGER
            _cameraConnectionManager.stop();
            #endif
        }
//        else
//            _frameUpdater.stop();
    }
}


