
#include "workspace.h"

Workspace::Workspace() {
    net = new Net();
    trial = new Trial();
    result = new Results();    
}

Workspace::~Workspace() {
    delete net;
    delete trial;
    delete result;
}

QString Workspace::getError() {
    return errorMsg;
}

bool Workspace::loadNetwork(QString const &filename) {

    std::string error;
    if (!net->load(filename.toStdString(), error))
    {
        errorMsg = QString::fromStdString(error);
        return false;
    } else {
        networkFilename = filename;
        emit networkChanged();
        return true;
    }

}

bool Workspace::loadTrial(QString const &filename) {

    std::string error;
    if (!trial->load(filename.toStdString(), error))
    {
        errorMsg = QString::fromStdString(error);
        return false;
    } else {
        trialFilename = filename;
        emit trialChanged();
        return true;
    }

}

bool Workspace::save(QString const &filename) {

    QMap<QString, QString> p;   
    p["networkFilename"] = networkFilename;
    p["trialFilename"] = trialFilename;
    
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    
    // Write a header with a magic number and version
    out << (quint32)0xBA012345;
    out << (qint32)1;
    out.setVersion(QDataStream::Qt_4_4);

    // Write out the map
    out << p;

    return true;
}


bool Workspace::load(QString const &filename) {
    QMap<QString, QString> p;   
    
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    // Read and check the header
    quint32 magic;
    in >> magic;
    if (magic != 0xBA012345) { return false; }

    // Read the version
    qint32 version;
    in >> version;    
    in.setVersion(QDataStream::Qt_4_4);
    
    // Read the data
    in >> p;

    // Update the workspace
    networkFilename = p.value("networkFilename", ""); 
    trialFilename = p.value("trialFilename", "");
    if (networkFilename != "") loadNetwork(networkFilename);
    if (trialFilename != "") loadTrial(trialFilename);
    
    return true;
}
