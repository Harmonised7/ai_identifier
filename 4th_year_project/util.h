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
#include <QGraphicsScene>
#include <QPainter>

#include<array>
#include<functional>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/videoio/registry.hpp>

#include "config.h"
#include "inference.h"

using namespace std;
using namespace cv;

class Util
{
public:
    static inline const double BEST_RED = 255, BEST_GREEN = 0, BEST_BLUE = 141;
    static inline const QColor BEST_COLOR = QColor(BEST_RED, BEST_GREEN, BEST_BLUE);
    static inline const Scalar BEST_SCALAR = Scalar(BEST_COLOR.blue(),  BEST_COLOR.green(), BEST_COLOR.red());
    static inline const QColor WHITE_COLOR = QColor(255, 255, 255);
    static inline const Scalar WHITE_SCALAR = Scalar(WHITE_COLOR.blue(),  WHITE_COLOR.green(), WHITE_COLOR.red());
    static inline const QColor BLACK_COLOR = QColor(0, 0, 0);
    static inline const Scalar BLACK_SCALAR = Scalar(BLACK_COLOR.blue(),  BLACK_COLOR.green(), BLACK_COLOR.red());

    static inline const QString BLACK = "black";
    static inline const QString BROWN = "brown";
    static inline const QString RED = "red";
    static inline const QString ORANGE = "orange";
    static inline const QString YELLOW = "yellow";
    static inline const QString GREEN = "green";
    static inline const QString BLUE = "blue";
    static inline const QString VIOLET = "violet";
    static inline const QString GREY = "grey";
    static inline const QString WHITE = "white";
    static inline const QString GOLD = "gold";

    static inline const QString RESISTOR = "resistor";

    static QMap<QString, int> makeResistorColorCodesMap()
    {
        QMap<QString, int> map;
        map.insert(BLACK, 0);
        map.insert(BROWN, 1);
        map.insert(RED, 2);
        map.insert(ORANGE, 3);
        map.insert(YELLOW, 4);
        map.insert(GREEN, 5);
        map.insert(BLUE, 6);
        map.insert(VIOLET, 7);
        map.insert(GREY, 8);
        map.insert(WHITE, 9);
        map.insert(GOLD, 5);
        return map;
    }

    static inline const QMap<QString, int> RESISTOR_COLOR_CODES = makeResistorColorCodesMap();

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

    static double toDegs(const double &rads)
    {
        return qRadiansToDegrees(rads);
    }

    static double toRads(const double &degs)
    {
        return qDegreesToRadians(degs);
    }

    static double getAngleDegs(const double &p1x, const double &p1y, const double &p2x, const double &p2y)
    {
        return toDegs(getAngleRads(p1x, p1y, p2x, p2y));
    }

    static double getAngleRads(const double &p1x, const double &p1y, const double &p2x, const double &p2y)
    {
        return -atan2(p2y * p1x - p2x * p1y, p1x * p2x + p1y * p2y);
    }

    static QPointF offsetVec2InRadius(double x, double y, const double &radius, const double &degs)
    {
        const double rads = toRads(degs);
        return QPointF(x + radius * qSin(rads), y + radius * qCos(rads));
    }

    static QPointF offsetVec2InRadius(QPointF &originPos, const double &radius, const double &degs)
    {
        return offsetVec2InRadius(originPos.x(), originPos.y(), radius, degs);
    }

    static Mat saturate(const Mat &inputMat, const double &alpha, const double &beta)
    {
        const double rows = inputMat.rows;
        const double cols = inputMat.cols;
        Mat saturatedImage = Mat::zeros(inputMat.size(), inputMat.type());
        for( int y = 0; y < rows; y++ )
        {
            for( int x = 0; x < cols; x++ )
            {
                for( int c = 0; c < inputMat.channels(); c++ ) {
                    saturatedImage.at<Vec3b>(y,x)[c] =
                      saturate_cast<uchar>(alpha*inputMat.at<Vec3b>(y,x)[c] + beta);
                }
            }
        }
        return saturatedImage;
    }

    static QRectF midRect(const double &midX, const double &midY, double &width, double height = 0)
    {
        if(height <= 0)
            height = width;
        return QRectF(midX-width/2.0, midY-height/2.0, width, height);
    }

