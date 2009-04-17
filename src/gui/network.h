#ifndef NETWORK_H
#define NETWORK_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif

#include "wx/dc.h"

#include "../libdtnet/libdtnet.h"

class NetworkPanel : public wxPanel {
public:

    NetworkPanel(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size);
    void OnPaint(wxPaintEvent& event);
    DECLARE_EVENT_TABLE()

private:

};

class PopulationDetails : public wxPanel {
public:

		PopulationDetails(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize &size);
	
};

#endif
