#include <iostream>
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <stdio.h>
#include <cstdio>
#include <wx/wx.h>
#include <wx/utils.h>

#include "ARC.h"
#include "wxwindow.h"
#include <zlib.h>


using namespace std;
static bool compressImport;

/** @brief RETH (RenderEclipse Tools Hashes) file type
 * ye I did my own file type just bc I want to try to make one by myself
 * although is kinda useless and probably I should have go with just making
 * a whole txt list with real names and then make a process to test if any
 * hash pairs with a converted to hash name with SHSM_FilenameHash
 * 
 * Credits to:
 * - PatrickHamster (XeNTaX Discord) giving me a guide to manage hashes.
 * - SPECIAL THANKS to TPU (XeNTaX page [R.I.P]) as they discovered the hashing
 * method that without it I wouldn't have even tried to make this tool.
 */

unsigned long ARC::SHSM_FilenameHash(string text) {
    unsigned long hash = 0; text += "";
    for (int i = 0; text[i] != '\0'; i++) {
        hash = (hash * 33) ^ tolower(text[i]);
    }
    return hash;
}

void ARC::readFilenames(string filePath) {
    ifstream file(filePath.c_str(), ios::in | ios_base::binary);

    RETH::header reth_header;
    RETH::hashes reth_hash;
    ARC::header  arc_header;

    std::string   fileName;
    unsigned long stringSize;
    
    if (ARC::loadedARC_Info.fileNames.size() != 0) {
        ARC::loadedARC_Info.fileNames.clear();
    }

    switch (ARC::loadedARC_Info.type) {
    case ARC::type_SM:
        file.read((char*)&reth_header, sizeof(reth_header));
        if (reth_header.header != RETH_SIGNATURE) {
            wxLogStatus("Not a RETH file!");
            ARC::loadedARC_Info.fileNames = {};
            break;
        }
        for (unsigned long i = 0; i < reth_header.hashesCount; i++) {
            file.read((char*)&reth_hash, 6);
            fileName.resize(reth_hash.stringSize);
            file.read((char*)&fileName[0], reth_hash.stringSize);
            ARC::loadedARC_Info.fileNames.push_back({reth_hash.hash, fileName});
            fileName.clear();
        }
        break;
    case ARC::type_Solent:
        file.seekg(4, ios::beg);
    case ARC::type_LA:
        file.read((char*)&arc_header, 16);
        file.seekg(arc_header.namesPos, ios::beg);
        for (unsigned long i = 0; i < arc_header.fileCount; i++) {
            if (i != 0 && i != arc_header.fileCount - 1) {
                stringSize = ARC::loadedARC_Info.fileData[i + 1].fileName - ARC::loadedARC_Info.fileData[i].fileName - 1;
            }
            else if (i == arc_header.fileCount - 1) {
                stringSize = arc_header.namesSize - ARC::loadedARC_Info.fileData[i].fileName - 1;
            }
            else {
                stringSize = ARC::loadedARC_Info.fileData[i + 1].fileName - 1;
            }
            fileName.resize(stringSize);
            file.read((char*)&fileName[0], stringSize);
            file.seekg(1, ios::cur);
            ARC::loadedARC_Info.fileNames.push_back({0, fileName});
            fileName.clear();
        }
        break;
    }
    file.close();
    return;
}

string ARC::endianChangeString(unsigned long ELittleToEBig) {
    ostringstream tempString0;
    tempString0 << hex << (((ELittleToEBig << 24) & 0xff000000)
    | ((ELittleToEBig << 8) & 0xff0000) | ((ELittleToEBig >> 8) & 0xff00)
    | ((ELittleToEBig >> 24) & 0xff));
    string tempString = tempString0.str();
    transform(tempString.begin(), tempString.end(), tempString.begin(), ::toupper);
    return tempString;
}

