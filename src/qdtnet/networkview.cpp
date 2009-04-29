
#include "networkview.h"


NetworkView::NetworkView(Net *net)
{
	this->net = net;	
	scene = new QGraphicsScene(this);	
	setScene(scene);
	setRenderHint(QPainter::Antialiasing);

}

void NetworkView::replaceNetwork(Net *net)
{
	this->net = net;
	
	// Start fresh
	clear();	
	NetworkItem *ni = new NetworkItem(tr("ON"), tr("Onset Evoked"));		
	scene->addItem(ni);
	ni->setPos(0, 0);
	scale(qreal(1), qreal(1));	

	// Update the view
	update();
}



void NetworkView::drawBackground(QPainter *painter, const QRectF &rect)
{
	
}

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
