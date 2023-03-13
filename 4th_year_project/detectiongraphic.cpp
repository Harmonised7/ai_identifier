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
    QPainterPath path;
    //Pen (outline) uses bounding box color
    QPen pen = QPen(QColor(_detection.color[0], _detection.color[1], _detection.color[2]));
    //Brush (fill) uses 1/8th of bounding box color
    QBrush brush = QBrush(QColor(_detection.color[0]/8, _detection.color[1]/8, _detection.color[2]/8));
    pen.setWidth(5);
    painter->setPen(pen);
    painter->setBrush(brush);

    //Draw the box
    painter->drawPolyline(bound);

    //Text
    QFont font = painter->font();
    font.setPointSizeF(_fontSize);
    painter->setFont(font);
    QString text = _detection.className + " " + QString::number(_detection.confidence).mid(0, 4) + " " + _detection.extra;

    pen.setWidth(10);
    const qreal halfBoundWidth = bound.width()/2.0;
    //result of mapping 0 to CAMERA_WIDTH range, to 0 to 1 range.
    const qreal horizontalPos = (bound.left()+halfBoundWidth) / CAMERA_WIDTH;
    const qreal estimatedTextWidth = _fontSize*text.length()*0.7;
    //Text starts from left bound of box,
    //If the text length is more than the width of the bounding box
    //Offset the starting position until the text ends at the end of
    //the bounding box scaled by horizontalPos
    const qreal leftBound = bound.left() - max(0.0, estimatedTextWidth - bound.width())*horizontalPos;
    //Convert text to path
    path.addText(QPointF(leftBound, bound.top() - font.pointSizeF()/2.0), font, text);
    //Outline the text path
    painter->strokePath(path, pen);
    //Fill the text inside
    painter->fillPath(path, brush);
}
