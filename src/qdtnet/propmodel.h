#ifndef PROPMODEL_H
#define PROPMODEL_H

#include "../libdtnet/libdtnet.h"
#include <QAbstractItemModel>
#include <QVariant>
#include <QDebug>

#include "propitem.h"

class PropModel : public QAbstractItemModel
{
    Q_OBJECT
    
public:
    PropModel( Population *p, QObject *parent = 0 );
    ~PropModel();
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int selection, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    void setupModelData(Population *p, PropItem *parent);
    PropItem *getItem(const QModelIndex &index) const;    
    PropItem *rootItem;
    
};

#endif
