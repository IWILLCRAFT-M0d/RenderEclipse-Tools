#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>

class MainFrame : public wxFrame {
public:
	wxBoxSizer* m_mainsizerHor;
	wxPanel* panel0 = new wxPanel(this);
	// wxPanel* panel1 = new wxPanel(this);
	wxMenu* contextMenu = new wxMenu();
	wxTreeCtrl* fileListTree = new wxTreeCtrl(panel0);
	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* treeListSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* infoFileSizer = new wxBoxSizer(wxBOTH);
	wxMenuItem* Call_Export;
	wxMenuItem* Call_ExportAll;
	wxMenuItem* Call_Import;
	MainFrame(const wxString& titleBar);
	// ~MainFrame() {
	// 	wxLog::SetActiveTarget(nullptr);
	// 	delete logger;
	// }
	//Events
	void menuBarOpenARC(wxCommandEvent& evt);
	void menuBarClose(wxCommandEvent& evt);
	void menuBarCredits(wxCommandEvent& evt);
	void ShowContextMenu(wxTreeEvent& evt);
	void ContextMenu_Export(wxCommandEvent& evt);
	void ContextMenu_ExportAll(wxCommandEvent& evt);
	void ContextMenu_Import(wxCommandEvent& evt);
// private:
	// wxLog *logger;
};

class App : public wxApp {
public:
	bool OnInit();
};

wxDECLARE_APP(App);