#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <Windows.h>

#include <wx/string.h>
#include <wx/mstream.h>
#include <wx/utils.h>
#include <wx/aboutdlg.h>
#include <zlib.h>


#include "wxwindow.h"
#include "fileLoader.h"
#include "ARC.h"

using namespace std;
using namespace std::filesystem;

fileLoader::file_DataInfo fileLoader::data_info;
fileLoader::fileInfoPanel fileLoader::filePanel;
bool fileLoader::fileLoaded;

unsigned long fileLoader::changeEndian(unsigned long value, bool change) {
    if (change == 0) {
        return (((value << 24) & 0xff000000)
            | ((value << 8) & 0xff0000)
            | ((value >> 8) & 0xff00)
            | ((value >> 24) & 0xff));
    } else {
        return value;
    }
}

unsigned long fileLoader::char2Long(char* dataPos) {
    char tempChar[4];
    memcpy(tempChar, dataPos, 4);
    return *(unsigned long*)tempChar;
}

void fileLoader::UnloadFile(void) {
    if (fileLoader::filePanel.RWSFileListTree) {
        fileLoader::filePanel.RWSFileListTree->Destroy();
    }
    delete[] fileLoader::data_info.fileData;
    fileLoader::fileLoaded = false;
    return;
}

void fileLoader::Loadfile(void) {
    if (fileLoader::fileLoaded) {
        fileLoader::UnloadFile();
    }
    char *compData;
    bool rethEmpty = false, invalidFile = false;
    unsigned long hashedStringFileName2Int, unHashedStringFileName2Int, fileIndex, fileInfo_Header, i, pos = 0;
    std::string extractPath, fileName = ARC::loadedARC_Info.fileItemSelected.ToStdString();
    

    unHashedStringFileName2Int = ARC::endianChangeULong(fileName);
    hashedStringFileName2Int = ARC::SHSM_FilenameHash(ARC::loadedARC_Info.fileItemSelected.ToStdString());
    
    
    if (ARC::loadedARC_Info.fileNames.size() == 0) { rethEmpty = true; }
    std::ifstream ARCFile(ARC::loadedARC_Info.pathFileLoaded, std::ios::in | std::ios_base::binary);

    for (i = 0; i < ARC::loadedARC_Info.fileData.size(); i++) {
        if (ARC::loadedARC_Info.type == ARC::type_SM) {
            if (unHashedStringFileName2Int == ARC::loadedARC_Info.fileData[i].fileName) {
                fileIndex = i;
                break;
            } else if (hashedStringFileName2Int == ARC::loadedARC_Info.fileData[i].fileName) {
                fileIndex = i;
                break;
            }
        } else if (fileName == ARC::loadedARC_Info.fileNames[i].filename) {
            fileIndex = i;
            break;
        }
    }

    // if (rethEmpty == true || fileName == ARC::loadedARC_Info.fileNames[fileIndex].filename) {}
    if (ARC::loadedARC_Info.fileData[fileIndex].dataSizeReal != 0 && ARC_IsNotType(ARC::type_LA)) {
        compData = new char[ARC::loadedARC_Info.fileData[fileIndex].dataSize];
        ARCFile.seekg(ARC::loadedARC_Info.fileData[fileIndex].dataPos, std::ios::beg);
        ARCFile.read(compData, ARC::loadedARC_Info.fileData[fileIndex].dataSize);
        fileLoader::data_info.fileData = new char[ARC::loadedARC_Info.fileData[fileIndex].dataSizeReal];
        uncompress((Bytef*)fileLoader::data_info.fileData, &ARC::loadedARC_Info.fileData[fileIndex].dataSizeReal,
        (Bytef*)compData, ARC::loadedARC_Info.fileData[fileIndex].dataSize);
        delete[] compData;
        fileLoader::data_info.fileSize = ARC::loadedARC_Info.fileData[fileIndex].dataSizeReal;
    } else {
        fileLoader::data_info.fileData = new char[ARC::loadedARC_Info.fileData[fileIndex].dataSize];
        ARCFile.seekg(ARC::loadedARC_Info.fileData[fileIndex].dataPos, std::ios::beg);
        ARCFile.read(fileLoader::data_info.fileData, ARC::loadedARC_Info.fileData[fileIndex].dataSize);
        fileLoader::data_info.fileSize = ARC::loadedARC_Info.fileData[fileIndex].dataSize;
    }


fileHeader_Analysis:
    fileInfo_Header = fileLoader::char2Long(fileLoader::data_info.fileData+pos);
    switch (fileInfo_Header) {
        case FS_C_STR_TABLE: //String table
            wxLogStatus("String table opened");
            break;
        case FS_RW_RWS1: //RenderWare Stream
        case FS_RW_RWS2:
            wxLogStatus("RWS opened");
            fileLoader::RW::RWStreamFile();
            break;
        case FS_G_JPG:
            wxLogStatus("JPG image opened");
            break;
        default:
            switch (ARC::loadedARC_Info.type) {
                case ARC::type_LA:
                    if (pos < 192) {
                        pos = 192;
                        goto fileHeader_Analysis;
                    }
                    break;
                case ARC::type_SM:
                    fileInfo_Header = fileLoader::char2Long(fileLoader::data_info.fileData+8);
                    if (fileInfo_Header == FS_G_XAML) {
                        wxLogStatus("XAML/XML SHSM Encripted opened");
                        break;
                    }
                    break;
            }
            invalidFile = true;
            break;
    }
    
    if (invalidFile) {
        mainWin->errorMessage = new wxMessageDialog(NULL, wxT("Unrecognized file format."), wxT("Error"), wxOK | wxICON_ERROR);
        mainWin->errorMessage->ShowModal();
        delete mainWin->errorMessage;
    }
}