unsigned long ARC::endianChangeULong(string EBigToELittle) {
    transform(EBigToELittle.begin(), EBigToELittle.end(), EBigToELittle.begin(), ::tolower);
    unsigned long tempLong = 0;
    stringstream tempString;
    tempString << hex << EBigToELittle;
    tempString >> tempLong;
    return (((tempLong << 24) & 0xff000000) | ((tempLong << 8) & 0xff0000)
    | ((tempLong >> 8) & 0xff00) | ((tempLong >> 24) & 0xff));
}

/**@brief Main ARC managment handlers. */

void ARC::exportFile(bool exportAllFiles) {
    char *rawData, *unCompData;
    bool nameFound = false, rethEmpty = false;
    unsigned long filename_hash_SHSM, unHashedStringFileName2Int, i, j;
    string extractPath, fileName = ARC::loadedARC_Info.fileItemSelected.ToStdString();
    
    if (exportAllFiles == true) {
        extractPath = "./" + std::filesystem::path(ARC::loadedARC_Info.pathFileLoaded).stem().string();
        if (!std::filesystem::is_directory(extractPath) || !std::filesystem::exists(extractPath)) { std::filesystem::create_directory(extractPath); }
    } else if (ARC_IsType(ARC::type_SM)) {
        unHashedStringFileName2Int = ARC::endianChangeULong(fileName);
        filename_hash_SHSM = ARC::SHSM_FilenameHash(fileName);
    }
    if (ARC::loadedARC_Info.fileNames.size() == 0) { rethEmpty = true; }

    ifstream ARC(ARC::loadedARC_Info.pathFileLoaded, ios::in | ios_base::binary);
    
    if (exportAllFiles == false && fileName != "") {
        ofstream fileExtOne("./" + fileName, ios::out | ios::binary | ios::trunc);
        for (i = 0; i < ARC::loadedARC_Info.fileData.size(); i++) {
            if (ARC::loadedARC_Info.type == ARC::type_SM) {
                if (unHashedStringFileName2Int == ARC::loadedARC_Info.fileData[i].fileName) {
                    fileName += ".dat";
                    nameFound = true;
                } else if (filename_hash_SHSM == ARC::loadedARC_Info.fileData[i].fileName) {
                    nameFound = true;
                }
            } else if (fileName == ARC::loadedARC_Info.fileNames[i].filename) {
                nameFound = true;
            }
            
            if (nameFound) {
                rawData = new char[ARC::loadedARC_Info.fileData[i].dataSize];
                ARC.seekg(ARC::loadedARC_Info.fileData[i].dataPos, ios::beg);
                ARC.read(rawData, ARC::loadedARC_Info.fileData[i].dataSize);
                if (ARC::loadedARC_Info.fileData[i].dataSizeReal > 0) {
                    unCompData = new char[ARC::loadedARC_Info.fileData[i].dataSizeReal];
                    uncompress((Bytef*)unCompData, &ARC::loadedARC_Info.fileData[i].dataSizeReal, (Bytef*)rawData, ARC::loadedARC_Info.fileData[i].dataSize);
                    fileExtOne.write((char*)&unCompData[0], ARC::loadedARC_Info.fileData[i].dataSizeReal);
                    delete[] unCompData;
                } else {
                    fileExtOne.write((char*)&rawData[0], ARC::loadedARC_Info.fileData[i].dataSize);
                }
                delete[] rawData;
                fileExtOne.close();
                break;
            }
        }
    } else if (exportAllFiles == true) {
        for (i = 0; i < ARC::loadedARC_Info.fileData.size(); i++) {
            nameFound = false;
            if (ARC_IsNotType(ARC::type_SM)) {
                fileName = "/" + ARC::loadedARC_Info.fileNames[i].filename;
                nameFound = true;
            } else if (!rethEmpty) {
                for (j = 0; j < ARC::loadedARC_Info.fileNames.size(); j++) {
                    if (ARC::loadedARC_Info.fileData[i].fileName == ARC::loadedARC_Info.fileNames[j].hash) {
                        fileName = "/" + ARC::loadedARC_Info.fileNames[j].filename;
                        nameFound = true;
                        break;
                    }
                }
            }

            if (!nameFound) {
                fileName = "/Unknown Names/" + ARC::endianChangeString(ARC::loadedARC_Info.fileData[i].fileName) + ".dat";
                if (!std::filesystem::is_directory(extractPath + "/Unknown Names") || !std::filesystem::exists(extractPath + "/Unknown Names")) {
                    std::filesystem::create_directory(extractPath + "/Unknown Names");
                }
            }
            ofstream fileExt(extractPath + fileName, ios::out | ios::binary | ios::trunc);
            rawData = new char[ARC::loadedARC_Info.fileData[i].dataSize];
            ARC.seekg(ARC::loadedARC_Info.fileData[i].dataPos, ios::beg);
            ARC.read(rawData, ARC::loadedARC_Info.fileData[i].dataSize);
            if (ARC::loadedARC_Info.fileData[i].dataSizeReal > 0) {
                unCompData = new char[ARC::loadedARC_Info.fileData[i].dataSizeReal];
                uncompress((Bytef*)unCompData, &ARC::loadedARC_Info.fileData[i].dataSizeReal, (Bytef*)rawData, ARC::loadedARC_Info.fileData[i].dataSize);
                fileExt.write((char*)&unCompData[0], ARC::loadedARC_Info.fileData[i].dataSizeReal);
                delete[] unCompData;
            } else {
                fileExt.write((char*)&rawData[0], ARC::loadedARC_Info.fileData[i].dataSize);
            }
            delete[] rawData;
            fileExt.close();
        }
    }
};

