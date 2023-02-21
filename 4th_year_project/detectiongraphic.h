#ifndef DETECTIONGRAPHIC_H
#define DETECTIONGRAPHIC_H

#include <QGraphicsItem>
#include <QtMath>
#include <inference.h>

class DetectionGraphic : public QGraphicsItem
{
public:
    DetectionGraphic(Detection detection);

    // QGraphicsItem interface
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    Detection _detection;
    const qreal _fontSize = 40;
};

#endif // DETECTIONGRAPHIC_H