    static double getLineY(const double &x, const double &slope, const double &constant = 0)
    {
        return x*slope + constant;
    }

    static QLineF getLineCrossingDegs(QPointF originPos, const double &length, const double &degs)
    {
        const QPointF pos1 = offsetVec2InRadius(originPos, length, degs);
        const QPointF pos2 = offsetVec2InRadius(originPos, length, degs+180);
        return QLineF(pos1, pos2);
    }

    static void for3d(const int &maxX, const int &maxY, const int &maxZ, std::function<void(const int &x, const int &y, const int &z)> func, const bool maxInclusive = false)
    {
        for3d(0, maxX, 0, maxY, 0, maxZ, func, maxInclusive);
    }

    static void for3d(const int &minX, const int &maxX, const int &minY, const int &maxY, const int &minZ, const int &maxZ, std::function<void(const int &x, const int &y, const int &z)> func, const bool maxInclusive = false)
    {
        const int finalMaxZ = maxInclusive ? maxZ + 1 : maxZ;
        const int finalMaxY = maxInclusive ? maxY + 1 : maxY;
        const int finalMaxX = maxInclusive ? maxX + 1 : maxX;
        for(int z = minZ; z < finalMaxZ; ++z)
        {
            for(int y = minY; y < finalMaxY; ++y)
            {
                for(int x = minX; x < finalMaxX; ++x)
                {
                    func(x, y, z);
                }
            }
        }
    }

    static void for2d(const int &maxX, const int &maxY, std::function<void(const int &x, const int &y)> func, const bool maxInclusive = false)
    {
        for2d(0, maxX, 0, maxY, func, maxInclusive);
    }

    static void for2d(const int &minX, const int &maxX, const int &minY, const int &maxY, std::function<void(const int &x, const int &y)> func, const bool maxInclusive = false)
    {
        const int finalMaxY = maxInclusive ? maxY + 1 : maxY;
        const int finalMaxX = maxInclusive ? maxX + 1 : maxX;
        for(int y = minY; y < finalMaxY; ++y)
        {
            for(int x = minX; x < finalMaxX; ++x)
            {
                func(x, y);
            }
        }
    }

    static void for1d(const int &maxI, std::function<void(const int &i)> func)
    {
        for1d(0, maxI, func);
    }

    static void for1d(const int &minI, const int &maxI, std::function<void(const int &i)> func)
    {
        for(int i = minI; i < maxI; ++i)
        {
            func(i);
        }
    }

    static qreal clamp(const qreal &value, const qreal &minValue, const qreal &maxValue)
    {
        return max(minValue, min(maxValue, value));
    }

    static void drawAxis(Mat& image, Point2d p1, Point2d q1, Scalar color, const double scale = 1)
    {
        double angle = atan2(p1.y - q1.y, p1.x - q1.x);
        double hypotenuse = sqrt(pow(p1.y - q1.y, 2) + pow(p1.x - q1.x, 2));

        q1.x = p1.x - scale * hypotenuse * cos(angle);
        q1.y = p1.y - scale * hypotenuse * sin(angle);
        line(image, Point(static_cast<int>(p1.x), static_cast<int>(p1.y)), Point(static_cast<int>(q1.x), static_cast<int>(q1.y)), color, 3, LINE_AA);

        p1.x = q1.x + 9 * cos(angle + CV_PI / 4);
        p1.y = q1.y + 9 * sin(angle + CV_PI / 4);
        line(image, Point(static_cast<int>(p1.x), static_cast<int>(p1.y)), Point(static_cast<int>(q1.x), static_cast<int>(q1.y)), color, 3, LINE_AA);

        p1.x = q1.x + 9 * cos(angle - CV_PI / 4);
        p1.y = q1.y + 9 * sin(angle - CV_PI / 4);
        line(image, Point(static_cast<int>(p1.x), static_cast<int>(p1.y)), Point(static_cast<int>(q1.x), static_cast<int>(q1.y)), color, 3, LINE_AA);
    }

