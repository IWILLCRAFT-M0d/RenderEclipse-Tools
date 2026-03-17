#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>


class MainFrame : public wxFrame {
public:
	wxBoxSizer* m_mainsizerHor;
	wxPanel* panel0 = new wxPanel(this);
	wxPanel* panel1 = new wxPanel(this);
	wxTreeCtrl* fileListTree = new wxTreeCtrl(panel0);
	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* treeListSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* infoFileSizer = new wxBoxSizer(wxHORIZONTAL);
    wxMessageDialog* errorMessage;
	MainFrame(const wxString& titleBar);
#ifdef DEBOOG
	~MainFrame() {
		wxLog::SetActiveTarget(nullptr);
		delete logger;
	}
#endif // DEBOOG
	//Events
	void MenuBar_OpenARC(wxCommandEvent& evt);
	void MenuBar_Close(wxCommandEvent& evt);
	void MenuBar_Credits(wxCommandEvent& evt);
	void ContextMenu_Show(wxTreeEvent& evt);
	void ContextMenu_Export(wxCommandEvent& evt);
	void ContextMenu_ExportAll(wxCommandEvent& evt);
	void ContextMenu_Import(wxCommandEvent& evt);
	void ContextMenu_LoadFile(wxCommandEvent& evt);
#ifdef DEBOOG
private:
	wxLog *logger;
#endif // DEBOOG
};

class App : public wxApp {
public:
	bool OnInit();
};

extern MainFrame* mainWin;

wxDECLARE_APP(App);