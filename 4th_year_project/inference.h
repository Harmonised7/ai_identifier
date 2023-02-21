#ifndef INFERENCE_H
#define INFERENCE_H

// Qt includes
#include <QObject>
#include <QDebug>
#include <QColor>
#include <QFile>

// Cpp native
#include <fstream>
#include <vector>
#include <string>
#include <random>

// OpenCV / DNN / Inference
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include <util.h>

struct Detection
{
    int class_id{0};
    QString className{};
    QString extra{};
    float confidence{0.0};
    cv::Scalar color{};
    cv::Rect box{};
    cv::Mat mat{};
};

class Inference
{
public:
    // Initialize the inference class
    Inference(const QString &onnxModelPath, const cv::Size2f &modelInputShape, const QString &classesTxtFile, const bool &runWithCuda = true);
    QVector<Detection> runInference(const cv::Mat &input);

private:
    void loadClassesFromFile();
    void loadOnnxNetwork();

    QString modelPath{};
    QString classesPath{};
    bool cudaEnabled{};

    QVector<cv::Scalar> colors{};
    QVector<QString> classes{};

    cv::Size2f modelShape{};

    float modelConfidenseThreshold  {0.7};
    float modelScoreThreshold       {0.2};
    float modelNMSThreshold         {0.4};

    cv::dnn::Net net;
};

#endif // INFERENCE_H
