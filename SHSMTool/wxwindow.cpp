#include "wxwindow.h"
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/string.h>
#include <wx/mstream.h>
#include <wx/utils.h>
#include <wx/aboutdlg.h>

#include <vector>
#include <sstream>
#include <filesystem>
#include "ARC.h"
#include "reth.h"
#include <zlib.h>

using namespace std;
using namespace std::filesystem;

MainFrame::MainFrame(const wxString& titleBar): wxFrame(nullptr, wxID_ANY, titleBar) {
    wxMenuBar* cMenuBarMain = new wxMenuBar();
    wxMenu* cMenuBar0 = new wxMenu();
    wxMenu* cMenuBar1 = new wxMenu();
    cMenuBarMain->Append(cMenuBar0, _("&File"));
    cMenuBarMain->Append(cMenuBar1, _("&About"));
    cMenuBar0->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::menuBarOpenARC, this, cMenuBar0->Append(wxID_ANY, _("&Load file"))->GetId());
    cMenuBar0->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::menuBarClose, this, cMenuBar0->Append(wxID_ANY, _("&Close"))->GetId());
    cMenuBar1->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::menuBarCredits, this, cMenuBar1->Append(wxID_ANY, _("&Credits"))->GetId());

    mainSizer->Add(panel0, 1, wxEXPAND | wxALL, 10);
    this->SetSizerAndFit(mainSizer);
    panel0->SetSizerAndFit(treeListSizer);
    treeListSizer->Add(fileListTree, 1, wxEXPAND | wxALL);
    
    SetMenuBar(cMenuBarMain);
    CreateStatusBar();
    
    fileListTree->Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &MainFrame::ShowContextMenu, this);
}

void MainFrame::menuBarClose(wxCommandEvent& evt) {
    Close(true);
    return;
}

void MainFrame::menuBarCredits(wxCommandEvent& evt) {
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("RenderEclipse Extractor");
    aboutInfo.SetVersion("V1");
    aboutInfo.SetDescription(_("A extration tool for Climax's Silent Hill games"));
    aboutInfo.SetWebSite("https://github.com/IWILLCRAFT-M0d/RenderEclipse-Tools");
    aboutInfo.AddDeveloper("IWILLCRAFT (Creator)");
    aboutInfo.AddDeveloper("Ikskoks (SHSM Hashing Help)");
    aboutInfo.AddDeveloper("TPU (Original SHSM Hashing finder)");
    aboutInfo.AddDeveloper("Rheini and AlphaTwentyThree (SHO/SHSM ARC)");
    aboutInfo.AddDeveloper("Special thanks for XeNTaX users who contributed to many SHO/SHSM researches");
    wxAboutBox(aboutInfo);
    return;
}

//I do this so I can pass the val to other funcs
wxString fileItemSelected;
string pathFileLoaded;
vector<vector<unsigned long>> ARCdata;
vector<pair<unsigned long, string>> RETH;
unsigned short fileNameTest;
unsigned long rethSize;

