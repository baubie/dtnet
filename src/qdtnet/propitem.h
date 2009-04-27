#ifndef PROPITEM_H
#define PROPITEM_H

#include <QList>
#include <QVariant>
#include <QDebug>

class PropItem
{
public:

    PropItem(const QString &name, const QVariant &value, PropItem *parent = 0);
    ~PropItem();
    
    void appendChild(PropItem *child);
    
    PropItem *child(int row);
    int childCount() const;
    int columnCount() const;
    int row() const;
    PropItem *parent();
    bool setData(int column, const QVariant &value);
    
    QString name() const;
    QVariant value();
    
    
private:
    QList<PropItem*> childItems;
    PropItem *parentItem;
    bool itemIsText;

    QString itemName;
    QVariant itemValue;
};

#endif
