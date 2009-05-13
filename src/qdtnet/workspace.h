#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "../libdtnet/libdtnet.h"
#include <QString>
#include <QDataStream>
#include <QMap>
#include <QFile>

class Workspace : public QObject
{
    Q_OBJECT

public:

    Workspace();
    ~Workspace();

    bool loadTrial(QString const &filename, QString &e);
    bool loadNetwork(QString const &filename, QString &e);

    bool save(QString const &filename);
    bool load(QString const &filename);

    QString networkFilename;
    QString trialFilename;
    Net *net;
	Trial *trial;
    Results *result;   

signals:
    void networkChanged();
    void trialChanged();
     
};

#endif
