#ifndef GDTNET_H
#define GDTNET_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif

#include "mainwindow.h"

class gdtnet : public wxApp {
    virtual bool OnInit();
};

#endif
