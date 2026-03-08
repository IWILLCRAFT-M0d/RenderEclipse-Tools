#include <wx/wx.h>
#include "wxwindow.h"

wxIMPLEMENT_APP(App);

bool App::OnInit() {
    MainFrame* mainWin = new MainFrame("Climax ARC Manager");
    mainWin->SetIcon(wxICON(PROGRAM_ICON));
    mainWin->Show();
    mainWin->Maximize(true);
    return true;
}
