
#include "propmodel.h"

PropModel::PropModel( Population *p, QObject *parent)
{
    Q_UNUSED(parent) 

    rootItem = new PropItem("Property", "Value");    
    if (p != NULL) setupModelData(p, rootItem);
}

PropModel::~PropModel()
{
    delete rootItem;
}

void PropModel::setupModelData(Population *p, PropItem *parent)
{
    Q_UNUSED(parent)

    std::map<std::string,Population>::iterator i;
    std::map<std::string,Range>::iterator v;
    
    PropItem *pop = rootItem; //new PropItem(QString(i->second.name.c_str()), QString(i->second.model_type.c_str()), rootItem);
    
    for (v = p->params.vals.begin(); v != p->params.vals.end(); ++v)
    {
        QString value = QString("%1").arg(v->second.front());

        if (v->second.front() != v->second.back())
        {
            value += "..." + QString("%1").arg(v->second.back());
        }
        PropItem *prop = new PropItem(QString(v->first.c_str()), value, pop);            
        PropItem *start = new PropItem(QString("Start"), v->second.front(), prop);
        PropItem *end = new PropItem(QString("End"), v->second.back(), prop);
        
        double stepValue = 0;
        if (v->second.size() > 1) stepValue = v->second.values[1] - v->second.values[0];
        PropItem *step = new PropItem(QString("Step"), stepValue, prop);
        
        prop->appendChild(start);
        prop->appendChild(end);
        prop->appendChild(step);
        pop->appendChild(prop);                            
    }        
    
    // Add in default parameters
    std::map<std::string, double> piter = dtnet::defaultModelParams(p->model_type);
    std::map<std::string, double>::iterator v_iter;
    for (v_iter = piter.begin(); v_iter != piter.end(); ++v_iter) {
        if (p->params.vals.find(v_iter->first) == p->params.vals.end()) {
            PropItem *prop = new PropItem(QString(v_iter->first.c_str()), v_iter->second, pop);                            
            PropItem *start = new PropItem(QString("Start"), v_iter->second, prop);
            PropItem *end = new PropItem(QString("End"), v_iter->second, prop);
            PropItem *step = new PropItem(QString("Step"), 0, prop);
            prop->appendChild(start);
            prop->appendChild(end);
            prop->appendChild(step);
            pop->appendChild(prop);      
        }
    }                
//    rootItem->appendChild(pop);
}

PropItem *PropModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        PropItem *item = static_cast<PropItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem;
}

bool PropModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) return false;
    
    PropItem *item = getItem(index);
    bool result = item->setData(index.column(), value);
    
    // if (result) emit dataChanged(index, index);
    
    return result;
}


QModelIndex PropModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
        
    PropItem *parentItem;
    
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<PropItem*>(parent.internalPointer());
        
    PropItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex PropModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    
    PropItem *childItem = static_cast<PropItem*>(index.internalPointer());
    PropItem *parentItem = childItem->parent();
    
    if (parentItem == rootItem)
        return QModelIndex();
    
    return createIndex(parentItem->row(), 0, parentItem);
}

int PropModel::rowCount(const QModelIndex &parent) const
{
    PropItem *parentItem;
    if (parent.column() > 0)
        return 0;
    
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<PropItem*>(parent.internalPointer());
        
    return parentItem->childCount();
}

int PropModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<PropItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant PropModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
        
    if (role != Qt::DisplayRole)
        return QVariant();
    
    PropItem *item = static_cast<PropItem*>(index.internalPointer());
    
    switch(index.column())
    {
        case 0:
            return item->name();
            break;
        case 1:
            return item->value();
            break;
    }
    return QVariant();
}

Qt::ItemFlags PropModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    
    if (index.column() == 1)
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant PropModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
            case 0:
                return "Property";
                break;
            case 1:
                return "Value";
                break;
        }
    }
        
    return QVariant();
}
