
#include "networkview.h"


NetworkView::NetworkView(Net *net)
{
	this->net = net;	
	scene = new QGraphicsScene(this);	
	setScene(scene);
}

void NetworkView::replaceNetwork(Net *net)
{
	this->net = net;
	
	// Start fresh
	qreal y = 0;
	clear();	
    std::map<std::string,Population>::iterator i;
    for (i = net->populations.begin(); i != net->populations.end(); ++i )
    {
        NetworkItem *ni = new NetworkItem(QString(i->second.ID.c_str()), QString(i->second.name.c_str()), QString(i->second.model_type.c_str()));		
		scene->addItem(ni);
		ni->setPos(0, y);
		y += 80;

        /*
        QObject::connect( ni, SIGNAL(selected()), 
                         this, SLOT(loadNetwork())
                        );
                        */
    }   
    
	scale(qreal(1), qreal(1));	

	// Update the view
	update();
}

void NetworkView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(painter)	
    Q_UNUSED(rect)	
}

/*
 * clear() function taken from:
 * http://lists.trolltech.com/qt-interest/2007-11/thread00667-0.html
 */
void NetworkView::clear()
{
    QList<QGraphicsItem*> scene_items = scene->items();
    QList<QGraphicsItem*> remove_items;

    for (int i = 0; i < scene_items.size(); ++i)
    {
            /*
                    only [re]move top level items.. all others are 
                    being taken care of by their respective parent
            */
            if (scene_items[i]->parentItem() == 0)
            {
                    scene->removeItem(scene_items[i]);
                    remove_items << scene_items[i];
            }
    }
    /*
            finally delete them
    */
    for (int i = 0; i < remove_items.size();++i)
    {
                    delete remove_items[i];
    }
}