    static double getOrientation(const std::vector<Point>& pts, Mat& image, const double drawScale = 0)
    {
        //Construction of a buffer for the PCA analysis
        const int imageSize = static_cast<int>(pts.size());
        Mat dataPoints(imageSize, 2, CV_64F);
        for (int i = 0; i < dataPoints.rows; i++) {
            dataPoints.at<double>(i, 0) = pts[i].x;
            dataPoints.at<double>(i, 1) = pts[i].y;
        }

        //PCA analysis (EigenValues, EigenVectors, etc!)
        const PCA pca(dataPoints, {}, PCA::DATA_AS_ROW);

        //Center of the bounding box (mean|average)
        Point center(pca.mean.at<double>(0, 0), pca.mean.at<double>(0, 1));

        double angleRads = atan2(pca.eigenvectors.at<double>(0, 1), pca.eigenvectors.at<double>(0, 0)); // orientation in radians

        if(drawScale > 0)
        {
            // Draw the principal components
            circle(image, center, 3, Scalar(0, 255, 255), 2*drawScale);
            Point2d p1(center.x + 0.2*drawScale * pca.eigenvectors.at<double>(0, 0) * pca.eigenvalues.at<double>(0, 0),
                       center.y + 0.2*drawScale * pca.eigenvectors.at<double>(0, 1) * pca.eigenvalues.at<double>(0, 0));
            Point2d p2(center.x - 0.2*drawScale * pca.eigenvectors.at<double>(1, 0) * pca.eigenvalues.at<double>(1, 0),
                       center.y - 0.2*drawScale * pca.eigenvectors.at<double>(1, 1) * pca.eigenvalues.at<double>(1, 0));
            line(image, center, p1, Scalar(255, 0, 255), 2*drawScale);
            line(image, center, p2, Scalar(255, 255, 0), 1*drawScale);

            //Label with the rotation angle
//            QString label = " Rotation Angle: " + QString::number(-static_cast<int>(round(angle * 180 / CV_PI)) - 90) + " degrees";
//            rectangle(img, Point(center.x, center.y - 25), Point(center.x + 250, center.y + 10), Scalar(255, 255, 255), -1);
//            cv::putText(img, label.toStdString(), Point(center.x, center.y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1, LINE_AA);
        }

        return angleRads;
    }

    static void hsvFilter(Mat inputMat, const double &minHueF = 0, const double &maxHueF = 1, const double minSatF = 0, const double maxSatF = 1, const double minValF = 0, const double maxValF = 1, const QColor elseColor = Util::BLACK_COLOR, const bool type = false, const int inX1 = -1, const int inY1 = -1, const int inX2 = -1, const int inY2 = -1)
    {
        const int x1 = inX1 >= 0 ? max(0, inX1) : 0;
        const int x2 = inX2 >= 0 ? min(inputMat.cols-1, inX2) : inputMat.cols-1;

        const int y1 = inY1 >= 0 ? max(0, inY1) : 0;
        const int y2 = inY2 >= 0 ? min(inputMat.rows-1, inY2) : inputMat.rows-1;

        for(int x = x1; x < x2; ++x)
        {
            for(int y = y1; y < y2; ++y)
            {
                Vec3b &bgr = inputMat.at<Vec3b>(Point(x, y));
                const QColor color = QColor(bgr[2], bgr[1], bgr[0]);
                const double hue = Util::clamp(color.hueF(), 0, 1);
                const double sat = color.saturationF();
                const double val = color.valueF();

                const bool correctHue = filter(hue, minHueF, maxHueF);
                const bool correctSat = filter(sat, minSatF, maxSatF);
                const bool correctVal = filter(val, minValF, maxValF);

                if(type == (correctHue && correctSat && correctVal))
                {
                    bgr[0] = elseColor.blue();
                    bgr[1] = elseColor.green();
                    bgr[2] = elseColor.red();
                }
            }
        }
    }

    static bool filter(const qreal &input, const qreal &minBound, const qreal &maxBound)
    {
        if(maxBound < minBound)
            return input < maxBound || input > minBound;
        else
        {
            return minBound <= input && input <= maxBound;
        }
    }

