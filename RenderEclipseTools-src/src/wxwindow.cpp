#include "wxwindow.h"
#include <wx/string.h>
#include <wx/mstream.h>
#include <wx/utils.h>
#include <wx/aboutdlg.h>

#include <sstream>
#include <filesystem>

#include "ARC.h"
#include "reth.h"
#include "fileLoader.h"
#include <zlib.h>

using namespace std;
using namespace std::filesystem;

MainFrame::MainFrame(const wxString& titleBar): wxFrame(nullptr, wxID_ANY, titleBar) {
    wxLog* logger = new wxLogWindow(this, "Test", true, false);
    wxLog::SetActiveTarget(logger);
    wxMenuBar* cMenuBarMain = new wxMenuBar();
    wxMenu* cMenuBar0 = new wxMenu();
    wxMenu* cMenuBar1 = new wxMenu();
    cMenuBarMain->Append(cMenuBar0, _("&File"));
    cMenuBarMain->Append(cMenuBar1, _("&About"));
    cMenuBar0->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::menuBarOpenARC, this, cMenuBar0->Append(wxID_ANY, _("&Load file"))->GetId());
    cMenuBar0->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::menuBarClose, this, cMenuBar0->Append(wxID_ANY, _("&Close"))->GetId());
    cMenuBar1->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::menuBarCredits, this, cMenuBar1->Append(wxID_ANY, _("&Credits"))->GetId());
    mainSizer->Add(panel0, 3, wxEXPAND | wxALL, 10);
    mainSizer->Add(panel1, 5, wxEXPAND | wxALL, 10);
    this->SetSizerAndFit(mainSizer);
    panel0->SetSizerAndFit(treeListSizer);
    panel1->SetSizerAndFit(infoFileSizer);
    treeListSizer->Add(fileListTree, 1, wxEXPAND | wxALL);
    
    SetMenuBar(cMenuBarMain);
    CreateStatusBar();
    
    fileListTree->Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &MainFrame::ShowContextMenu, this);
}

void MainFrame::menuBarOpenARC(wxCommandEvent& evt) {
    wxFileDialog fileARC(this, "Open ARC file", "", "", "ARC file (*.ARC)|*.ARC", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (fileARC.ShowModal() == wxID_CANCEL) { wxLogStatus("No file has been open"); return; }
    if (!fileListTree->IsEmpty()) { fileListTree->Delete(fileListTree->GetRootItem()); }
    valuesARC.pathFileLoaded = fileARC.GetPath();
    wxTreeItemId rootListTree = fileListTree->AddRoot(path(fileARC.GetPath().ToStdString()).filename().string());
    valuesARC.ARCdata = ARC::readARC(fileARC.GetPath().ToStdString());
    switch (ARC::ARCType) {
    case 1:
        wxLogStatus("Open file is from Shattered Memories");
        valuesARC.RETH = RETH::readNames("./hashes.reth", {});
        valuesARC.rethSize = valuesARC.RETH.size();
        /*
        0 = reth empty/dont exist
        1 = reth not empty, but there are files that has not been unhashed
        2 = reth not empty and all file names has been unhashed
    
        This checks if there is a reth file, if not it will make the
        reader use the hashes names, if there is a reth file it will
        check if there is any file name that is not unhashed so it
        will properly split it in two roots
        */

        if (valuesARC.rethSize > 0) {
            long i = 0, j = 0;
            while (true) {
                if (valuesARC.ARCdata[i][0] == valuesARC.RETH[j].first) {
                    valuesARC.fileNameTest = 2;
                    j = 0;
                    i++;
                }
                else if (j == valuesARC.rethSize - 1 && i <= valuesARC.ARCdata.size()) {
                    valuesARC.fileNameTest = 1;
                    break;
                }
                else if (i + 1 == valuesARC.ARCdata.size()) {
                    break;
                }
                else {
                    j++;
                }
            }
        }
        else {
            valuesARC.fileNameTest = 0;
        }

        for (long i = 0; i < valuesARC.ARCdata.size(); i++) {
            if (valuesARC.fileNameTest == 0) {
                fileListTree->AppendItem(rootListTree, ARC::endianChangeString(valuesARC.ARCdata[i][0]));
            }
            else {
                for (unsigned long j = 0; j < valuesARC.rethSize; j++) {
                    if (valuesARC.ARCdata[i][0] == valuesARC.RETH[j].first) {
                        fileListTree->AppendItem(rootListTree, valuesARC.RETH[j].second);
                        break;
                    }
                    else if (j == valuesARC.rethSize - 1) {
                        fileListTree->AppendItem(rootListTree, ARC::endianChangeString(valuesARC.ARCdata[i][0]));
                    }
                }
            }
        }

        fileListTree->Expand(rootListTree);
        wxMessageDialog* noRETHError;
        if (valuesARC.fileNameTest == 0) {
            noRETHError = new wxMessageDialog(NULL, wxT("No unhashed name has been detected!\nAll files has their hashed names."), wxT("Error"), wxOK | wxICON_ERROR);
            noRETHError->ShowModal();
        } else if (valuesARC.fileNameTest == 1) {
            noRETHError = new wxMessageDialog(NULL, wxT("Not all files has unhashed name.\nSome files has their hashed names."), wxT("Error"), wxOK | wxICON_ERROR);
            noRETHError->ShowModal();
        }
        break;
    default:
        if (ARC::ARCType == 2) {
            wxLogStatus("Open file is from Origins (Climax UK)");
        } else {
            wxLogStatus("Open file is from Origins (Climax LA)");
        }
        valuesARC.RETH = RETH::readNames(valuesARC.pathFileLoaded, valuesARC.ARCdata);
        valuesARC.rethSize = valuesARC.RETH.size();
        for (unsigned long i = 0; i < valuesARC.rethSize; i++) {
            fileListTree->AppendItem(rootListTree, valuesARC.RETH[i].second);
        }
        fileListTree->Expand(rootListTree);
        break;
    }
    return;
}

void MainFrame::ShowContextMenu(wxTreeEvent& evt) {
    valuesARC.fileItemSelected = fileListTree->GetItemText(evt.GetItem());
    contextMenu->Remove(Call_Export);
    contextMenu->Remove(Call_ExportAll);
    contextMenu->Remove(Call_Import);
    contextMenu->Remove(Call_LoadFile);
    
    wxString selectedParent = fileListTree->GetItemText((fileListTree->GetRootItem()));
    if (valuesARC.fileItemSelected != selectedParent) {
        Call_Export = contextMenu->Append(wxID_ANY, "&Export");
        Call_Import = contextMenu->Append(wxID_ANY, "&Import");
        Call_LoadFile = contextMenu->Append(wxID_ANY, "&Load File");
        //I have seen that connect is no longer used and replaced with Bind() but for some
        //reason Bind() doesn't work for this case.
        Connect(Call_Export->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ContextMenu_Export), NULL, this);
        Connect(Call_Import->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ContextMenu_Import), NULL, this);
        Connect(Call_LoadFile->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ContextMenu_LoadFile), NULL, this);
    } else if (valuesARC.fileItemSelected == selectedParent) {
        Call_ExportAll = contextMenu->Append(wxID_ANY, "&Export all files");
        Connect(Call_ExportAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ContextMenu_ExportAll), NULL, this);
    }
    PopupMenu(this->contextMenu);
}

