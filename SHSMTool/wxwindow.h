#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>

class MainFrame : public wxFrame {
public:
	wxBoxSizer* m_mainsizerHor;
	wxPanel* panel0 = new wxPanel(this);
	wxMenu* contextMenu = new wxMenu();
	wxTreeCtrl* fileListTree = new wxTreeCtrl(panel0);
	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* treeListSizer = new wxBoxSizer(wxHORIZONTAL);
	wxMenuItem* Call_Export;
	wxMenuItem* Call_ExportAll;
	MainFrame(const wxString& titleBar);
private:
	//Events
	void menuBarOpenARC(wxCommandEvent& evt);
	void menuBarClose(wxCommandEvent& evt);
	void menuBarCredits(wxCommandEvent& evt);
	void ShowContextMenu(wxTreeEvent& evt);
	void ContextMenu_Export(wxCommandEvent& evt);
	void ContextMenu_ExportAll(wxCommandEvent& evt);
};

class App : public wxApp {
public:
	/*
	MainFrame* mainWin = new MainFrame("RenderEclipse Tools");
	The original purpose of the tool was going to be a toolset for
	extracting and modding data for Silent Hill: Shattered Memories.
	I (IWILLCRAFT) created this tool to add Heavy from Team Fortress 2
	to Shattered Memories and do beta research for Shattered Memories too,
	but due lack of skills in both reverse engeneering, coding and
	personal life, this toolset has been reduced to only a file
	extractor for file containers from Climax's Silent Hill games.
	*/
	MainFrame* mainWin = new MainFrame("RenderEclipse Extractor");
	bool OnInit();
};

wxDECLARE_APP(App);