    static void adjustSaturationTo(cv::Mat& image, const double &alpha, const double &beta)
    {
        cv::Mat result;

        // Split the image into its color channels
        std::vector<cv::Mat> channels;
        cv::split(image, channels);

        // Apply brightness adjustment to each color channel
        for (int i = 0; i < channels.size(); i++)
        {
            channels[i].convertTo(channels[i], -1, alpha, beta);
            cv::equalizeHist(channels[i], channels[i]);
        }

        // Merge the color channels back into a single image
        cv::merge(channels, result);

        // Set the result as the new image
        image = result;
    }

    static void brightnessFilter(cv::Mat& inputMat, const int minThreshold, const int maxThreshold)
    {
        for(int x = 0; x < inputMat.cols; ++x)
        {
            for(int y = 0; y < inputMat.rows; ++y)
            {
                Vec3b &bgr = inputMat.at<Vec3b>(Point(x, y));
                const double bright = (bgr[0] + bgr[1] + bgr[2])/3;
                const bool tooLow = bright <= minThreshold;
                const bool tooHigh = bright > maxThreshold;
                if(tooLow || tooHigh)
                {
                    bgr[0] = 0;
                    bgr[1] = 0;
                    bgr[2] = 0;
                }
            }
        }
    }

    template <typename T>
    static QList<T> reverse(const QList<T> &listIn)
    {
        QList<T> listOut;
        listOut.reserve(listIn.size()); // reserve is new in Qt 4.7
        std::reverse_copy(listIn.begin(), listIn.end(), std::back_inserter(listOut));
        return listOut;
    }

    static QString identifyBandColor(QColor bandColor)
    {
        const double value = bandColor.valueF();
        if(value < 0.25)
            return BLACK;

        const int hue = bandColor.hue();
        const double saturation = bandColor.saturationF();

        if(hue > 10)
        {
            if(hue < 30)
            {
                if(saturation < 0.3)
                    return GOLD;
                else if(value < 0.6)
                    return BROWN;
                else
                    return ORANGE;
            }
            else if(hue < 50)
            {
                if(saturation < 0.5 || value < 0.6)
                    return GOLD;
                else
                    return ORANGE;
            }
            else if(hue <= 85)
            {
                if(saturation < 0.75 || value < 0.5)
                    return GOLD;
                else
                    return YELLOW;
            }
            else if(hue <= 140)
                return GREEN;
        }

        if(hue > 320 || hue <= 10)
        {
            if(hue > 320)
                return RED;
            else
            {
                if(saturation > 0.666 && value > 0.666)
                    return RED;
                return BROWN;
            }
        }

        if(hue > 180 && hue < 290)
        {
            if(hue <= 245)
                return BLUE;
            else
                return VIOLET;
        }
        return "unknown";
    }

    static QList<QString> identifyBandColors(QList<QColor> bands)
    {

        QList<QString> stringCodedBands;
        if(bands.length() == 0)
            return stringCodedBands;
        for(const QColor color : bands)
        {
            stringCodedBands.push_back(identifyBandColor(color));
        }
        //We're looking at the resistor backwards
        if(stringCodedBands[0] == GOLD)
            stringCodedBands = reverse(stringCodedBands);
        return stringCodedBands;
    }

    static int decodeBand(QColor bandColor)
    {
        return decodeBand(identifyBandColor(bandColor));
    }

    static int decodeBand(QString bandColor)
    {
        return RESISTOR_COLOR_CODES[bandColor];
    }

    static qreal decodeResistorBands(QList<QColor> bands)
    {
        return decodeResistorBands(identifyBandColors(bands));
    }

    static qreal decodeResistorBands(QList<QString> bands)
    {
        int ohms = 0;
        const int length = bands.length();
        if(length == 0)
            return ohms;
        //We're looking at the resistor backwards
        if(bands[0] == GOLD)
            bands = reverse(bands);
        const bool toleranceBandPresent = bands[length-1] == GOLD;
        const int valueBandsLength = toleranceBandPresent ? length - 1 : length;

        for(int i = 0; i < valueBandsLength; ++i)
        {
            const QString bandColor = bands[i];
            const int power = valueBandsLength-2-i;
            const int bandValue = decodeBand(bandColor);
            if(power >= 0)
                ohms += pow(10, power) * bandValue;
            else
                ohms *= pow(10, bandValue);
        }

        return ohms;
    }

