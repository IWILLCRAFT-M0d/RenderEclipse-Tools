#include <wx/wx.h>
#include "wxwindow.h"
wxIMPLEMENT_APP(App);

bool App::OnInit() {
    MainFrame* mainWin = new MainFrame("RenderEclipse Tools");
    mainWin->SetIcon(wxIcon(wxT("sprog.ico"), wxBITMAP_TYPE_ICO));
    mainWin->Show();
    mainWin->Maximize(true);
    return true;
}