
#include "propitem.h"

PropItem::PropItem(const QString &name, const QVariant &value, PropItem *parent) 
{
    itemName = name;
    itemValue = value;
    parentItem = parent;
}

PropItem::~PropItem()
{
    qDeleteAll(childItems);
}

bool PropItem::setData(int column, const QVariant &value)
{   
    // We can only edit the right hand column
    if (column != 1) return false;
    itemValue = value;
    return true;
}

void PropItem::appendChild(PropItem *child)
{
    childItems.append(child);
}

PropItem *PropItem::child(int row)
{
    return childItems.value(row);
}

int PropItem::childCount() const
{
    return childItems.count();
}

int PropItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<PropItem*>(this));
    return 0;
}

int PropItem::columnCount() const
{
    return 2;
}

PropItem *PropItem::parent()
{
    return parentItem;
}

QString PropItem::name() const
{
    return itemName;
}

QVariant PropItem::value()
{
    return itemValue;
}