    static Scalar toScalar(QColor color)
    {
        return Scalar(color.blue(), color.green(), color.red());
    }

    static QList<int> getRowNonBlackCount(Mat inputMat)
    {
        return getRowNonBlackCount(inputMat, 0, inputMat.cols);
    }

    static QList<int> getRowNonBlackCount(Mat inputMat, int x1, int x2)
    {
        QList<int> rowsCount;
        for(int y = 0; y < inputMat.rows; ++y)
        {
            int count = 0;
            for(int x = x1; x < x2; ++x)
            {
                const Vec3b &bgr = inputMat.at<Vec3b>(Point(x, y));
                if(bgr[0] != 0 && bgr[1] != 0 && bgr[2] != 0)
                    ++count;
            }
            rowsCount.push_back(count);
        }
        return rowsCount;
    }

    static QList<double> getRowMeanHue(Mat inputMat)
    {
        return getRowMeanHue(inputMat, 0, inputMat.cols);
    }

    static QList<double> getRowMeanHue(Mat inputMat, int x1, int x2)
    {
        QList<double> rowsCount;
        for(int y = 0; y < inputMat.rows; ++y)
        {
            double totalHue = 0;
            int count = 0;
            for(int x = x1; x < x2; ++x)
            {
                const Vec3b &bgr = inputMat.at<Vec3b>(Point(x, y));
                const QColor color = QColor(bgr[2], bgr[1], bgr[0]);
                const double hueF = color.hueF();
                if(hueF < 0)
                    continue;
                if(isBlack(bgr))
                    continue;
                totalHue += hueF;
                ++count;
            }
            rowsCount.push_back(count > 0 ? totalHue/(double) count : 0);
        }
        return rowsCount;
    }

    static QList<QColor> getRowMeanColor(Mat inputMat, int x1, int x2)
    {
        QList<QColor> rowsCount;
        for(int y = 0; y < inputMat.rows; ++y)
        {
            double r = 0;
            double g = 0;
            double b = 0;
            int count = 0;
            for(int x = x1; x < x2; ++x)
            {
                const Vec3b &bgr = inputMat.at<Vec3b>(Point(x, y));
                const QColor color = QColor(bgr[2], bgr[1], bgr[0]);
                const double hueF = color.hueF();
                if(hueF < 0)
                    continue;
                if(isBlack(bgr))
                    continue;
                r += color.redF();
                g += color.greenF();
                b += color.blueF();
                ++count;
            }
            rowsCount.push_back(count > 0 ? QColor::fromRgbF(r/(double) count, g/(double) count, b/(double) count) : Util::BLACK_COLOR);
        }
        return rowsCount;
    }

    static bool isBlack(const Vec3b &pixel)
    {
        return pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0;
    }

    static Mat blur(Mat inputMat, const int blurAmount = 1)
    {
        return blur(inputMat, blurAmount, 0, 0, inputMat.cols, inputMat.rows);
    }

    static Mat blur(Mat inputMat, const int blurAmount, const int x1, const int y1, const int x2, const int y2)
    {
        const int rows = inputMat.rows;
        const int cols = inputMat.cols;
        Mat outputMat = inputMat.clone();

        for(int row = y1; row < y2; ++row)
        {
            for(int col = x1; col < x2; ++col)
            {
                if(isBlack(inputMat.at<Vec3b>(Point(col, row))))
                    continue;
                const int y1 = max(0, row-blurAmount);
                const int y2 = min(rows, row+blurAmount);

                const int x1 = max(0, col-blurAmount);
                const int x2 = min(cols, col+blurAmount);
                double total = ((y2 - y1) + 1) * ((x2 - x1) + 1);
                int totalR = 0;
                int totalG = 0;
                int totalB = 0;

                for(int y = y1; y <= y2; ++y)
                {
                    for(int x = x1; x <= x2; ++x)
                    {
                        const Vec3b &bgr = inputMat.at<Vec3b>(Point(x, y));
                        totalR += bgr[2];
                        totalG += bgr[1];
                        totalB += bgr[0];
                    }
                }
                Vec3b &bgr = outputMat.at<Vec3b>(Point(col, row));
                bgr[2] = totalR / total;
                bgr[1] = totalG / total;
                bgr[0] = totalB / total;
            }
        }
        return outputMat;
    }

