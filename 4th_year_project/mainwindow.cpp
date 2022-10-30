#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _connectCamera();
    _frameUpdater.setInterval(_intervalMs);
    #ifdef CONNECTION_MANAGER
    _cameraConnectionManager.setInterval(1000);

    QGraphicsView outputGraphicsView = ui->outputGraphicView;
    outputGraphicsView.setMinimumSize(CAMERA_WIDTH, CAMERA_WIDTH);
    outputGraphicsView.setMaximumSize(CAMERA_WIDTH, CAMERA_WIDTH);
    if(!_vCap.isOpened())
    {
        print("ERROR! Camera not found! Please connect the camera to continue.");
        _frameUpdater.stop();
        _cameraConnectionManager.start();
    }
    QObject::connect(&_cameraConnectionManager, &QTimer::timeout, this, &MainWindow::_connectCamera);
    #endif

    _frameUpdater.start();
    QObject::connect(&_frameUpdater, &QTimer::timeout, this, &MainWindow::_onFrame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::print(QString string)
{
    ui->outputTextBox->moveCursor(QTextCursor::End);
    ui->outputTextBox->insertPlainText(string + "\n");
    ui->outputTextBox->moveCursor(QTextCursor::End);
    qDebug() << string;
}

bool MainWindow::_test()
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

void MainWindow::_onFrame()
{
    ++_frameCount;
    #ifdef FRAME_PRINT
    qDebug() << "INFO: Retrieving frame..." << _frameCount;
    #endif

    // wait for a new frame from camera and store it into 'frame'
    _vCap.read(_frameMat);
    // check if we succeeded
    if (_frameMat.empty())
    {
        print("ERROR! Camera disconnected");
        _frameUpdater.stop();
        destroyWindow(_cameraWindowName);

        #ifdef CONNECTION_MANAGER
        _cameraConnectionManager.start();
        _vCap.release();
        #endif
        return;
    }
    // show live and wait for a key with timeout long enough to show images
//    imshow(_cameraWindowName, _frameMat);
//    _frameMat = Mat(404, 404, CV_8UC4);
    const double scale = 1 - (ui->zoomSlider->value()/8192.0);

    //Crop to 1:1
    _frameMat = Util::cropTo1By1Mid(_frameMat);
    //Shrink to zoom slider value
    _frameMat = Util::cropShrink(_frameMat, scale);

    _framePixmap = Util::matToPixmap(_frameMat);
    _frameScene.addPixmap(_framePixmap);

    QGraphicsView *outputGraphicView = ui->outputGraphicView;
    outputGraphicView->setScene(&_frameScene);
    outputGraphicView->update();
    outputGraphicView->show();
}

void MainWindow::_connectCamera()
{
    #ifdef CONNECTION_PRINT
    print("INFO: Attempting connection... "/* + QString((_vCap.isOpened() ? "open" : "closed"))*/);
    #endif
    if(!_vCap.isOpened())
    {
        _vCap.open(_cameraPath, _apiRef);
        if(_vCap.isOpened())
        {
            _initCamera();
            print("INFO: Camera connected");
            _frameUpdater.start();

            #ifdef CONNECTION_MANAGER
            _cameraConnectionManager.stop();
            #endif
        }
        else
            _frameUpdater.stop();
    }
}


void MainWindow::_initCamera()
{
    _vCap.set(cv::CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
    _vCap.set(cv::CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
}
