#include "wx/wx.h"
#include "wx/graphics.h"

class MyFrame:public wxFrame
{
public:
    MyFrame(wxWindow *parent,
		    const wxWindowID id,
		    const wxString& title,
		    const wxPoint& pos,
		    const wxSize& size,
		    const long style)
	  : wxFrame(parent, id, title, pos, size,
			      style|wxFRAME_SHAPED)
    {
      
       wxGraphicsPath path = wxGraphicsRenderer::GetDefaultRenderer()->CreatePath();
       path.AddEllipse(0, 0,GetRect().width-20,GetRect().height/2);
       SetShape(path);
    };

    ~MyFrame() 
    {};
};


class MyApp:public wxApp
{
public:
  bool OnInit()
  {
      if ( !wxApp::OnInit() )
	  return false;

      // Create the main window
      MyFrame* frame = new MyFrame((wxFrame *)NULL, wxID_ANY, "",
				   wxDefaultPosition, wxSize(500, 400),
				   wxNO_BORDER);
      frame->Show(true);

      return true;
  };
};   
IMPLEMENT_APP(MyApp)