    static void betterShow(const Mat inputMat, const QString name, const double scale = 1)
    {
        betterShow(inputMat, name.toStdString(), scale);
    }

    static void betterShow(const Mat inputMat, const std::string name, const double scale = 1)
    {
        namedWindow(name, cv::WINDOW_NORMAL);
        resizeWindow(name, inputMat.cols*scale, inputMat.rows*scale);
        imshow(name, inputMat);
    }

    template<typename T>
    static T make(T item, std::function<void(T item)> init)
    {
        init(item);
        return item;
    }

    static QList<QColor> getOhmBands(Mat inputMat, const qreal horizontalBoundPos = 0.7,
                                     const qreal verticalBoundPos = 0.75,
                                     const qreal middleCropBoundPos = 0.125,
                                     const qreal minHue = 0, const qreal maxHue = 1,
                                     const qreal minSat = 0.35, const qreal maxSat = 1,
                                     const qreal minVal = 0, const qreal maxVal = 1,
                                     const bool display = false, QString displayName = "")
    {
        //Crop out the glare region
        const int cropFromMid = inputMat.cols * (cap(middleCropBoundPos, 0, 1)/2.) - 1;
        const Rect leftRect(0, 0, (inputMat.cols/2) - cropFromMid, inputMat.rows);
        const Rect rightRect(inputMat.cols/2 + cropFromMid, 0, (inputMat.cols/2) - cropFromMid, inputMat.rows);
        hconcat(Mat(inputMat, leftRect), Mat(inputMat, rightRect), inputMat);
    //    imshow((QString::number(++_debugCountInt) + "_" + name).toStdString(), inputMat);
        QList<QColor> ohmBands;
    //    return ohmBands;

        const double imageWidth = inputMat.cols;
//        const double midX = imageWidth/2.;

        const double imageHeight = inputMat.rows;
//        const double midY = imageHeight/2.;

        const double lineWidth = 20;

        //Vertical Sampling
    //    const double rectWidth = 60;
        const double rectHeight = imageHeight*cap(verticalBoundPos, 0, 1);
        const double x1 = cap(horizontalBoundPos, 0, 1) * ((imageWidth-lineWidth)/2.);
        const double x2 = imageWidth - x1;
        const int rectWidth = x2 - x1;

        const double y1 = (imageHeight - rectHeight) / 2.;
        const double y2 = y1 + rectHeight;

        inputMat = Util::blur(inputMat, 1, x1, y1, x2, y2);
    //    inputMat = Util::saturate(inputMat, 6, -600);
        Util::hsvFilter(inputMat, minHue, maxHue, minSat, maxSat, minVal, maxVal, Util::BLACK_COLOR, false, x1, y1, x2, y2);

        const int rowCountThreshold = rectWidth*0.25;
        QList<int> rowsCount = Util::getRowNonBlackCount(inputMat, x1, x2);
        QList<QColor> colorMeans = Util::getRowMeanColor(inputMat, x1, x2);
    //    qDebug() << "width" << imageWidth;
        int startY = 0;
        bool bandStarted = false;
        double bandMeanHue;
        for(int y = y1; y < y2; ++y)
        {
            //Decision if currently inside a band, or not.
            const QColor rowMeanColor = colorMeans[y];
            const double rowMeanHue = rowMeanColor.hueF();
            const int rowCount = rowsCount[y];
    //        qDebug() << y << rowCount;
    //        qDebug() << y << rowMeanHue;
            if(rowCount > rowCountThreshold)
            {
                if(!bandStarted)
                {
                    bandMeanHue = 0;
                    startY = y;
                }
                bandMeanHue += rowMeanHue;
    //            qDebug() << "row mean hue" << rowMeanHue;
                bandStarted = true;
                continue;
            }
            if(!bandStarted)
                continue;
            //At the end of a band. startY is the start.
            bandStarted = false;

            const int height = y - startY;
            const int midY = (y + startY)/2;
            bandMeanHue /= height;

            //If band is tall enough
            if(height >= 3)
            {
    //            qDebug() << y - startY << startY << "to" << y;
                const QColor midColor = colorMeans[midY];
                if(display)
                {
                    const Scalar scalar = Util::toScalar(midColor);
            //        rectangle(inputMat, Rect(x1, startY, rectWidth, height), scalar, 2);
                    rectangle(inputMat, Rect(0, startY, imageWidth, height), scalar, 2);
                }
                ohmBands.push_back(midColor);
                y += 16;
            }
        }
        if(display)
        {
            rectangle(inputMat, Rect(x1-1, 0, 2, imageHeight), Util::BEST_SCALAR, 2);
            rectangle(inputMat, Rect(x2-1, 0, 2, imageHeight), Util::BEST_SCALAR, 2);

            rectangle(inputMat, Rect(0, y1-1, imageWidth, 2), Util::BEST_SCALAR, 2);
            rectangle(inputMat, Rect(0, y2-1, imageWidth, 2), Util::BEST_SCALAR, 2);

            Util::betterShow(inputMat, displayName, 3);
        }
        return ohmBands;
    }

