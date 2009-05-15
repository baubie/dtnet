#include "networkitem.h"


#define BOX_WIDTH 130
#define BOX_HEIGHT 70
#define SHADOW_OFFSET -2
#define BOX_CORNER 5


NetworkItem::NetworkItem(const QString &ID, const QString &name, const QString &model_type)
{
    this->itemID = ID;
    this->itemName = name;
    this->itemModel = model_type;
    setFlag(ItemIsMovable);
    setZValue(1);
}

void NetworkItem::focusInEvent( QFocusEvent *event )
{
}

 QRectF NetworkItem::boundingRect() const
 {
     return QRectF(-BOX_WIDTH/2, 
                   -BOX_HEIGHT/2, 
                    BOX_WIDTH+SHADOW_OFFSET,
                    BOX_HEIGHT+SHADOW_OFFSET
                    );
 }

 QPainterPath NetworkItem::shape() const
 {
     QPainterPath path;
     path.addRoundedRect(-BOX_WIDTH/2,
                         -BOX_HEIGHT/2,
                          BOX_WIDTH+SHADOW_OFFSET,
                          BOX_HEIGHT+SHADOW_OFFSET,
                          BOX_CORNER, 
                          BOX_CORNER);
     return path;
 }

void NetworkItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
 {
     /*
     painter->setPen(Qt::NoPen);
     painter->setBrush(Qt::darkGray);
     painter->drawRoundedRect(-BOX_WIDTH/2,
                              -BOX_HEIGHT/2,
                               BOX_WIDTH,
                               BOX_HEIGHT,
                               BOX_CORNER, 
                               BOX_CORNER);
                               */

     if (option->state & QStyle::State_Sunken)
     {
         painter->setBrush(QColor(120,120,200,240));
     } else {
         painter->setBrush(QColor(80,80,100,200));
     }
     painter->setPen(QPen(Qt::black, 0));
     painter->drawRoundedRect(-BOX_WIDTH/2+SHADOW_OFFSET,
                              -BOX_HEIGHT/2+SHADOW_OFFSET,
                               BOX_WIDTH,
                               BOX_HEIGHT,
                               BOX_CORNER, 
                               BOX_CORNER);

     painter->setPen(Qt::white);
     painter->setBrush(Qt::white);
     painter->drawText(-BOX_WIDTH/2,-BOX_HEIGHT/2, BOX_WIDTH, BOX_HEIGHT, 
                        Qt::AlignHCenter | 
                        Qt::AlignVCenter |
                        Qt::TextWordWrap,
                        itemName+"\n"+itemModel);
 }
