
#include "mainwindow.h"

MainWindow::MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *) NULL, -1, title, pos, size) {

    this->InitMenu();

     // notify wxAUI which frame to use
     m_mgr.SetManagedWindow(this);

	NetworkPanel *networkPanel = new NetworkPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    wxPanel *inputPanel = new wxPanel(this);
    wxPanel *resultsPanel = new wxPanel(this);
    wxPanel *propertiesPanel = new wxPanel(this);
  
    m_mgr.AddPane(propertiesPanel, wxRIGHT, wxT("Properties"));
    m_mgr.AddPane(resultsPanel, wxBOTTOM, wxT("Simulation Results"));
	m_mgr.AddPane(inputPanel, wxBOTTOM, wxT("Network Inputs"));
	m_mgr.AddPane(networkPanel, wxCENTER);

	m_mgr.Update();

    CreateStatusBar();
    wxString version((dtnet::version()).c_str(), wxConvUTF8);
    SetStatusText(version);

}

MainWindow::~MainWindow() {
	// deinitialize the frame manager
	m_mgr.UnInit();
}

void MainWindow::InitMenu() {
    wxMenu *menuFile = new wxMenu;
    wxMenu *menuHelp = new wxMenu;

    menuFile->Append(ID_Quit, _T("&Quit"));
    menuHelp->Append(ID_About, _T("&About..."));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(menuHelp, _T("&Help"));

    SetMenuBar(menuBar);
}

void MainWindow::OnQuit(wxCommandEvent& WXUNUSED(event)) {
    Close(TRUE);
}

void MainWindow::OnAbout(wxCommandEvent& WXUNUSED(event)) {
    wxMessageBox(_T("This is a wxWidgets Hello world sample"),
            _T("About dtnet"), wxOK | wxICON_INFORMATION, this);
}

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
EVT_MENU(ID_Quit, MainWindow::OnQuit)
EVT_MENU(ID_About, MainWindow::OnAbout)
END_EVENT_TABLE()
