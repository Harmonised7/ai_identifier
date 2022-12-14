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
#include <QTextEdit>

#include<array>
#include<functional>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/videoio/registry.hpp>

#include "config.h"

using namespace cv;

class Util
{
public:
    static bool writeToFile(const QJsonArray &json, const QString &outputPath)
    {
        return writeToFile(toByteArray(json), outputPath);
    }

    static bool writeToFile(const QJsonObject &json, const QString &outputPath)
    {
        return writeToFile(toByteArray(json), outputPath);
    }

    static bool writeToFile(const QJsonDocument &json, const QString &outputPath)
    {
        return writeToFile(toByteArray(json), outputPath);
    }

    static QByteArray toByteArray(const QJsonArray &json)
    {
        return toByteArray(QJsonDocument(json));
    }

    static QByteArray toByteArray(const QJsonObject &json)
    {
        return toByteArray(QJsonDocument(json));
    }

    static QByteArray toByteArray(const QJsonDocument &json)
    {
        return json.toJson(QJsonDocument::Compact);
    }

    static bool writeToFile(const QByteArray &byteArray, const QString &outputPath)
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

    static QJsonDocument parseJson(QFile &file)
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

    static QPoint getMidPoint(const QPoint &p1, const QPoint &p2)
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

    static int genRand(const int &min, const int &max = 0)
    {
        if(min == max)
            return min;
        else if(max > min)
            return QRandomGenerator::global()->bounded(min, max + 1);
        else
            return QRandomGenerator::global()->bounded(max, min + 1);
    }

    static double genRandDouble(const double &min, const double &max = 0)
    {
        if(min == max)
            return min;
        else if(max > min)
            return min + QRandomGenerator::global()->bounded(max - min);
        else
            return max + QRandomGenerator::global()->bounded(min - max);
    }

    static QPoint genRandQPoint(const cv::Rect &area)
    {
        return genRandQPoint(QPoint(area.tl().x, area.tl().y), QPoint(area.br().x, area.br().y));
    }

    static Point genRandPoint(const cv::Rect &area)
    {
        return genRandPoint(Point(area.tl().x, area.tl().y), Point(area.br().x, area.br().y));
    }

    static QPoint genRandQPoint(const cv::Mat &mat)
    {
        return genRandQPoint(QPoint(0, 0), QPoint(mat.cols, mat.rows));
    }

    static Point genRandPoint(const cv::Mat &mat)
    {
        return genRandPoint(Point(0, 0), Point(mat.cols, mat.rows));
    }

    static QPoint genRandQPoint(const QPoint &p1, const QPoint &p2)
    {
        return QPoint(p1.x() + genRand(p2.x() - p1.x()), p1.y() + genRand(p2.y() - p1.y()));
    }

    static Point genRandPoint(const Point &p1, const Point &p2)
    {
        return Point(p1.x + genRand(p2.x - p1.x), p1.y + genRand(p2.y - p1.y));
    }

    static QPoint genRandQPointOffset(const QPoint &p, const int &offset)
    {
        return QPoint(p.x() + genRand(-offset, offset), p.y() + genRand(-offset, offset));
    }

    static Point genRandPointOffset(const Point &p, const int &offset)
    {
        return Point(p.x + genRand(-offset, offset), p.y + genRand(-offset, offset));
    }

    static Point movePoint(const Point &p, const int &moveX, const int &moveY)
    {
        return Point(p.x + moveX, p.y + moveY);
    }

    static QPoint moveQPoint(const QPoint &p, const int &moveX, const int &moveY)
    {
        return QPoint(p.x() + moveX, p.y() + moveY);
    }

    static Mat cropTo1By1Mid(const Mat &mat)
    {
        const int width = mat.cols, height = mat.rows;
        if(height < width)
        {
            const int gap = (width - height)/2;
            const Rect cropRect = Rect(gap, 0, height, height);
            return mat(cropRect);
        }
        else
        {
            const int gap = (height - width)/2;
            const Rect cropRect = Rect(gap, 0, width, width);
            return mat(cropRect);
        }
        return mat;
    }

    static Mat cropShrink(Mat &mat, double scaleRatio, double xPosRatio = 0.5, double yPosRatio = 0.5, const bool &keepResolution = true)
    {
        scaleRatio = cap(scaleRatio, 0.01, 1);
        xPosRatio = cap(xPosRatio, 0, 1);
        yPosRatio = cap(yPosRatio, 0, 1);
        const int height = mat.rows, width = mat.cols;
        const int goalWidth = width*scaleRatio, goalHeight = height*scaleRatio;
        const int widthGap = (width-goalWidth), heightGap = (height-goalHeight);
        const Rect cropRect = Rect(widthGap * xPosRatio, heightGap * yPosRatio, goalWidth, goalHeight);
        mat = mat(cropRect);
        if(keepResolution)
            resize(mat, mat, Size(), 1/scaleRatio, 1/scaleRatio, INTER_CUBIC);
        return mat;
    }

    static QString getFormattedDate()
    {
        return QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    }

    static void appendQTextEditLine(QTextEdit *textEdit, const QString &text)
    {
        textEdit->moveCursor(QTextCursor::End);
        textEdit->insertPlainText(text + "\n");
        textEdit->moveCursor(QTextCursor::End);
    }

    template<typename KEY, typename VAL>
    static VAL getOrDefault(QMap<KEY, VAL> *map, const KEY key, VAL defaultValue)
    {
        if(map->contains(key))
            return map->value(key);
        return defaultValue;
    }

    template<typename KEY, typename VAL>
    static VAL computeIfAbsent(QMap<KEY, VAL> *map, const KEY key, std::function<VAL(void)> getter)
    {
        if(!map->contains(key))
            map->insert(getter());
        return map->value(key);
    }

    template<typename KEY>
    static int increment(QMap<KEY, int> *map, const KEY key)
    {
        const int value = map->contains(key) ?
                          map->value(key) + 1 :
                          1;
        map->insert(key, value);
        return map->value(key);
    }

    static QRect rectToQRect(cv::Rect rect)
    {
        return QRect(rect.x, rect.y, rect.width, rect.height);
    }
};

#endif // UTIL_H
