#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);
    
    ui->statusBar->showMessage(QString((dtnet::version()).c_str()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
