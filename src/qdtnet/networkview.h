 #ifndef NETWORKVIEW_H
 #define NETWORKVIEW_H

 #include "../libdtnet/libdtnet.h"
 #include <QtGui/QGraphicsView>
 #include <QGraphicsScene> 
 #include "networkitem.h"

 class NetworkView : public QGraphicsView
 {
     Q_OBJECT

 public:
     NetworkView(Net *net);	 
     void itemMoved();
	 void replaceNetwork(Net *net);

 protected:
	void drawBackground(QPainter *painter, const QRectF &rect);
	void clear();
	Net *net;	
	QGraphicsScene *scene;
 };

 #endif
