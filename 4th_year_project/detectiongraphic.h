#ifndef DETECTIONGRAPHIC_H
#define DETECTIONGRAPHIC_H

#include <QGraphicsItem>
#include <inference.h>

class DetectionGraphic : public QGraphicsItem
{
public:
    DetectionGraphic(Detection detection);

private:
    Detection _detection;

    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // DETECTIONGRAPHIC_H
