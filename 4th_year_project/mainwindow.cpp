#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _connectCamera();
//    _frameUpdater.setInterval(_intervalMs);
    _frameUpdater.setSingleShot(true);
    _frameUpdater.start();
    #ifdef FRAME_PRINT
    _frameCountPrinter.setInterval(FRAME_PRINT_EVERY_MS);
    _frameCountPrinter.start();
    QObject::connect(&_frameCountPrinter, &QTimer::timeout, this, &MainWindow::_printFramerate);
    #endif
    #ifdef CONNECTION_MANAGER
    _cameraConnectionManager.setInterval(1000);

    _outputGraphicsView = QSharedPointer<QGraphicsView>(ui->outputGraphicView);
//    _outputGraphicsView->setMinimumSize(CAMERA_WIDTH, CAMERA_WIDTH);
//    _outputGraphicsView->setMaximumSize(CAMERA_WIDTH, CAMERA_WIDTH);
    _outputGraphicsView->setFrameShape(QFrame::NoFrame);
    _outputGraphicsView->setRenderHints(QPainter::Antialiasing | QPainter::LosslessImageRendering | QPainter::TextAntialiasing);

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

//    qDebug() << _frameUpdater.isActive() << _frameUpdater.interval() << _framerate << _intervalMs;

    return 0;
}

void MainWindow::_onFrame()
{
    ++_frameCountTotal;
    ++_frameCount;
    #ifdef FRAME_PRINT
    if(_frameCountTotal % 10 == 0)
        qDebug() << "INFO: Retrieving frame..." << _frameCountTotal;
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

    //Crop to 1:1
    _frameMat = Util::cropTo1By1Mid(_frameMat);
    const int matSize = _frameMat.cols;

    //Crop to specified zoom/pos values
    const double scale = 1 - (ui->zoomSlider->value()/SLIDER_RANGE);
    const double horizontalWidthPos = ui->horizontalZoomPosSlider->value()/SLIDER_RANGE;
    const double verticalWidthPos = 1.0-(ui->verticalZoomPosSlider->value()/SLIDER_RANGE);
    _frameMat = Util::cropShrink(_frameMat, scale, horizontalWidthPos, verticalWidthPos);

    //Convert to Pixmap
    _framePixmap = Util::matToPixmap(_frameMat);
    _frameScene.addPixmap(_framePixmap);

    //Set output
    _outputGraphicsView->fitInView(0, 0, matSize, matSize, Qt::KeepAspectRatio);
    _outputGraphicsView->setScene(&_frameScene);
    _outputGraphicsView->update();
    _outputGraphicsView->show();
    _frameUpdater.start();
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

void MainWindow::_printFramerate()
{
    _frameRate = (_frameCount / (double) FRAME_PRINT_EVERY_MS) * 1000.0;
    print(QString::number(_frameRate) + "fps");
    _frameCount = 0;
}

void MainWindow::on_centerZoomButton_clicked()
{
    ui->horizontalZoomPosSlider->setValue(SLIDER_RANGE*0.5);
    ui->verticalZoomPosSlider->setValue(SLIDER_RANGE*0.5);
}

void MainWindow::exportImage(QString name, bool verbose)
{
    if(name.length() == 0)
        name = Util::getFormattedDate() + ".png";
    imwrite(name.toStdString(), _frameMat);
    if(verbose)
        print(name + " exported");
}

void MainWindow::on_actionExport_Image_triggered()
{
    exportImage();
}

void MainWindow::on_exportButton_clicked()
{
    exportImage();
}

