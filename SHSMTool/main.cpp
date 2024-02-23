#include <wx/wx.h>
#include "wxwindow.h"
wxIMPLEMENT_APP(App);

bool App::OnInit() {
    mainWin->SetIcon(wxIcon(wxT("sprog.ico"), wxBITMAP_TYPE_ICO));
    mainWin->Show();
    mainWin->Maximize(true);
    return true;
}