#include "inference.h"

Inference::Inference(const QString &onnxModelPath, const cv::Size2f &modelInputShape, const QString &classesTxtFile, const bool &runWithCuda)
{
    // TODO: This could use more checks to ensure that the paths aren't empty etc...

    modelPath = onnxModelPath;
    modelShape = modelInputShape;
    classesPath = classesTxtFile;
    cudaEnabled = runWithCuda;

    // Load the Onnx (Converted from .pt) network
    loadOnnxNetwork();

    // Load in the classes/class colors from file (Acumen project)
    loadClassesFromFile();

    qDebug() << "_<<Initialized the Inference Engine>>_";
}

QList<Detection> Inference::runInference(const cv::Mat &input)
{
    cv::Mat blob;
    //Crop to 1:1 if not 1:1
    cv::Mat inputImage = input.rows == input.cols
            ? input :
              Util::cropTo1By1Mid(input);

    cv::dnn::blobFromImage(inputImage, blob, 1.0/255.0, modelShape, cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    float x_factor = inputImage.cols / modelShape.width;
    float y_factor = inputImage.rows / modelShape.height;

    float *data = (float *)outputs[0].data;

    // Number of Classes + 5
    const int dimensions = outputs[0].size[2];
    // Natural number of the YOLO detections (Usually 25200 @640x640)
    const int rows = outputs[0].size[1];

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < rows; ++i)
    {
        float confidence = data[4];

        if (confidence >= modelConfidenseThreshold)
        {
            float *classes_scores = data + 5;
            cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double max_class_score;

            minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

            if (max_class_score > modelScoreThreshold)
            {
                confidences.push_back(confidence);
                class_ids.push_back(class_id.x);

                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];

                int left = int((x - 0.5 * w) * x_factor);
                int top = int((y - 0.5 * h) * y_factor);

                int width = int(w * x_factor);
                int height = int(h * y_factor);

                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }

        data += dimensions;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, modelScoreThreshold, modelNMSThreshold, nms_result);

    QVector<Detection> detections{};

    for (int i = 0; i < nms_result.size(); ++i)
    {
        int idx = nms_result[i];

        Detection result;
        result.class_id = class_ids[idx];
        result.confidence = confidences[idx];
        result.color = colors[result.class_id];
        result.className = classes[result.class_id];
        result.box = boxes[idx];

        detections.push_back(result);
    }

    std::sort(detections.begin(), detections.end(), [](const Detection &a, const Detection &b) -> bool {return b.confidence < a.confidence;});

    return detections;
}

void Inference::loadClassesFromFile()
{
    QFile inputFile(classesPath);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QStringList currentLine = in.readLine().split(',');

            QString classLine = currentLine.at(0).simplified();
            QString colorLine = currentLine.at(1).simplified();

            classes.push_back(classLine);

            QColor c(colorLine);
            colors.push_back(cv::Scalar(c.blue(), c.green(), c.red()));

            qDebug() << "New class:" << classLine << "color:" << c.red() << c.green() << c.blue();
        }
        inputFile.close();
    }
    qDebug() << "\n";
}

void Inference::loadOnnxNetwork()
{
    net = cv::dnn::readNetFromONNX(modelPath.toStdString());

    if (cudaEnabled)
    {
        qDebug() << "\nRunning on CUDA\n";
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    }
    else
    {
        qDebug() << "\nRunning on CPU\n";
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

//        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
//        net.setPreferableTarget(cv::dnn::DNN_TARGET_MYRIAD);
    }
}
