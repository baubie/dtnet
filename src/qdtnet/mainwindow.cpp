#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);    
    ui->statusBar->showMessage(QString::fromStdString(dtnet::version()));
    
    ui->dockProperties->setObjectName("NetworkProperties");
    ui->dockProperties->setWindowTitle(tr("Network Properties"));
    
    propertiesTree = new QTreeView();
    propertiesTree->setAlternatingRowColors(true);
    propertiesTree->setSelectionBehavior( QAbstractItemView::SelectRows );
    propertiesTree->setWordWrap( false );
    propertiesTree->setAnimated( false );
    ui->dockProperties->setWidget(propertiesTree);
    
    propertiesModel = new PropModel( NULL );
    propertiesTree->setModel(propertiesModel);
    
    // Setup Signals and Slots
    QObject::connect(this, SIGNAL(networkChanged()), 
                     this, SLOT(loadNetwork())
                    );
}

MainWindow::~MainWindow()
{
    delete ui;
    delete propertiesTree;
    delete propertiesModel;
}

void MainWindow::loadNetwork() 
{
    delete propertiesModel;
    setWindowTitle("Qt-dtnet - " + this->networkFilename);
    propertiesModel = new PropModel( &net );    
    propertiesTree->setModel(propertiesModel);
}

void MainWindow::drawNetwork()
{
    
}

void MainWindow::on_actionOpen_Network_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Network"), "", tr("Network XML Files (*.xml)"));

    std::string error;

    if (!net.load(filename.toStdString(), error))
    {
        QMessageBox msgBox;    
        msgBox.setText(QString::fromStdString(error));
        msgBox.exec();
    } else {
        this->networkFilename = filename;
        emit networkChanged();
    }
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_actionProperties_Window_triggered(bool checked)
{
    if (checked)
    {
        ui->dockProperties->show();
    } else {
        ui->dockProperties->hide();
    }
}

void MainWindow::on_dockProperties_visibilityChanged(bool visible)
{
    ui->actionProperties_Window->setChecked(visible);
}
