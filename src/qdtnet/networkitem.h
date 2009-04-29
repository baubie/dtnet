#ifndef NETWORKITEM_H
#define NETWORKITEM_H

 #include <QGraphicsItem>
 #include <QGraphicsSceneMouseEvent>
 #include <QPainter>
 #include <QStyleOption>

class NetworkItem : public QGraphicsItem
{
public:
     NetworkItem(const QString &ID, const QString &name);
     QRectF boundingRect() const;
     QPainterPath shape() const;
     void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QString itemID;
    QString itemName;
};

#endif