void MainFrame::ContextMenu_Export(wxCommandEvent& evt) {
    ARC::extractFunc(false);
}

void MainFrame::ContextMenu_ExportAll(wxCommandEvent& evt) {
    ARC::extractFunc(true);
}

void MainFrame::ContextMenu_Import(wxCommandEvent& evt) {
    wxFileDialog fileImport(this, "Select file to import", "", "", "", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (fileImport.ShowModal() == wxID_CANCEL) { wxLogStatus("No file has been open"); return; }
    
    ARC::importFunc(fileImport.GetPath().ToStdString());
}

void MainFrame::ContextMenu_LoadFile(wxCommandEvent& evt) {
    // wxLogStatus("Opened a file");
    delete[] RWSFileListTree;
    char *rawData, *data;
    bool rethEmpty = false;
    unsigned long hashedStringFileName2Int, unHashedStringFileName2Int, fileIndex, fileInfo_Header, pos = 0;
    string extractPath, fileName = valuesARC.fileItemSelected.ToStdString();
    

    unHashedStringFileName2Int = ARC::endianChangeULong(fileName);
    hashedStringFileName2Int = RETH::SHSMWord2Hash(valuesARC.fileItemSelected.ToStdString());
    
    
    if (valuesARC.RETH.size() == 0) { rethEmpty = true; }
    ifstream ARC(valuesARC.pathFileLoaded, ios::in | ios_base::binary);

    for (unsigned long i = 0; i < valuesARC.ARCdata.size(); i++) {
        if (ARC::ARCType == 1) {
            if (unHashedStringFileName2Int == valuesARC.ARCdata[i][0]) {
                fileIndex = i;
                    break;
            } else if (hashedStringFileName2Int == valuesARC.ARCdata[i][0]) {
                fileIndex = i;
                    break;
            }
        } else if (fileName == valuesARC.RETH[i].second) {
            fileIndex = i;
                break;
        }
    }

    // if (rethEmpty == true || fileName == valuesARC.RETH[fileIndex].second) {}
    if (valuesARC.ARCdata[fileIndex][3] != 0 && ARC::ARCType != 3) {
        rawData = new char[valuesARC.ARCdata[fileIndex][2]];
        ARC.seekg(valuesARC.ARCdata[fileIndex][1], ios::beg);
        ARC.read(rawData, valuesARC.ARCdata[fileIndex][2]);
        data = new char[valuesARC.ARCdata[fileIndex][3]];
        uncompress((Bytef*)data, &valuesARC.ARCdata[fileIndex][3],
        (Bytef*)rawData, valuesARC.ARCdata[fileIndex][2]);
        delete[] rawData;
    } else {
        data = new char[valuesARC.ARCdata[fileIndex][2]];
        ARC.seekg(valuesARC.ARCdata[fileIndex][1], ios::beg);
        ARC.read(data, valuesARC.ARCdata[fileIndex][2]);
    }


fileHeader_Analysis:
    fileInfo_Header = fileLoader::char2Long(data+pos);
    switch (fileInfo_Header) {
        case 2: //String table
            wxLogStatus("String table opened");
            break;
        case 1814: //RenderWare Stream
        case 1820:
            wxLogStatus("RWS opened");
            RWSFileListTree = new wxTreeCtrl(panel1);
            infoFileSizer->Add(RWSFileListTree, 1, wxEXPAND | wxALL);
            break;
        default:
            if (ARC::ARCType == 3 && pos != 192) {
                pos = 192;
                goto fileHeader_Analysis;
            } else if (ARC::ARCType == 1) {
                fileInfo_Header = fileLoader::char2Long(data+8);
                if (fileInfo_Header == 1180189254) {
                    wxLogStatus("XAML/XML SHSM Encripted opened");
                    break;
                }
            }
            wxLogStatus("%i", fileInfo_Header);
            break;
    }
    delete[] data;
}

void MainFrame::menuBarClose(wxCommandEvent& evt) {
    Close(true);
    return;
}

void MainFrame::menuBarCredits(wxCommandEvent& evt) {
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("RenderEclipse Tools");
    aboutInfo.SetVersion("V2");
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