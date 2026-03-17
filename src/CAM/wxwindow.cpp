#include "wxwindow.h"
#include <wx/string.h>
#include <wx/mstream.h>
#include <wx/utils.h>
#include <wx/aboutdlg.h>
#include <zlib.h>

#include <sstream>
#include <filesystem>

#include "ARC.h"
#include "fileLoader.h"

ARC::loadedInfo ARC::loadedARC_Info;

/** @brief Main function. */

MainFrame::MainFrame(const wxString& titleBar): wxFrame(nullptr, wxID_ANY, titleBar) {
#ifdef DEBOOG
    wxLog* logger = new wxLogWindow(this, "Test", true, false);
    wxLog::SetActiveTarget(logger);
#endif // DEBOOG

    wxMenuBar* menuBar_Main = new wxMenuBar();
    wxMenu* menuBar_Opt_File = new wxMenu();
    wxMenu* menuBar_Opt_Abt = new wxMenu();

    menuBar_Main->Append(menuBar_Opt_File, _("&File"));
    menuBar_Main->Append(menuBar_Opt_Abt, _("&About"));

    menuBar_Opt_File->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::MenuBar_OpenARC, this, menuBar_Opt_File->Append(wxID_ANY, _("&Load file"))->GetId());
    menuBar_Opt_File->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::MenuBar_Close, this, menuBar_Opt_File->Append(wxID_ANY, _("&Close"))->GetId());
    menuBar_Opt_Abt->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::MenuBar_Credits, this, menuBar_Opt_Abt->Append(wxID_ANY, _("&Credits"))->GetId());
    
    mainSizer->Add(panel0, 3, wxEXPAND | wxALL, 10);
    mainSizer->Add(panel1, 5, wxEXPAND | wxALL, 10);
    this->SetSizerAndFit(mainSizer);
    panel0->SetSizerAndFit(treeListSizer);
    panel1->SetSizerAndFit(infoFileSizer);
    treeListSizer->Add(fileListTree, 1, wxEXPAND | wxALL);
    
    SetMenuBar(menuBar_Main);
    CreateStatusBar();
    
    fileListTree->wxEvtHandler::Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &MainFrame::ContextMenu_Show, this);
}

/** @brief Context menu functions. */

void MainFrame::ContextMenu_Show(wxTreeEvent& evt) {
	wxMenu* contextMenu = new wxMenu();

    ARC::loadedARC_Info.fileItemSelected = fileListTree->GetItemText(evt.GetItem());
    
    wxString selectedParent = fileListTree->GetItemText((fileListTree->GetRootItem()));
    if (ARC::loadedARC_Info.fileItemSelected != selectedParent) {
        contextMenu->Append(0, "&Export");
        contextMenu->Append(1, "&Import");
#ifdef DEBOOG
        contextMenu->Append(2, "&Load File");
#endif // DEBOOG
        wxEvtHandler::Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ContextMenu_Export, this, 0);
        wxEvtHandler::Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ContextMenu_Import, this, 1);
#ifdef DEBOOG
        wxEvtHandler::Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ContextMenu_LoadFile, this, 2);
#endif // DEBOOG
    } else if (ARC::loadedARC_Info.fileItemSelected == selectedParent) { // <- Don't turn into `else`.
        contextMenu->Append(3, "&Export all files");
        wxEvtHandler::Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::ContextMenu_ExportAll, this, 3);
    }

    wxWindow::PopupMenu(contextMenu);
    delete contextMenu;
    ARC::loadedARC_Info.fileItemSelected.clear();
}

void MainFrame::ContextMenu_Export(wxCommandEvent& evt) {
    ARC::exportFile(false);
}

void MainFrame::ContextMenu_ExportAll(wxCommandEvent& evt) {
    ARC::exportFile(true);
}

