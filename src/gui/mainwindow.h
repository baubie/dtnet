#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif

#include "wx/aui/aui.h"

#include "../libdtnet/libdtnet.h"
#include <string>
#include "network.h"

class MainWindow : public wxFrame {
public:

    MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MainWindow();
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnResize(wxCommandEvent& event);
    DECLARE_EVENT_TABLE()

private:
	wxAuiManager m_mgr;
    void InitMenu();
};

enum {
    ID_Quit = 1,
    ID_About,
};



#endif
