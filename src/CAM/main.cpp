#include <wx/wx.h>
#include "wxwindow.h"

wxIMPLEMENT_APP(App);

MainFrame* mainWin;

bool App::OnInit() {
    mainWin = new MainFrame("Climax ARC Manager");
    mainWin->SetIcon(wxICON(PROGRAM_ICON));
    mainWin->Show();
    mainWin->Maximize(true);
    return true;
}
