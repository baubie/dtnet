#ifndef GDTNET_H
#define GDTNET_H

#include <wx/wx.h>
#include "lib/mathplot/mathplot.h"
#include "../libdtnet/libdtnet.h"

class gdtnet : public wxApp {
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    mpWindow *m_plot;

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

enum {
    ID_Quit = 1,
    ID_About,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Quit, MyFrame::OnQuit)
EVT_MENU(ID_About, MyFrame::OnAbout)
END_EVENT_TABLE()

#endif
