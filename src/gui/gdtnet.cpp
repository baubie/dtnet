
#include "gdtnet.h"

IMPLEMENT_APP(gdtnet);

bool gdtnet::OnInit() {
    MainWindow *mainWindow = new MainWindow(_T("dtnet GUI"), wxPoint(50, 50), wxSize(750, 550));
    mainWindow->Show(TRUE);
    SetTopWindow(mainWindow);
    return TRUE;
}

