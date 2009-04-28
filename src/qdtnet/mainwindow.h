#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../libdtnet/libdtnet.h"
#include <QMainWindow>
#include <QTreeView>
#include <QFileDialog>
#include <QMessageBox>

#include <QGraphicsScene>

#include <QGraphicsItem>

#include "propmodel.h"

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
signals:
    void networkChanged();
    
private slots:
    void on_actionQuit_triggered();    
    void on_actionOpen_Network_triggered();    
    void on_actionProperties_Window_triggered(bool checked);
    void on_dockProperties_visibilityChanged(bool visible);
    void loadNetwork();

private:
    // Functions
    void drawNetwork();
    
    // Widgets
    Ui::MainWindowClass *ui;
    QTreeView *propertiesTree;    
    PropModel *propertiesModel;
    
    // State variables
    QString networkFilename;
    Net net;
};

#endif // MAINWINDOW_H