void fileLoader::RW::RWStreamFile(void) {
    using namespace fileLoader;
    string fileName;
    bool BigEndian = false;
    unsigned long pos = 0;
    RW::RwHeader data;
    fileLoaded = true;
    /*
    filePanel.RWSFileListTree = new wxTreeCtrl(mainWin->panel1);
    mainWin->infoFileSizer->Add(filePanel.RWSFileListTree, 1, wxEXPAND | wxALL);
    mainWin->panel1->Refresh();
    mainFileName = ARC::loadedARC_Info.fileItemSelected.ToStdString();
    do {
        fileName = "";
        memcpy((char*)&data, data_info.fileData+pos, sizeof(RW::RwHeader)); pos += sizeof(RW::RwHeader);
        chunkSel++;
        if (data.ID == 1814) {
            unsigned long fileInfoHeader = char2Long(data_info.fileData+pos); pos += 4;
            if (fileInfoHeader < data.chunkSize) {
                BigEndian = true;
            }
            unsigned long fileNameSize = changeEndian(char2Long(data_info.fileData+pos), BigEndian);
            if (fileNameSize > 4) {
                for(int i=4; data_info.fileData[pos+i] != '\0'; i++) {
                    fileName += data_info.fileData[pos+i];
                    if (i >= fileNameSize+4) { break; }
                }
                cout << "- File name: " << fileName << endl;
            } else {
                long unsigned fileTypeRead = changeEndian(char2Long(data_info.fileData+pos+24), BigEndian);
                for(int i=28; data_info.fileData[pos+i] != '\0'; i++) {
                    fileName += data_info.fileData[pos+i];
                    if (i >= fileTypeRead+28) { break; }
                }
                cout << "- RW Id: " << fileName << endl;
                if (fileName == "rwID_TEXDICTIONARY") {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".txd";
                } else if (fileName == "rwID_AUDIODATA") {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".snd";
                } else if (fileName == "rwID_AUDIOCUES") {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".cue";
                } else if (fileName == "rwID_STATETRANSITION") {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".rst";
                } else if (fileName == "rwID_HANIMANIMATION") {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".anm";
                } else if (fileName == "rwID_SPLINE") {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".spl";
                } else if (fileName == "rwID_RWS" || fileName == "rwpID_BODYDEF") {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".rws";
                } else if (fileName == "rwID_WORLD") {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".bsp";
                } else if (fileName == "rwID_CLUMP") {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".dff";
                } else if (fileName == "rwID_DMORPHANMSTREAM") {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".dma";
                } else {
                    fileName = mainFileName + "_" + to_string(chunkSel) + ".bin";
                }
            }
            filePanel.RWSFileListTree->AddRoot(fileName);
            pos += changeEndian(fileInfo, BigEndian);
            long unsigned fileSizeNonChunk = changeEndian(char2Long(data_info.fileData+pos), BigEndian);
            // cout << "File Size (no chunk) " << fileSizeNonChunk << endl;
            pos += 4;
            pos += fileSizeNonChunk;
            // cout << "Actual position: " << pos << " | Full chunk size: " << chunkSize+12 <<endl;
        } else {
            filePanel.RWSFileListTree->AddRoot(mainFileName + "_" + to_string(chunkSel) + ".bin");
            pos += data.chunkSize;
        }
        cout << "Chunk has been ended | Actual Position: " << pos << " | File size: " << fullFileSize <<endl;
    } while (pos < data_info.fileSize);
    */
    //char tempChar[4];
    //char* fileData_OG;
    //unsigned long header, chunkSize, chunkSel = 0, pos = 0;
    //string fileName;
    //bool BigEndian = false;
    //struct RWStruct {
    //    unsigned long ID;
    //    unsigned long chunkSize;
    //    unsigned long RWVersion;
    //} sRWStruct;
    //
    //do {
    //    BigEndian = false;
    //    fileName = "";
    //    memcpy((char*)&sRWStruct, fileData+pos, 12); pos += 12;
    //    chunkSel++;
    //    if (sRWStruct.ID == 1814) {
    //        unsigned long fileInfoHeader = fileLoader::char2Long(fileData+pos); pos += 4;
    //        if (fileInfoHeader < sRWStruct.chunkSize) {
    //            BigEndian = true;
    //        }
    //        unsigned long fileNameSize = fileLoader::changeEndian(fileLoader::char2Long(fileData+pos), BigEndian);
    //        if (fileNameSize > 4) {
    //            for(int i=4; fileData[pos+i] != '\0'; i++) {
    //                fileName += fileData[pos+i];
    //                if (i >= fileNameSize+4) { break; }
    //            }
    //            cout << "- File name: " << fileName << endl;
    //        } else {
    //            long unsigned fileTypeRead = changeEndian(char2Long(fileData+pos+24), BigEndian);
    //            for(int i=28; fileData[pos+i] != '\0'; i++) {
    //                fileName += fileData[pos+i];
    //                if (i >= fileTypeRead+28) { break; }
    //            }
    //            cout << "- RW Id: " << fileName << endl;
    //            if (fileName == "rwID_TEXDICTIONARY") {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".txd";
    //            } else if (fileName == "rwID_AUDIODATA") {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".snd";
    //            } else if (fileName == "rwID_AUDIOCUES") {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".cue";
    //            } else if (fileName == "rwID_STATETRANSITION") {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".rst";
    //            } else if (fileName == "rwID_HANIMANIMATION") {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".anm";
    //            } else if (fileName == "rwID_SPLINE") {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".spl";
    //            } else if (fileName == "rwID_RWS" || fileName == "rwpID_BODYDEF") {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".rws";
    //            } else if (fileName == "rwID_WORLD") {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".bsp";
    //            } else if (fileName == "rwID_CLUMP") {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".dff";
    //            } else if (fileName == "rwID_DMORPHANMSTREAM") {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".dma";
    //            } else {
    //                fileName = mainFileName + "_" + to_string(chunkSel) + ".bin";
    //            }
    //        }
    //        pos += changeEndian(fileInfo, BigEndian);
    //        long unsigned fileSizeNonChunk = changeEndian(char2Long(fileData+pos), BigEndian);
    //        // cout << "File Size (no chunk) " << fileSizeNonChunk << endl;
    //        fileData_OG = new char[fileSizeNonChunk];
    //        pos += 4;
    //        memcpy(fileData_OG, fileData+pos, fileSizeNonChunk);
    //        ofstream fileExt("./Extracted Data/" + fileName, ios::out | ios::binary | ios::trunc);
    //        fileExt.write(fileData_OG, fileSizeNonChunk);
    //        fileExt.close();
    //        delete[] fileData_OG;
    //        pos += fileSizeNonChunk;
    //        if (SHSMData == 1) { while (pos % 4) {pos += 1;} }
    //        // cout << "Actual position: " << pos << " | Full chunk size: " << chunkSize+12 <<endl;
    //    } else {
    //        fileData_OG = new char[chunkSize];
    //        memcpy(fileData_OG, fileData+pos, chunkSize);
    //        ofstream fileExt("./Extracted Data/" + mainFileName + "_" + to_string(chunkSel) + ".bin", ios::out | ios::binary | ios::trunc);
    //        fileExt.write(fileData_OG, chunkSize);
    //        fileExt.close();
    //        delete[] fileData_OG;
    //        pos += chunkSize;
    //    }
    //    // cout << "Chunk has been ended | Actual Position: " << pos << " | File size: " << fullFileSize <<endl;
    //} while (pos < fullFileSize);
}

