#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../libdtnet/libdtnet.h"
#include <QMainWindow>
#include <QTreeView>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTabWidget>
#include <QDebug>
#include <QImage>
#include <QLabel>

#include "propmodel.h"
#include "networkview.h"

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
    void on_actionOpen_Trial_triggered();
    void on_actionRun_Simulation_triggered();
    void on_actionProperties_Window_triggered(bool checked);
    void on_dockProperties_visibilityChanged(bool visible);
    void loadNetwork();

private:
    
    // Widgets
    Ui::MainWindowClass *ui;
    QTreeView *propertiesTree;    
    PropModel *propertiesModel;
	QTabWidget *tabWidget;
	NetworkView *networkView;
    QImage *voltageImage;
    QLabel *voltageLabel;
	int networkTab;
	int rtvoltageTab;
    
    // State variables
    QString networkFilename;
    QString trialFilename;
    Net net;
	Trial trial;
    Results result;
};

#endif // MAINWINDOW_H
