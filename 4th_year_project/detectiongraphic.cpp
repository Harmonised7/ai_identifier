#include "detectiongraphic.h"

#include <QPainter>

DetectionGraphic::DetectionGraphic(Detection detection) :
    _detection(detection)
{

}

QRectF DetectionGraphic::boundingRect() const
{
    return QRectF(_detection.box.x, _detection.box.y, _detection.box.width, _detection.box.height);
}

void DetectionGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF bound = boundingRect();
//    QBrush brush(QColor(_detection.color[0], _detection.color[1], _detection.color[2]));
//    painter->setBrush(brush);

    QPen pen = QPen(QColor(_detection.color[0], _detection.color[1], _detection.color[2]));
    pen.setWidth(5);
    painter->setPen(pen);

    painter->drawPolyline(bound);
    QFont font = painter->font();
    font.setPointSizeF(20);
    painter->setFont(font);
    painter->drawText(bound.left(), bound.top() - painter->font().pointSizeF()/2.0, _detection.className + " " + QString::number(_detection.confidence).mid(0, 4));
}