void MainFrame::menuBarOpenARC(wxCommandEvent& evt) {
    wxFileDialog fileARC(this, "Open ARC file", "", "", "ARC file (*.ARC)|*.ARC", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (fileARC.ShowModal() == wxID_CANCEL) { wxLogStatus("No file has been open"); return; }
    if (!fileListTree->IsEmpty()) { fileListTree->Delete(fileListTree->GetRootItem()); }
    pathFileLoaded = fileARC.GetPath();
    wxTreeItemId rootListTree = fileListTree->AddRoot(path(fileARC.GetPath().ToStdString()).filename().string());
    ARCdata = readARC(fileARC.GetPath().ToStdString());
    switch (ARCType) {
    case 1:
        RETH = readNames("./hashes.reth", {});
        rethSize = RETH.size();
        /*
        * 0 = reth empty
        * 1 = reth not empty, but there are files that has not been unhashed
        * 2 = reth not empty and all file names has been unhashed
    
        * This checks if there is a reth file, if not it will make the
        * reader use the hashes names, if there is a reth file it will
        * check if there is any file name that is not unhashed so it
        * will properly split it in two roots
        */

        if (rethSize > 0) {
            long i = 0, j = 0;
            while (true) {
                if (ARCdata[i][0] == RETH[j].first) {
                    fileNameTest = 2;
                    j = 0;
                    i++;
                }
                else if (j == rethSize - 1 && i <= ARCdata.size()) {
                    fileNameTest = 1;
                    break;
                }
                else if (i + 1 == ARCdata.size()) {
                    break;
                }
                else {
                    j++;
                }
            }
        }
        else {
            fileNameTest = 0;
        }

        if (fileNameTest == 1) {
            wxTreeItemId fileNames = fileListTree->AppendItem(rootListTree, "Files with names");
            wxTreeItemId hashNames = fileListTree->AppendItem(rootListTree, "Files without names");
            for (unsigned long i = 0; i < ARCdata.size(); i++) {
                for (unsigned long j = 0; j < rethSize; j++) {
                    if (ARCdata[i][0] == RETH[j].first) {
                        fileListTree->AppendItem(fileNames, RETH[j].second);
                        break;
                    } else if (j == rethSize-1) {
                        fileListTree->AppendItem(hashNames, endianChangeString(ARCdata[i][0]));
                    }
                }
            }
            fileListTree->Expand(fileNames);
            fileListTree->Expand(hashNames);
        } else {
            for (long i = 0; i < ARCdata.size(); i++) {
                if (fileNameTest == 0) {
                    fileListTree->AppendItem(rootListTree, endianChangeString(ARCdata[i][0]));
                } else {
                    for (unsigned long j = 0; j < rethSize; j++) {
                        if (ARCdata[i][0] == RETH[j].first) {
                            fileListTree->AppendItem(rootListTree, RETH[j].second);
                            break;
                        }
                    }
                }
            }
        }
        fileListTree->Expand(rootListTree);
        if (fileNameTest == 0) {
            wxMessageDialog* noRETHError = new wxMessageDialog(NULL, wxT("No unhashed name has been detected!\nAll files has their hashed names."), wxT("Error"), wxOK | wxICON_ERROR);
            noRETHError->ShowModal();
        }
        wxLogStatus("Open file is from Shattered Memories");
        break;
    case 2:
    case 3:
        RETH = readNames(pathFileLoaded, ARCdata);
        rethSize = RETH.size();
        for (unsigned long i = 0; i < rethSize; i++) {
            fileListTree->AppendItem(rootListTree, RETH[i].second);
        }
        if (ARCType == 2) {
            wxLogStatus("Open file is from Origins (Climax UK)");
        } else {
            wxLogStatus("Open file is from Origins (Climax LA)");
        }
        fileListTree->Expand(rootListTree);
        break;
    }
    return;
}

void MainFrame::ShowContextMenu(wxTreeEvent& evt) {
    fileItemSelected = fileListTree->GetItemText(evt.GetItem());
    contextMenu->Remove(Call_Export);
    contextMenu->Remove(Call_ExportAll);
    if (fileItemSelected != "Files with names"
        && fileItemSelected != "Files without names"
        && fileItemSelected != fileListTree->GetItemText((fileListTree->GetRootItem()))) {
        Call_Export = contextMenu->Append(wxID_ANY, "&Export");
        //I have seen that connect is no longer used and replaced with Bind() but for some
        //reason Bind() doesn't for this.
        Connect(Call_Export->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ContextMenu_Export), NULL, this);
    } else if (fileItemSelected == fileListTree->GetItemText((fileListTree->GetRootItem()))) {
        Call_ExportAll = contextMenu->Append(wxID_ANY, "&Export all files");
        Connect(Call_ExportAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::ContextMenu_ExportAll), NULL, this);
    }
    PopupMenu(this->contextMenu);
}

