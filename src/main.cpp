#include "MainFrame.h"
#include <wx/wx.h>

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        MainFrame* frame = new MainFrame("文件批量重命名");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);