#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../libdtnet/libdtnet.h"
#include <QMainWindow>
#include <QTreeView>
#include <QTableView>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTabWidget>
#include <QDebug>
#include <QImage>
#include <QLabel>
#include <QToolBox>

#include "propmodel.h"
#include "networkview.h"
#include "workspace.h"

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
    
private slots:
    void on_actionQuit_triggered();    
    void on_actionOpen_Network_triggered();    
    void on_actionOpen_Trial_triggered();
    void on_actionRun_Simulation_triggered();
    void on_actionProperties_Window_triggered(bool checked);
    void on_actionOpen_Workspace_triggered();
    void on_actionSave_Workspace_triggered();
    void on_dockProperties_visibilityChanged(bool visible);

    void loadNetwork();
    void loadTrial();

private:
    
    // Widgets
    Ui::MainWindowClass *ui;

    PropModel *propertiesModel;
	QTabWidget *tabWidget;
    QToolBox *networkToolBox;
	NetworkView *networkView;
    QImage *voltageImage;
    QLabel *voltageLabel;
	int networkTab;
	int rtvoltageTab;
    
    // State variables
    Workspace ws;
};

#endif // MAINWINDOW_H
