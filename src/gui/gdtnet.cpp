
#include "gdtnet.h"

IMPLEMENT_APP(gdtnet);

bool gdtnet::OnInit() {
    MyFrame *frame = new MyFrame(_T("dtnet"), wxPoint(50, 50), wxSize(450, 340));
    frame->Show(TRUE);
    SetTopWindow(frame);
    return TRUE;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *) NULL, -1, title, pos, size) {

    wxMenu *menuFile = new wxMenu;
    wxMenu *menuHelp = new wxMenu;

    menuFile->Append(ID_Quit, _T("&Quit"));
    menuHelp->Append(ID_About, _T("&About..."));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(menuHelp, _T("&Help"));

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText(_T(dtnet::version()));
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
    wxMessageBox(_T("This is a wxWidgets Hello world sample"),
            _T("About dtnet"), wxOK | wxICON_INFORMATION, this);
}
