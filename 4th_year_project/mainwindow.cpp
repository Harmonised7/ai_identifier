#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _connectCamera();
    _frameUpdater.setInterval(_frameIntervalMs);
    qDebug() << _frameIntervalMs;
//    _frameUpdater.setSingleShot(true);
    #ifdef FRAME_PRINT
    _frameCountPrinter.setInterval(FRAME_PRINT_EVERY_MS);
    QObject::connect(&_frameCountPrinter, &QTimer::timeout, this, &MainWindow::_printFramerate);
    #endif
    #ifdef CONNECTION_MANAGER
    _cameraConnectionManager.setInterval(1000);

    _outputGraphicsView = QSharedPointer<QGraphicsView>(ui->outputGraphicView);
    _outputGraphicsView->minimumSize().setWidth(CAMERA_WIDTH);
    _outputGraphicsView->minimumSize().setHeight(CAMERA_HEIGHT);
    _outputGraphicsView->size().setWidth(CAMERA_WIDTH);
    _outputGraphicsView->size().setHeight(CAMERA_HEIGHT);
    _outputGraphicsView->setFrameShape(QFrame::NoFrame);
    _outputGraphicsView->setRenderHints(QPainter::Antialiasing | QPainter::LosslessImageRendering | QPainter::TextAntialiasing);

    if(!_vCap.isOpened())
    {
        print("ERROR! Camera not found! Please connect the camera to continue.");
        _frameUpdater.stop();

        #ifdef FRAME_PRINT
        _frameCountPrinter.stop();
        #endif

        _cameraConnectionManager.start();

    }
    QObject::connect(&_cameraConnectionManager, &QTimer::timeout, this, &MainWindow::_connectCamera);
    #endif

    QObject::connect(&_frameUpdater, &QTimer::timeout, this, &MainWindow::_onFrame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::print(const QString &text)
{
    Util::appendQTextEditLine(ui->consoleTextBox, text);
    qDebug() << text;
}

void MainWindow::_appendInfoBox(const QString &text)
{
    Util::appendQTextEditLine(ui->infoTextBox, text);
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

    _detections.clear();
    _detectionsByClass.clear();
    _frameScene.clear();
//    _frameMat = Util::cropTo1By1Mid(_frameMat);

    //FPS Count
    ++_frameCountTotal;
    ++_frameCount;

    #ifdef FRAME_PRINT
    if(_frameCountTotal % 10 == 0)
        qDebug() << "INFO: Retrieving frame..." << _frameCountTotal;
    #endif

    //Clear info box
    ui->infoTextBox->clear();

    if(_inferenceEnabled)
    {
        //Run Inference
//        _frameMat = Util::cropTo1By1Mid(_frameMat);
        _runInference(_frameMat);
    }

    const int matSize = _frameMat.cols;

    //Crop to specified zoom/pos values
    const double scale = 1 - (ui->zoomSlider->value()/SLIDER_RANGE);
    const double horizontalWidthPos = ui->horizontalZoomPosSlider->value()/SLIDER_RANGE;
    const double verticalWidthPos = 1.0-(ui->verticalZoomPosSlider->value()/SLIDER_RANGE);
    _frameMat = Util::cropShrink(_frameMat, scale, horizontalWidthPos, verticalWidthPos);

    //Convert to Pixmap
    _framePixmap = Util::matToPixmap(_frameMat);
    _frameScene.addPixmap(_framePixmap);

    //Add detections to the Scene
    const int detectionCount = _detections.length();
    for(int i = 0; i < detectionCount; ++i)
    {
        _frameScene.addItem(new DetectionGraphic(_detections[i]));
    }

    //Set output
    _outputGraphicsView->fitInView(0, 0, matSize, matSize, Qt::KeepAspectRatio);
    _outputGraphicsView->setScene(&_frameScene);
    _outputGraphicsView->update();
    _outputGraphicsView->show();
//    _frameUpdater.start();
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

            #ifdef FRAME_PRINT
            _frameCountPrinter.start();
            _frameCount = 0;
            #endif

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
    _vCap.set(cv::CAP_PROP_FPS, 24);
}

void MainWindow::_printFramerate()
{
    _frameRate = (_frameCount / (double) FRAME_PRINT_EVERY_MS) * 1000.0;
    print(QString::number(_frameRate) + "fps");
    _frameCount = 0;
}

void MainWindow::_runInference(const Mat &inputMat)
{
    _detectionCountMap->clear();
    _detections = _inf.runInference(inputMat);


    const int detectionsCount = _detections.size();
    _appendInfoBox("Detection count: " + QString::number(detectionsCount) + "\n");

    for(auto detection : _detections)
    {
        //Retrieve each bounding box as a Mat
        detection.mat = Mat(_frameMat, detection.box);
        //Categorise each detection by class name
        if(!_detectionsByClass.contains(detection.className))
           _detectionsByClass.insert(detection.className, QList<Detection>());
        _detectionsByClass[detection.className].push_back(detection);
    }

    for(const QString &key : _detectionsByClass.keys())
    {
        _appendInfoBox(key + ": " + QString::number(_detectionsByClass.value(key).length()));
    }

    _appendInfoBox();

    for(const Detection &detection : _detections)
    {
        _appendInfoBox(QString::number(detection.confidence).mid(0, 4).append(" ").append(detection.className));
    }
}

void MainWindow::on_centerZoomButton_clicked()
{
    ui->horizontalZoomPosSlider->setValue(SLIDER_RANGE*0.5);
    ui->verticalZoomPosSlider->setValue(SLIDER_RANGE*0.5);
}

void MainWindow::exportImage(const bool &verbose)
{
    const QString date = Util::getFormattedDate();
    const QString dirPath = "./export/" + date;
    QDir().mkpath(dirPath);
    Util::exportSceneToFile(_frameScene, dirPath + "/" + "final");
    cv::imwrite(QString(dirPath + "/" + "raw.png").toStdString(), _frameMat);
    if(verbose)
        print(dirPath + " exported");

    for(auto className : _detectionsByClass.keys())
    {
        const QList<Detection> detections = _detectionsByClass[className];
        const int length = detections.length();
        for(int i = 0; i < length; ++i)
        {
            const Detection detection = detections[i];
            cv::imwrite(QString(dirPath + "/" + "crop_" + className + "_" + QString::number(i+1) + ".png").toStdString(), detection.mat);
        }
        print("    " + QString::number(length) + "x" + className);
    }
}

void MainWindow::on_actionExport_Image_triggered()
{
    exportImage();
}

void MainWindow::on_exportButton_clicked()
{
    exportImage();
}

void MainWindow::on_actionViewInference_triggered(bool checked)
{
    _inferenceEnabled = checked;
}

void MainWindow::on_actionViewText_triggered(bool checked)
{
    _drawText = checked;
}

void MainWindow::on_actionViewOutline_triggered(bool checked)
{
    _drawOutline = checked;
}