void MainFrame::ContextMenu_Export(wxCommandEvent& evt) {
    char* rawData;
    char* unCompData;
    string fileName = fileItemSelected.ToStdString();
    unsigned long unHashedStringFileName2Int = endianChangeULong(fileName);
    unsigned long hashedStringFileName2Int = SHSMWord2Hash(fileItemSelected.ToStdString());
    ifstream ARC(pathFileLoaded, ios::in | ios_base::binary);
    if (fileNameTest == 0) {}
    for (unsigned long i = 0; i < ARCdata.size(); i++) {
        if (ARCType == 1 && (hashedStringFileName2Int == ARCdata[i][0]
            || unHashedStringFileName2Int == ARCdata[i][0])) {
            rawData = new char[ARCdata[i][2]];
            ARC.seekg(ARCdata[i][1], ios::beg);
            ARC.read(rawData, ARCdata[i][2]);
            if ((unHashedStringFileName2Int == ARCdata[i][0]) && ARCType == 1) {
                fileName += ".dat";
            }
            ofstream fileExt("./" + fileName, ios::out | ios::binary | ios::trunc);
            if (ARCdata[i][3] > 0) {
                unCompData = new char[ARCdata[i][3]];
                uncompress((Bytef*)unCompData, &ARCdata[i][3], (Bytef*)rawData, ARCdata[i][2]);
                fileExt.write((char*)&unCompData[0], ARCdata[i][3]);
                delete[] unCompData;
            } else {
                fileExt.write((char*)&rawData[0], ARCdata[i][2]);
            }
            delete[] rawData;
            fileExt.close();
            break;
        } else if (ARCType == 2 || ARCType == 3) {
            if (fileName == RETH[i].second) {
                rawData = new char[ARCdata[i][2]];
                ARC.seekg(ARCdata[i][1], ios::beg);
                ARC.read(rawData, ARCdata[i][2]);
                ofstream fileExt("./" + fileName, ios::out | ios::binary | ios::trunc);
                if (ARCdata[i][3] > 0) {
                    unCompData = new char[ARCdata[i][3]];
                    uncompress((Bytef*)unCompData, &ARCdata[i][3], (Bytef*)rawData, ARCdata[i][2]);
                    fileExt.write((char*)&unCompData[0], ARCdata[i][3]);
                    delete[] unCompData;
                }
                else {
                    fileExt.write((char*)&rawData[0], ARCdata[i][2]);
                }
                delete[] rawData;
                fileExt.close();
                break;
            }
        }
    }
}

void MainFrame::ContextMenu_ExportAll(wxCommandEvent& evt) {
    string extractPath = "./" + path(pathFileLoaded).stem().string();
    if (!is_directory(extractPath) || !exists(extractPath)) { create_directory(extractPath); }
    char* rawData;
    char* realData;
    bool nameFinded;
    bool rethEmpty = false;
    string fileName;
    if (RETH.size() == 0) { rethEmpty = true; }
    ifstream ARC(pathFileLoaded.c_str(), ios::in | ios_base::binary);
    if (!ARC.is_open()) { printf("Missing file ARC!\n"); ARC.close(); return; }
    for (unsigned long i = 0; i < ARCdata.size(); i++) {
        rawData = new char[ARCdata[i][2]];
        ARC.seekg(ARCdata[i][1], ios::beg);
        ARC.read(rawData, ARCdata[i][2]);
        nameFinded = false;
        if (!rethEmpty) {
            for (unsigned long j = 0; j < RETH.size(); j++) {
                if (ARCdata[i][0] == RETH[j].first) {
                    cout << "Extracting file number: " << i + 1 << " | File name: " << RETH[j].second << endl;
                    fileName = "/" + RETH[j].second;
                    nameFinded = true;
                    break;
                }
            }
        }
        
        if (ARCType == 2 || ARCType == 3) {
            fileName = "/" + RETH[i].second;
            nameFinded = true;
        }
        if (!nameFinded) {
            fileName = "/Unknown Names/" + endianChangeString(ARCdata[i][0]) + ".dat";
            if (!is_directory(extractPath + "/Unknown Names") || !exists(extractPath + "/Unknown Names")) { create_directory(extractPath + "/Unknown Names"); }
            cout << "Extracting file number: " << i + 1 << " | File name has not been found" << endl;
        }
        ofstream fileExt(extractPath + fileName, ios::out | ios::binary | ios::trunc);
        if (ARCdata[i][3] > 0) {
            realData = new char[ARCdata[i][3]];
            uncompress((Bytef*)realData, &ARCdata[i][3], (Bytef*)rawData, ARCdata[i][2]);
            fileExt.write((char*)&realData[0], ARCdata[i][3]);
            delete[] realData;
        }
        else {
            fileExt.write((char*)&rawData[0], ARCdata[i][2]);
        }
        delete[] rawData;
        fileExt.close();
    }
    return;
}