    static Mat prepareResistorMat(Mat inputMat, const bool display = false)
    {
        Util::adjustSaturationTo(inputMat, 0.1, 100);
        const Size imageSize = inputMat.size();
        const double imageWidth = inputMat.cols;
        const double midX = imageWidth/2.0;

        const double imageHeight = inputMat.rows;
        const double midY = imageHeight/2.0;

        QPointF midPos = QPointF(midX, midY);

        const double lowestDimension = min(imageWidth, imageHeight);
        const double highestDimension = max(imageWidth, imageHeight);

        const QPointF origin = QPointF(0, 0);

        Mat grey;
        cvtColor(inputMat, grey, cv::COLOR_BGR2GRAY);

        Mat bw;
        threshold(grey, bw, 50, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

        vector<vector<Point>> contours;
        findContours(bw, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

        double highestArea = 0;
    //    int highestAreaIndex = -1;
        double orientation = 0;
        for (int i = 0; i < contours.size(); i++)
        {
            vector<Point> contour = contours[i];
            double area = cv::contourArea(contour);
            if (area < 2000/* || area > 10000*/)
                continue;
            if(display)
            {
                drawContours(inputMat, contours, i, cv::Scalar(255, 0, 255), 2);
            }
            const double countourOrientation = Util::getOrientation(contour, inputMat, 0);
            if(highestArea < area)
            {
                highestArea = area;
    //            highestAreaIndex = i;
                orientation = countourOrientation;
            }
        }

        //Rotation
        const cv::Point2f centerPoint(midX, midY);
        const double finalDegs = Util::toDegs(orientation) + 90;
        const Rect2f boundingBox = cv::RotatedRect(cv::Point2f(), imageSize, finalDegs).boundingRect2f();
        cv::Mat rotMat = cv::getRotationMatrix2D(centerPoint, finalDegs, 1.0);
        rotMat.at<double>(0,2) += boundingBox.width/2.0 - imageWidth/2.0;
        rotMat.at<double>(1,2) += boundingBox.height/2.0 - imageHeight/2.0;
        warpAffine(inputMat, inputMat, rotMat, boundingBox.size());

        return inputMat;
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
    static VAL computeIfAbsent(QMap<KEY, VAL> *map, const KEY key, std::function<VAL(KEY)> getter)
    {
        if(!map->contains(key))
            map->insert(key, getter());
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

    static void exportSceneToFile(QGraphicsScene &scene, QString name = getFormattedDate(), QString extension = "png")
    {
        if(name.length() == 0)
            name = Util::getFormattedDate();

        //Export the scene as an image
        QImage image(scene.width(), scene.height(), QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        scene.render(&painter);

        //Save image
        image.save(name + "." + extension);
    }

    static Rect boundedRect(const qreal &x, const qreal &y, const qreal &width, const qreal &height, const qreal minX = 0, const qreal minY = 0, const qreal maxX = CAMERA_WIDTH, const qreal maxY = CAMERA_HEIGHT)
    {
        return Rect
            (
                max(x, minX),
                max(y, minY),
                min(maxX - x, width),
                min(maxY - y, height)
            );
    }
};

#endif // UTIL_H