void MainFrame::ContextMenu_Import(wxCommandEvent& evt) {
    wxFileDialog fileImport(this, "Select file to import", "", "", "", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (fileImport.ShowModal() == wxID_CANCEL) { wxLogStatus("No file has been open"); return; }
    
    ARC::importFile(fileImport.GetPath().ToStdString());
}

void MainFrame::ContextMenu_LoadFile(wxCommandEvent& evt) {
    fileLoader::Loadfile();
}

/** @brief Menu bar functions. */

void MainFrame::MenuBar_OpenARC(wxCommandEvent& evt) {
    enum hashesInfoStatus {
        HIS_Null     = 0, // Missing RETH file/No recognized filenames detected.
        HIS_Missing  = 1, // Filenames recognized, but no the totallity from the ARC.
        HIS_Complete = 2  // All filenames recognized.
    };

    unsigned long    fileNameCount = 0;
    unsigned long    fileCount     = 0;
    hashesInfoStatus HIS           = HIS_Null;

    wxFileDialog fileARC(this, "Open ARC file", "", "", "ARC file (*.ARC)|*.ARC", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (fileARC.ShowModal() == wxID_CANCEL) { wxLogStatus("No file has been open"); return; }

    ARC::loadedARC_Info.pathFileLoaded = fileARC.GetPath().ToStdString();

    if (ARC::read() == false) {
        ARC::loadedARC_Info.pathFileLoaded.clear();
        errorMessage = new wxMessageDialog(NULL, wxT("Invalid *.ARC file"), wxT("Error"), wxOK | wxICON_ERROR);
        errorMessage->ShowModal();
        delete errorMessage;
        return;
    }

    if (!fileListTree->IsEmpty()) { fileListTree->Delete(fileListTree->GetRootItem()); }

    wxTreeItemId rootListTree = fileListTree->AddRoot(std::filesystem::path(ARC::loadedARC_Info.pathFileLoaded).filename().string());

    /** Store and display filenames.
     * Dependant on if the file is from Shattered Memories or not
     * it will use or not the RETH file.
     * 
     * In Shattered Memories case:
     * Checks if there is a reth file, if not it will make the
     * reader use the hashes names, if there is a reth file it will
     * check if there is any file name that is not unhashed so it
     * will properly split it in two roots
     */

    switch (ARC::loadedARC_Info.type) {
    case ARC::type_SM:
        wxLogStatus("Open file is from Shattered Memories");
        ARC::readFilenames("./hashes.reth");

        fileNameCount = ARC::loadedARC_Info.fileNames.size();
        fileCount     = ARC::loadedARC_Info.fileData.size();

        if (fileNameCount > 0) {
            long i = 0, j = 0;
            while (true) {
                if (ARC::loadedARC_Info.fileData[i].fileName == ARC::loadedARC_Info.fileNames[j].hash) {
                    HIS = HIS_Complete;
                    j = 0;
                    i++;
                }
                else if (j == fileNameCount - 1 && i <= fileCount) {
                    HIS = HIS_Missing;
                    break;
                }
                else if (i + 1 == ARC::loadedARC_Info.fileData.size()) {
                    break;
                }
                else {
                    j++;
                }
            }
        }

        for (long i = 0; i < fileCount; i++) {
            if (HIS == HIS_Null) {
                fileListTree->AppendItem(rootListTree, ARC::endianChangeString(ARC::loadedARC_Info.fileData[i].fileName));
            }
            else {
                for (unsigned long j = 0; j < fileNameCount; j++) {
                    if (ARC::loadedARC_Info.fileData[i].fileName == ARC::loadedARC_Info.fileNames[j].hash) {
                        fileListTree->AppendItem(rootListTree, ARC::loadedARC_Info.fileNames[j].filename);
                        break;
                    }
                    else if (j == fileNameCount - 1) {
                        fileListTree->AppendItem(rootListTree, ARC::endianChangeString(ARC::loadedARC_Info.fileData[i].fileName));
                    }
                }
            }
        }

        fileListTree->Expand(rootListTree);
        if (HIS == HIS_Null) {
            errorMessage = new wxMessageDialog(NULL, wxT("No unhashed name has been detected!\nAll files has their hashed names."), wxT("Error"), wxOK | wxICON_ERROR);
            errorMessage->ShowModal();
            delete errorMessage;
        } else if (HIS == HIS_Missing) {
            errorMessage = new wxMessageDialog(NULL, wxT("Not all files has unhashed name.\nSome files has their hashed names."), wxT("Error"), wxOK | wxICON_ERROR);
            errorMessage->ShowModal();
            delete errorMessage;
        }
        break;
    default:
        if (ARC::loadedARC_Info.type == ARC::type_Solent) {
            wxLogStatus("Open file is from Origins (Climax UK)");
        } else {
            wxLogStatus("Open file is from Origins (Climax LA)");
        }
        ARC::readFilenames(ARC::loadedARC_Info.pathFileLoaded);
        fileNameCount = ARC::loadedARC_Info.fileNames.size();
        for (unsigned long i = 0; i < fileNameCount; i++) {
            fileListTree->AppendItem(rootListTree, ARC::loadedARC_Info.fileNames[i].filename);
        }
        fileListTree->Expand(rootListTree);
        break;
    }
    return;
}

void MainFrame::MenuBar_Close(wxCommandEvent& evt) {
    Close(true);
    return;
}

void MainFrame::MenuBar_Credits(wxCommandEvent& evt) {
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("Climax ARC Manager");
    aboutInfo.SetVersion("V2.1");
    aboutInfo.SetDescription(_("A modding tool for Climax's Silent Hill games"));
    aboutInfo.SetWebSite("https://github.com/IWILLCRAFT-M0d/RenderEclipse-Tools");
    aboutInfo.AddDeveloper("IWILLCRAFT (Creator)");
    aboutInfo.AddDeveloper("Ikskoks (SHSM Hashing Help)");
    aboutInfo.AddDeveloper("TPU (Original SHSM Hashing finder)");
    aboutInfo.AddDeveloper("Rheini and AlphaTwentyThree (SHO/SHSM ARC)");
    aboutInfo.AddDeveloper("Special thanks for XeNTaX users who contributed to many SHO/SHSM researches");
    wxAboutBox(aboutInfo);
    return;
}
