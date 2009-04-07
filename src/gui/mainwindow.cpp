
#include "mainwindow.h"

MainWindow::MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *) NULL, -1, title, pos, size) {

    this->InitMenu();

    wxNotebook *toolsNB = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

    wxPanel *status = new wxPanel(toolsNB);
    
    toolsNB->AddPage(status, "Network", true);
    toolsNB->AddPage(status, "Input", false);
    toolsNB->AddPage(status, "Results", false);

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    sizer->Add( toolsNB, 1, wxGROW | wxALL, 0 );

    SetSizer (sizer);
   // sizer->SetSizeHints ( this );

    CreateStatusBar();
    wxString version((dtnet::version()).c_str(), wxConvUTF8);
    SetStatusText(version);
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