void ARC::importFile(string fileImportPath) {
    string oldARCPath = ARC::loadedARC_Info.pathFileLoaded + ".bk";
    std::remove(oldARCPath.c_str());
    std::rename(ARC::loadedARC_Info.pathFileLoaded.c_str(), oldARCPath.c_str());
    ifstream fileImport(fileImportPath, ios_base::binary | ios::ate| ios::in);
    ifstream oldARC(oldARCPath, ios_base::binary | ios::in);

    /*
    Step One:    it will regenerate the file until the end of the table file
    Step Two:    replace the file size values and the data position of the next data table values in case of need it
    Step Three:  regenerate the file until the part where the new data is inserted
    Step Four:   add new data
    Step Five:   lastly, add the rest of the untouched info
    */

    // I have to make it practical so the user could import multiple files at
    // the same time without having to regenerate the whole file everytime just
    // one file is being imported

    /*
    Step zero:
    Get some of the headers values
    */

    string fileName = ARC::loadedARC_Info.fileItemSelected.ToStdString();
    unsigned long unHashedStringFileName2Int = ARC::endianChangeULong(fileName);
    unsigned long hashedStringFileName2Int = ARC::SHSM_FilenameHash(fileName);
    unsigned long fileCount, dataStartPos, fileImportIndex, fileNamesPos;
    
    
    switch (ARC::loadedARC_Info.type) {
        case ARC::type_SM:
            //Shattered Memories - 10FA0000
            oldARC.seekg(4, ios::beg);
            oldARC.read((char*)&fileCount, 4);
            oldARC.seekg(8, ios::beg);
            oldARC.read((char*)&dataStartPos, 4);
        case ARC::type_Solent:
            //Origins UK - 41322E30 (A2.0)
            oldARC.seekg(4, ios::beg);
            oldARC.read((char*)&fileCount, 4);
            oldARC.seekg(8, ios::beg);
            oldARC.read((char*)&dataStartPos, 4);
            oldARC.seekg(12, ios::beg);
            oldARC.read((char*)&fileNamesPos, 4);
            break;
        case ARC::type_LA:
            //Origins LA
            oldARC.read((char*)&fileCount, 4);
            oldARC.seekg(4, ios::beg);
            oldARC.read((char*)&dataStartPos, 4);
            oldARC.seekg(8, ios::beg);
            oldARC.read((char*)&fileNamesPos, 4);
            break;
    }


    // Find the index of the selected file to import
    for (unsigned long i = 0; i < ARC::loadedARC_Info.fileData.size(); i++) {
        if (ARC::loadedARC_Info.type == ARC::type_SM) {
            if (unHashedStringFileName2Int == ARC::loadedARC_Info.fileData[i].fileName
            || hashedStringFileName2Int == ARC::loadedARC_Info.fileData[i].fileName) {
                fileImportIndex = i;
                break;
            }
        } else if (fileName == ARC::loadedARC_Info.fileNames[i].filename) {
            fileImportIndex = i;
            break;
        }
    }
    

    // This asks if the file will be compressed or not. If the file comes
    // from CLA's Origins or the user clicks no then it's doesn't compress it
    // CLA's Origins doesn't seems to support ZLib as the debug symbols of the
    // May 2006 proto doesn't mention ZLib anywhere
    if (ARC::loadedARC_Info.type != ARC::type_LA) {
        signed long checkCompressImport = wxID_NO;
        if (ARC::loadedARC_Info.fileData[fileImportIndex].dataSizeReal == 0) {
            mainWin->errorMessage = new wxMessageDialog(NULL, _("Do you really want to compress this file?\nThis file was not compressed"), _("Compress file"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
        } else {
            mainWin->errorMessage = new wxMessageDialog(NULL, _("Do you really want to compress this file?\nThis file was compressed"), _("Compress file"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
        }
        checkCompressImport = mainWin->errorMessage->ShowModal();
        switch (checkCompressImport) {
            case wxID_YES:
                compressImport = true;
                break;
            default:
                compressImport = false;
                break;
        };
        delete mainWin->errorMessage;
    }
    

    // Step one: regenerate the ARC until the data start
    fstream newARC(ARC::loadedARC_Info.pathFileLoaded, ios::binary | ios::in | ios::out | ios::trunc);
    signed long readableData = dataStartPos, dataReadPos = 0, bufferSize = 8192;
    char *rawData, *compressedData;
    bool endfile = false;
   

    while (true) {
        readableData -= 8192;
        if (0 > readableData) {
            bufferSize = readableData + 8192;
            endfile = true;
        }
        oldARC.seekg(dataReadPos, ios::beg);
        rawData = new char[bufferSize];
        oldARC.read(rawData, bufferSize);
        newARC.write((char*)&rawData[0], bufferSize);
        
        delete[] rawData;
        if (endfile) {
            break;
        }
        dataReadPos += 8192;
    }
    

    // Second Step: replace the file data values and the header values
    oldARC.seekg(0, ios::end);
    unsigned long headerSkip = 16, compressedSize = 0, importRealFileSize = fileImport.tellg(),
    importFileSpace = 0, oldARCSize = oldARC.tellg(); // originalAddress is for step four
    if (ARC::loadedARC_Info.type == ARC::type_Solent) {headerSkip = 20;}


    fileImport.seekg(0, ios::beg);
    newARC.seekg(headerSkip + (fileImportIndex * 16) + 8, ios::beg);
    
    
    // This calculate space to reserved for each file in the ARC
    // TODO: research the CLA's Origins betas as some of this seems
    // To be using a space minimun of 16 bytes while other 32
    if (ARC::loadedARC_Info.type == ARC::type_LA || compressImport == false) {
        //Don't compress

        //Calculate space to grab
        switch (ARC::loadedARC_Info.type) {
            case ARC::type_SM:
                while (importRealFileSize > importFileSpace) {
                    importFileSpace += 2048;
                }
                break;
            case ARC::type_Solent:
                while (importRealFileSize > importFileSpace) {
                    importFileSpace += 32;
                }
                break;
            case ARC::type_LA:
                while (importRealFileSize > importFileSpace) {
                    importFileSpace += 32;
                }
                break;
        }

        ARC::loadedARC_Info.fileData[fileImportIndex].dataSize = importRealFileSize;
        ARC::loadedARC_Info.fileData[fileImportIndex].dataSizeReal = 0;
    } else {
        //Compress
        rawData = new char[importRealFileSize];
        fileImport.seekg(0, ios::beg);
        fileImport.read(rawData, importRealFileSize);
    
        compressedSize = compressBound(importRealFileSize);
        compressedData = new char[compressedSize];
    
        compress2((Bytef*)compressedData, &compressedSize, (Bytef*)rawData, importRealFileSize, 9);
        delete[] rawData;
        switch (ARC::loadedARC_Info.type) {
            case ARC::type_SM:
                while (compressedSize > importFileSpace) {
                    importFileSpace += 2048;
                }
                break;
            case ARC::type_Solent:
                while (compressedSize > importFileSpace) {
                    importFileSpace += 32;
                }
                break;
        }

        ARC::loadedARC_Info.fileData[fileImportIndex].dataSize = compressedSize;
        ARC::loadedARC_Info.fileData[fileImportIndex].dataSizeReal = importRealFileSize;
    }
    newARC.write((char*)&ARC::loadedARC_Info.fileData[fileImportIndex].dataSize, 4);
    newARC.write((char*)&ARC::loadedARC_Info.fileData[fileImportIndex].dataSizeReal, 4);



    // temp = get the amount of space changed by the new file
    // temp1 = save the value of the position of whatever follows the file that is
    // being imported
    signed long temp = 0, temp1, temp2;

    if (fileImportIndex != ARC::loadedARC_Info.fileData.size() - 1) {
        temp1 = ARC::loadedARC_Info.fileData[fileImportIndex+1].fileName;
    } else if (ARC::loadedARC_Info.type == ARC::type_LA) {
        temp1 = oldARCSize;
    } else {
        temp1 = fileNamesPos;
    }



    if (importFileSpace < (temp1 - ARC::loadedARC_Info.fileData[fileImportIndex].fileName)) {
        temp =  -((temp1 - ARC::loadedARC_Info.fileData[fileImportIndex].fileName) - importFileSpace);
    } else {
        temp = importFileSpace - (temp1 - ARC::loadedARC_Info.fileData[fileImportIndex].fileName);
    }

    for (unsigned long i = fileImportIndex+1; fileCount > i; i++) {
        newARC.seekg(headerSkip + (i * 16) + 4, ios::beg);
        ARC::loadedARC_Info.fileData[i].fileName += temp;
        if (ARC::loadedARC_Info.type != ARC::type_LA) {
            temp2 = ARC::loadedARC_Info.fileData[i].fileName;
        } else {
            temp2 = ARC::loadedARC_Info.fileData[i].fileName - dataStartPos;
        }
        newARC.write((char*)&temp2, 4);
    }


    if (ARC::loadedARC_Info.type != ARC::type_SM) {
        fileNamesPos += temp;
        newARC.seekg(headerSkip - 8, ios::beg);
        newARC.write((char*)&fileNamesPos, 4);
    }

    // Third Step: regenerate the file until the part where the new data is inserted
    readableData = ARC::loadedARC_Info.fileData[fileImportIndex].dataPos - dataStartPos;
    dataReadPos = dataStartPos;
    endfile = false;
    newARC.seekg(dataReadPos, ios::beg);
    bufferSize = 8192;
    while (true) {
        readableData -= 8192;
        if (0 > readableData) {
            bufferSize = readableData + 8192;
            endfile = true;
        }
        oldARC.seekg(dataReadPos, ios::beg);
        rawData = new char[bufferSize];
        oldARC.read(rawData, bufferSize);
        newARC.write((char*)&rawData[0], bufferSize);
        
        delete[] rawData;
        if (endfile) {
            break;
        }
        dataReadPos += 8192;
    }


    // Fourth step: add new data
    newARC.seekg(ARC::loadedARC_Info.fileData[fileImportIndex].dataPos, ios::beg);
    if (ARC::loadedARC_Info.type == ARC::type_LA || compressImport == false) {
        // The file is NOT being compressed
        readableData = importRealFileSize;
        dataReadPos = 0;
        endfile = false;
        bufferSize = 8192;
        while (true) {
            readableData -= 8192;
            if (0 > readableData) {
                bufferSize = readableData + 8192;
                endfile = true;
            }
            fileImport.seekg(dataReadPos, ios::beg);
            rawData = new char[bufferSize];
            fileImport.read(rawData, bufferSize);
            newARC.write((char*)&rawData[0], bufferSize);
            
            delete[] rawData;
            if (endfile) {
                bufferSize = importFileSpace - importRealFileSize;
                while (bufferSize > 0) {
                    newARC.write((char*)"", 1);
                    bufferSize--;
                }
                break;
            }
            dataReadPos += 8192;
        }
    } else {
        // The file is NOT being compressed
        newARC.write((char*)&compressedData[0], compressedSize);
        delete[] compressedData;
        bufferSize = importFileSpace - compressedSize;
        while (bufferSize > 0) {
            newARC.write((char*)"", 1);
            bufferSize--;
        }
    }
    fileImport.close();

    // Fifth step: lastly, add the rest of the untouched info
    readableData = oldARCSize - temp1;
    dataReadPos = temp1;
    bufferSize = 8192;
    endfile = false;
    while (true) {
        readableData -= 8192;
        if (0 > readableData) {
            bufferSize = readableData + 8192;
            endfile = true;
        }
        oldARC.seekg(dataReadPos, ios::beg);
        rawData = new char[bufferSize];
        oldARC.read(rawData, bufferSize);
        newARC.write((char*)&rawData[0], bufferSize);
        
        delete[] rawData;
        if (endfile) {
            break;
        }
        dataReadPos += 8192;
    }



    oldARC.close();
    newARC.close();
    std::remove(oldARCPath.c_str());
};

bool ARC::read(void) {
    unsigned long fileSignature, fileSize, i;
    ARC::header ARC_header;
    ifstream ARC(ARC::loadedARC_Info.pathFileLoaded, ios::in | ios_base::binary);
    ARC.seekg(0, std::ios::end); fileSize = ARC.tellg(); ARC.seekg(0, std::ios::beg);
    if (!ARC.is_open()) {
        wxLogStatus("Missing file ARC!\n");
        return false;
    }
    if (ARC::loadedARC_Info.fileData.size() != 0) {
        ARC::loadedARC_Info.fileData.clear();
    }

    ARC.read((char*)&fileSignature, 4);
    ARC::fileEntryInfo ARC_FileEntryData;
    
    switch (fileSignature) {
        case ARC_SIGNATURE_SHSM:
            //Shattered Memories - 10FA0000
            ARC::loadedARC_Info.type = ARC::type_SM;
            break;
        case ARC_SIGNATURE_SOLENT:
            //Origins UK - 41322E30 (A2.0)
            ARC::loadedARC_Info.type = ARC::type_Solent;
            break;
        default:
            //Origins LA
            ARC.seekg(0, std::ios::beg);
            ARC::loadedARC_Info.type = ARC::type_LA;
            break;
    }
    ARC.read((char*)&ARC_header, sizeof(ARC::header));
    if (ARC_IsType(ARC::type_SM)) { ARC.seekg(16, std::ios::beg); }
    
    // File check to avoid someone crashing their PC opening ARC files from a completely different game.
    if (ARC_header.fileCount * 16 > fileSize ||
        (ARC_IsType(ARC::type_SM) && ARC_header.namesPos != 0) ||
        ((ARC_IsType(ARC::type_Solent) || ARC_IsType(ARC::type_LA)) && (ARC_header.namesPos + ARC_header.namesSize) > fileSize)) {
        wxLogStatus("Not a Climax ARC file!\n");
        return false;
    }
    

    for (i = 0; i < ARC_header.fileCount; i++) {
        ARC.read((char*)&ARC_FileEntryData, 16);

        if (ARC_IsType(ARC::type_LA)) {
            ARC_FileEntryData.dataPos += fileSignature;
        }
        
        ARC::loadedARC_Info.fileData.push_back({
            ARC_FileEntryData.fileName,
            ARC_FileEntryData.dataPos,
            ARC_FileEntryData.dataSize,
            ARC_FileEntryData.dataSizeReal
        });
    }

    return true;
}
