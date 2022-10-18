#ifndef UTIL_H
#define UTIL_H

#include <QMap>
#include <QDebug>
#include <QList>
#include <QString>
#include <QTreeWidgetItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <qmath.h>
#include <QRandomGenerator>
#include <QDateTime>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/videoio/registry.hpp>

using namespace cv;

class Util
{
public:
    static bool writeToFile(QJsonArray json, QString outputPath)
    {
        return writeToFile(toByteArray(json), outputPath);
    }

    static bool writeToFile(QJsonObject json, QString outputPath)
    {
        return writeToFile(toByteArray(json), outputPath);
    }

    static bool writeToFile(QJsonDocument json, QString outputPath)
    {
        return writeToFile(toByteArray(json), outputPath);
    }

    static QByteArray toByteArray(QJsonArray json)
    {
        return toByteArray(QJsonDocument(json));
    }

    static QByteArray toByteArray(QJsonObject json)
    {
        return toByteArray(QJsonDocument(json));
    }

    static QByteArray toByteArray(QJsonDocument json)
    {
        return json.toJson(QJsonDocument::Compact);
    }

    static bool writeToFile(QByteArray byteArray, QString outputPath)
    {
        QFile output(outputPath);
        if(output.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            output.write(byteArray);
            output.close();
            return true;
        }
        else
            QMessageBox::critical(nullptr, "File in use", output.errorString());
        return false;
    }

    static QJsonDocument parseJson(QFile file)
    {
        if(file.exists())
        {
            if(file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QString jsonString = file.readAll();
                file.close();
                return QJsonDocument::fromJson(jsonString.toUtf8());
            }
        }
        return QJsonDocument();
    }

    static QPoint getMidPoint(QPoint p1, QPoint p2)
    {
        return QPoint((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);
    }

    static QPixmap matToPixmap(const Mat &openCVMat)
    {
        return QPixmap::fromImage(QImage(static_cast<const unsigned char*>(openCVMat.data), openCVMat.cols, openCVMat.rows, static_cast<int>(openCVMat.step), QImage::Format_RGB888).rgbSwapped());
    }

    static Mat pixmapToMat(const QPixmap &QtPixmap)
    {
        QImage _tempImage = pixmapToQImage(QtPixmap);

        Mat _tempMat = Mat(_tempImage.height(), _tempImage.width(), CV_8UC4, const_cast<uchar*>(_tempImage.bits()), static_cast<size_t>(_tempImage.bytesPerLine()));
        cvtColor(_tempMat, _tempMat, COLOR_BGRA2BGR);

        return _tempMat;
    }

    static QImage pixmapToQImage(const QPixmap &QtPixmap)
    {
        return QtPixmap.toImage();
    }

    static double map(const double &x, const double &in_min, const double &in_max, const double &out_min, const double &out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    static double mapCapped(const double &x, const double &in_min, const double &in_max, const double &out_min, const double &out_max)
    {
        return cap((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, in_min, in_max);
    }

    static double cap(const double &x, const double &min_bound, const double &max_bound)
    {
        if(max_bound > min_bound)
            return max(min_bound, min(max_bound, x));
        else
            return max(max_bound, min(min_bound, x));
    }

    static double getDistance(Point &p1, Point &p2)
    {
        return qSqrt(qPow(p2.x - p1.x, 2) + qPow(p2.y - p1.y, 2));
    }

    static double getDistance(QPoint &p1, QPoint &p2)
    {
        return qSqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));
    }

    static int genRand(int min, int max = 0)
    {
        if(min == max)
            return min;
        else if(max > min)
            return QRandomGenerator::global()->bounded(min, max + 1);
        else
            return QRandomGenerator::global()->bounded(max, min + 1);
    }

    static double genRandDouble(double min, double max = 0)
    {
        if(min == max)
            return min;
        else if(max > min)
            return min + QRandomGenerator::global()->bounded(max - min);
        else
            return max + QRandomGenerator::global()->bounded(min - max);
    }

    static QPoint genRandQPoint(cv::Rect area)
    {
        return genRandQPoint(QPoint(area.tl().x, area.tl().y), QPoint(area.br().x, area.br().y));
    }

    static Point genRandPoint(cv::Rect area)
    {
        return genRandPoint(Point(area.tl().x, area.tl().y), Point(area.br().x, area.br().y));
    }

    static QPoint genRandQPoint(cv::Mat mat)
    {
        return genRandQPoint(QPoint(0, 0), QPoint(mat.cols, mat.rows));
    }

    static Point genRandPoint(cv::Mat mat)
    {
        return genRandPoint(Point(0, 0), Point(mat.cols, mat.rows));
    }

    static QPoint genRandQPoint(QPoint p1, QPoint p2)
    {
        return QPoint(p1.x() + genRand(p2.x() - p1.x()), p1.y() + genRand(p2.y() - p1.y()));
    }

    static Point genRandPoint(Point p1, Point p2)
    {
        return Point(p1.x + genRand(p2.x - p1.x), p1.y + genRand(p2.y - p1.y));
    }

    static QPoint genRandQPointOffset(QPoint p, int offset)
    {
        return QPoint(p.x() + genRand(-offset, offset), p.y() + genRand(-offset, offset));
    }

    static Point genRandPointOffset(Point p, int offset)
    {
        return Point(p.x + genRand(-offset, offset), p.y + genRand(-offset, offset));
    }

    static Point movePoint(Point p, int moveX, int moveY)
    {
        return Point(p.x + moveX, p.y + moveY);
    }

    static QPoint moveQPoint(QPoint p, int moveX, int moveY)
    {
        return QPoint(p.x() + moveX, p.y() + moveY);
    }
};

#endif // UTIL_H
