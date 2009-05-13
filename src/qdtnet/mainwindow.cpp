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
    
	
	tabWidget = new QTabWidget();
	networkView = new NetworkView( NULL );
    
	networkTab = tabWidget->addTab(networkView, QString("Network View"));
    
    voltageLabel = new QLabel;   
    rtvoltageTab = tabWidget->addTab(voltageLabel, QString("Real-Time Voltage"));
	
	setCentralWidget(tabWidget);
	
    propertiesModel = new PropModel( NULL );
    propertiesTree->setModel(propertiesModel);
    
    // Setup Signals and Slots
    QObject::connect(&ws, SIGNAL(networkChanged()), 
                     this, SLOT(loadNetwork())
                    );
                    
    // We can't run simulations by default
    ui->actionRun_Simulation->setEnabled( false );
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
    setWindowTitle("Qt-dtnet - " + ws.networkFilename);
    propertiesModel = new PropModel( ws.net );    
    propertiesTree->setModel(propertiesModel);
	
	networkView->replaceNetwork( ws.net );
	networkView->show();
    
}

void MainWindow::on_actionOpen_Network_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Network"), "", tr("Network XML Files (*.xml)"));

    QString error;
    if (!ws.loadNetwork(filename, error))
    {
        QMessageBox msgBox;    
        msgBox.setText(error);
        msgBox.exec();
    } 
    else {
        if ( ws.trial->isReady() && ws.net->isReady() ) ui->actionRun_Simulation->setEnabled( true );   
    }
}

void MainWindow::on_actionOpen_Trial_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Trial"), "", tr("Trial XML Files (*.xml)"));

    QString error;
    if (!ws.loadTrial(filename, error))
    {
        QMessageBox msgBox;    
        msgBox.setText(error);
        msgBox.exec();
    } 
    else {
        if ( ws.trial->isReady() && ws.net->isReady() ) ui->actionRun_Simulation->setEnabled( true );   
    }
}

void MainWindow::on_actionRun_Simulation_triggered()
{
    
    voltageLabel->setPixmap(QPixmap::fromImage(QImage(QString::fromUtf8(":/newPrefix/resources/homer_running.jpg"))));
    
    ui->statusBar->showMessage(tr("Running Simulation..."));    
    Simulation sim(*ws.net, *ws.trial);  
    std::string filename = "";   
    dtnet::set("T", 100);
    dtnet::set("dt", 0.05);     
    dtnet::run(*ws.result, sim, filename, 1, 0, true);

    ui->statusBar->showMessage(tr("Generating Voltage Plot..."));    
    dtnet::set("graph.height", 20);
    dtnet::set("graph.width", 15);     
    dtnet::graphtrial(dtnet::PLOT_VOLTAGE, *ws.result, 0, "temp.png");

    ui->statusBar->showMessage(tr("Done!"));    
    QImage image("temp.png");
    if (image.isNull()) {
    QMessageBox::information(this, tr("Voltage Traces"), tr("Error loading voltage traces"));
    return;
    }
    voltageLabel->setPixmap(QPixmap::fromImage(image));
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
