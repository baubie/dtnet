#include "networkitem.h"

NetworkItem::NetworkItem(const QString &ID, const QString &name)
{
    this->itemID = ID;
    this->itemName = name;
    setFlag(ItemIsMovable);
    setZValue(1);
}

 QRectF NetworkItem::boundingRect() const
 {
     qreal adjust = 2;
     return QRectF(-10 - adjust, -10 - adjust,
                   23 + adjust, 23 + adjust);
 }

 QPainterPath NetworkItem::shape() const
 {
     QPainterPath path;
     path.addEllipse(-10, -10, 20, 20);
     return path;
 }

void NetworkItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
 {
     painter->setPen(Qt::NoPen);
     painter->setBrush(Qt::darkGray);
     painter->drawEllipse(-7, -7, 20, 20);

     QRadialGradient gradient(-3, -3, 10);
     if (option->state & QStyle::State_Sunken) {
         gradient.setCenter(3, 3);
         gradient.setFocalPoint(3, 3);
         gradient.setColorAt(1, QColor(Qt::yellow).light(120));
         gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
     } else {
         gradient.setColorAt(0, Qt::yellow);
         gradient.setColorAt(1, Qt::darkYellow);
     }
     painter->setBrush(gradient);
     painter->setPen(QPen(Qt::black, 0));
     painter->drawEllipse(-10, -10, 20, 20);
 }