// void fileLoader::RenderWare::RWStreamFile (char* fileData, unsigned long fullFileSize) {
//     char tempChar[4];
//     char* fileData_OG;
//     unsigned long header, chunkSize, chunkSel = 0, pos = 0;
//     string fileName;
//     bool BigEndian = false;
//     struct RWStruct {
//         unsigned long ID;
//         unsigned long chunkSize;
//         unsigned long RWVersion;
//     } sRWStruct;

//     do {
//         BigEndian = false;
//         fileName = "";
//         memcpy((char*)&sRWStruct, fileData+pos, 12); pos += 12;
//         chunkSel++;

//         if (sRWStruct.ID == 1814) {
//             unsigned long fileInfoHeader = fileLoader::char2Long(fileData+pos); pos += 4;

//             if (fileInfoHeader < sRWStruct.chunkSize) {
//                 BigEndian = true;
//             }

//             unsigned long fileNameSize = fileLoader::changeEndian(fileLoader::char2Long(fileData+pos), BigEndian);
//             if (fileNameSize > 4) {
//                 for(int i=4; fileData[pos+i] != '\0'; i++) {
//                     fileName += fileData[pos+i];
//                     if (i >= fileNameSize+4) { break; }
//                 }
//                 cout << "- File name: " << fileName << endl;
//             } else {
//                 long unsigned fileTypeRead = changeEndian(char2Long(fileData+pos+24), BigEndian);
//                 for(int i=28; fileData[pos+i] != '\0'; i++) {
//                     fileName += fileData[pos+i];
//                     if (i >= fileTypeRead+28) { break; }
//                 }
//                 cout << "- RW Id: " << fileName << endl;
//                 if (fileName == "rwID_TEXDICTIONARY") {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".txd";
//                 } else if (fileName == "rwID_AUDIODATA") {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".snd";
//                 } else if (fileName == "rwID_AUDIOCUES") {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".cue";
//                 } else if (fileName == "rwID_STATETRANSITION") {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".rst";
//                 } else if (fileName == "rwID_HANIMANIMATION") {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".anm";
//                 } else if (fileName == "rwID_SPLINE") {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".spl";
//                 } else if (fileName == "rwID_RWS" || fileName == "rwpID_BODYDEF") {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".rws";
//                 } else if (fileName == "rwID_WORLD") {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".bsp";
//                 } else if (fileName == "rwID_CLUMP") {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".dff";
//                 } else if (fileName == "rwID_DMORPHANMSTREAM") {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".dma";
//                 } else {
//                     fileName = mainFileName + "_" + to_string(chunkSel) + ".bin";
//                 }
//             }
//             pos += changeEndian(fileInfo, BigEndian);
//             long unsigned fileSizeNonChunk = changeEndian(char2Long(fileData+pos), BigEndian);
//             // cout << "File Size (no chunk) " << fileSizeNonChunk << endl;


//             fileData_OG = new char[fileSizeNonChunk];
//             pos += 4;
//             memcpy(fileData_OG, fileData+pos, fileSizeNonChunk);
//             ofstream fileExt("./Extracted Data/" + fileName, ios::out | ios::binary | ios::trunc);
//             fileExt.write(fileData_OG, fileSizeNonChunk);
//             fileExt.close();
//             delete[] fileData_OG;
//             pos += fileSizeNonChunk;
            
            
//             if (SHSMData == 1) { while (pos % 4) {pos += 1;} }
//             // cout << "Actual position: " << pos << " | Full chunk size: " << chunkSize+12 <<endl;
//         } else {
//             fileData_OG = new char[chunkSize];
//             memcpy(fileData_OG, fileData+pos, chunkSize);
//             ofstream fileExt("./Extracted Data/" + mainFileName + "_" + to_string(chunkSel) + ".bin", ios::out | ios::binary | ios::trunc);
//             fileExt.write(fileData_OG, chunkSize);
//             fileExt.close();
//             delete[] fileData_OG;

//             pos += chunkSize;
//         }
//         // cout << "Chunk has been ended | Actual Position: " << pos << " | File size: " << fullFileSize <<endl;
//     } while (pos < fullFileSize);
// }