
#include "network.h"

NetworkPanel::NetworkPanel(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, id, pos, size)
{
	
}

void NetworkPanel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	
	dc.SetPen(*wxBLACK_PEN);
	dc.SetBrush(*wxRED_BRUSH);
	
	wxSize sz = GetClientSize();
	
	wxCoord w = 100, h = 50;
	
	int x = wxMax(0, (sz.x-w)/2);
	int y = wxMax(0, (sz.y-h)/2);
	
	wxRect rectToDraw(x,y,w,h);
	
	if (IsExposed(rectToDraw)) {
		dc.DrawRectangle(rectToDraw);
		dc.DrawText(_T("Poisson"), x, y);
	}
}

BEGIN_EVENT_TABLE(NetworkPanel, wxPanel)
	EVT_PAINT(NetworkPanel::OnPaint)
END_EVENT_TABLE()
