#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <stdio.h>
#include <cstdio>
#include <wx/wx.h>

#include "ARC.h"
#include "reth.h"
#include <zlib.h>


using namespace std;
using namespace std::filesystem;
unsigned int ARC::ARCType;
bool ARC::compressImport;
classValuesARC valuesARC;

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

void ARC::importFunc(string fileImportPath) {
    string oldARCPath = valuesARC.pathFileLoaded + ".bk";
    std::remove(oldARCPath.c_str());
    std::rename(valuesARC.pathFileLoaded.c_str(), oldARCPath.c_str());
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

    string fileName = valuesARC.fileItemSelected.ToStdString();
    unsigned long unHashedStringFileName2Int = ARC::endianChangeULong(fileName);
    unsigned long hashedStringFileName2Int = RETH::SHSMWord2Hash(fileName);
    unsigned long fileCount, dataStartPos, fileImportIndex, fileNamesPos;
    
    
    switch (ARC::ARCType) {
        case 1:
            //Shattered Memories - 10FA0000
            oldARC.seekg(4, ios::beg);
            oldARC.read((char*)&fileCount, 4);
            oldARC.seekg(8, ios::beg);
            oldARC.read((char*)&dataStartPos, 4);
        case 2:
            //Origins UK - 41322E30 (A2.0)
            oldARC.seekg(4, ios::beg);
            oldARC.read((char*)&fileCount, 4);
            oldARC.seekg(8, ios::beg);
            oldARC.read((char*)&dataStartPos, 4);
            oldARC.seekg(12, ios::beg);
            oldARC.read((char*)&fileNamesPos, 4);
            break;
        default:
            //Origins LA
            oldARC.read((char*)&fileCount, 4);
            oldARC.seekg(4, ios::beg);
            oldARC.read((char*)&dataStartPos, 4);
            oldARC.seekg(8, ios::beg);
            oldARC.read((char*)&fileNamesPos, 4);
            break;
    }




    // Find the index of the selected file to import
    for (unsigned long i = 0; i < valuesARC.ARCdata.size(); i++) {
        if (ARC::ARCType == 1) {
            if (unHashedStringFileName2Int == valuesARC.ARCdata[i][0]
            || hashedStringFileName2Int == valuesARC.ARCdata[i][0]) {
                fileImportIndex = i;
                break;
            }
        } else if (fileName == valuesARC.RETH[i].second) {
            fileImportIndex = i;
            break;
        }
    }
    

    // This asks if the file will be compressed or not. If the file comes
    // from CLA's Origins or the user clicks no then it's doesn't compress it
    // CLA's Origins doesn't seems to support ZLib as the debug symbols of the
    // May 2006 proto doesn't mention ZLib anywhere
    wxMessageDialog* compressBox;
    if (ARC::ARCType != 3) {
        signed long checkCompressImport = wxID_NO;
        if (valuesARC.ARCdata[fileImportIndex][3] == 0) {
            compressBox = new wxMessageDialog(NULL, _("Do you really want to compress this file?\nThis file was not compressed"), _("Compress file"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
        } else {
            compressBox = new wxMessageDialog(NULL, _("Do you really want to compress this file?\nThis file was compressed"), _("Compress file"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
        }
        checkCompressImport = compressBox->ShowModal();
        switch (checkCompressImport) {
            case wxID_YES:
                ARC::compressImport = true;
                break;
            default:
                ARC::compressImport = false;
                break;
        };
    }
    

    // Step one: regenerate the ARC until the data start
    fstream newARC(valuesARC.pathFileLoaded, ios::binary | ios::in | ios::out | ios::trunc);
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
    if (ARC::ARCType == 2) {headerSkip = 20;}


    fileImport.seekg(0, ios::beg);
    newARC.seekg(headerSkip + (fileImportIndex * 16) + 8, ios::beg);
    
    
    // This calculate space to reserved for each file in the ARC
    // TODO: research the CLA's Origins betas as some of this seems
    // To be using a space minimun of 16 bytes while other 32
    if (ARC::ARCType == 3 || ARC::compressImport == false) {
        //Don't compress

        //Calculate space to grab
        switch (ARC::ARCType) {
            case 1:
                while (importRealFileSize > importFileSpace) {
                    importFileSpace += 2048;
                }
                break;
            case 2:
                while (importRealFileSize > importFileSpace) {
                    importFileSpace += 32;
                }
                break;
            case 3:
                while (importRealFileSize > importFileSpace) {
                    importFileSpace += 32;
                }
                break;
        }

        valuesARC.ARCdata[fileImportIndex][2] = importRealFileSize;
        valuesARC.ARCdata[fileImportIndex][3] = 0;
    } else {
        //Compress
        rawData = new char[importRealFileSize];
        fileImport.seekg(0, ios::beg);
        fileImport.read(rawData, importRealFileSize);
    
        compressedSize = compressBound(importRealFileSize);
        compressedData = new char[compressedSize];
    
        compress2((Bytef*)compressedData, &compressedSize, (Bytef*)rawData, importRealFileSize, 9);
        delete[] rawData;
        switch (ARC::ARCType) {
            case 1:
                while (compressedSize > importFileSpace) {
                    importFileSpace += 2048;
                }
                break;
            case 2:
                while (compressedSize > importFileSpace) {
                    importFileSpace += 32;
                }
                break;
        }

        valuesARC.ARCdata[fileImportIndex][2] = compressedSize;
        valuesARC.ARCdata[fileImportIndex][3] = importRealFileSize;
    }
    newARC.write((char*)&valuesARC.ARCdata[fileImportIndex][2], 4);
    newARC.write((char*)&valuesARC.ARCdata[fileImportIndex][3], 4);



    // temp = get the amount of space changed by the new file
    // temp1 = save the value of the position of whatever follows the file that is
    // being imported
    signed long temp = 0, temp1, temp2;

    if (fileImportIndex != valuesARC.ARCdata.size() - 1) {
        temp1 = valuesARC.ARCdata[fileImportIndex+1][1];
    } else if (ARC::ARCType == 1) {
        temp1 = oldARCSize;
    } else {
        temp1 = fileNamesPos;
    }



    if (importFileSpace < (temp1 - valuesARC.ARCdata[fileImportIndex][1])) {
        temp =  -((temp1 - valuesARC.ARCdata[fileImportIndex][1]) - importFileSpace);
    } else {
        temp = importFileSpace - (temp1 - valuesARC.ARCdata[fileImportIndex][1]);
    }

    for (unsigned long i = fileImportIndex+1; fileCount > i; i++) {
        newARC.seekg(headerSkip + (i * 16) + 4, ios::beg);
        valuesARC.ARCdata[i][1] += temp;
        if (ARC::ARCType != 3) {
            temp2 = valuesARC.ARCdata[i][1];
        } else {
            temp2 = valuesARC.ARCdata[i][1] - dataStartPos;
        }
        newARC.write((char*)&temp2, 4);
    }


    if (ARC::ARCType != 1) {
        fileNamesPos += temp;
        newARC.seekg(headerSkip - 8, ios::beg);
        newARC.write((char*)&fileNamesPos, 4);
    }



    // Third Step: regenerate the file until the part where the new data is inserted
    readableData = valuesARC.ARCdata[fileImportIndex][1] - dataStartPos;
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
    newARC.seekg(valuesARC.ARCdata[fileImportIndex][1], ios::beg);
    if (ARC::ARCType == 3 || ARC::compressImport == false) {
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


void ARC::extractFunc(bool All) {
    char *rawData, *unCompData;
    bool nameFinded, rethEmpty = false;
    unsigned long hashedStringFileName2Int, unHashedStringFileName2Int;
    string extractPath, fileName = valuesARC.fileItemSelected.ToStdString();
    
    if (All == true) {
        extractPath = "./" + path(valuesARC.pathFileLoaded).stem().string();
        if (!is_directory(extractPath) || !exists(extractPath)) { create_directory(extractPath); }
    } else {
        unHashedStringFileName2Int = ARC::endianChangeULong(fileName);
        hashedStringFileName2Int = RETH::SHSMWord2Hash(valuesARC.fileItemSelected.ToStdString());
    }
    if (valuesARC.RETH.size() == 0) { rethEmpty = true; }
    ifstream ARC(valuesARC.pathFileLoaded, ios::in | ios_base::binary);

    for (unsigned long i = 0; i < valuesARC.ARCdata.size(); i++) {
        nameFinded = false;
        if (All == false) {
            ofstream fileExt("./" + fileName, ios::out | ios::binary | ios::trunc);
            if (ARC::ARCType == 1) {
                if (unHashedStringFileName2Int == valuesARC.ARCdata[i][0]) {
                    fileName += ".dat";
                    nameFinded = true;
                } else if (hashedStringFileName2Int == valuesARC.ARCdata[i][0]) {
                    nameFinded = true;
                }
            } else if (fileName == valuesARC.RETH[i].second) {
                nameFinded = true;
            }

            if (nameFinded) {
                rawData = new char[valuesARC.ARCdata[i][2]];
                ARC.seekg(valuesARC.ARCdata[i][1], ios::beg);
                ARC.read(rawData, valuesARC.ARCdata[i][2]);
                if (valuesARC.ARCdata[i][3] > 0) {
                    unCompData = new char[valuesARC.ARCdata[i][3]];
                    uncompress((Bytef*)unCompData, &valuesARC.ARCdata[i][3], (Bytef*)rawData, valuesARC.ARCdata[i][2]);
                    fileExt.write((char*)&unCompData[0], valuesARC.ARCdata[i][3]);
                    delete[] unCompData;
                } else {
                    fileExt.write((char*)&rawData[0], valuesARC.ARCdata[i][2]);
                }
                delete[] rawData;
                fileExt.close();
                break;
            }
        } else {
            if (ARC::ARCType != 1) {
                fileName = "/" + valuesARC.RETH[i].second;
                nameFinded = true;
            } else if (!rethEmpty) {
                for (unsigned long j = 0; j < valuesARC.RETH.size(); j++) {
                    if (valuesARC.ARCdata[i][0] == valuesARC.RETH[j].first) {
                        fileName = "/" + valuesARC.RETH[j].second;
                        nameFinded = true;
                        break;
                    }
                }
            }

            if (!nameFinded) {
                fileName = "/Unknown Names/" + ARC::endianChangeString(valuesARC.ARCdata[i][0]) + ".dat";
                if (!is_directory(extractPath + "/Unknown Names") || !exists(extractPath + "/Unknown Names")) {
                    create_directory(extractPath + "/Unknown Names");
                }
            }
            ofstream fileExt(extractPath + fileName, ios::out | ios::binary | ios::trunc);
            rawData = new char[valuesARC.ARCdata[i][2]];
            ARC.seekg(valuesARC.ARCdata[i][1], ios::beg);
            ARC.read(rawData, valuesARC.ARCdata[i][2]);
            if (valuesARC.ARCdata[i][3] > 0) {
                unCompData = new char[valuesARC.ARCdata[i][3]];
                uncompress((Bytef*)unCompData, &valuesARC.ARCdata[i][3], (Bytef*)rawData, valuesARC.ARCdata[i][2]);
                fileExt.write((char*)&unCompData[0], valuesARC.ARCdata[i][3]);
                delete[] unCompData;
            } else {
                fileExt.write((char*)&rawData[0], valuesARC.ARCdata[i][2]);
            }
            delete[] rawData;
            fileExt.close();
        }
    }
};


//TODO: make a verifier for the third case so users doesn't try to load
//ARC files from other games like PC Silent Hill 3
vector<vector<unsigned long>> ARC::readARC(string filePath) {
    ifstream ARC(filePath.c_str(), ios::in | ios_base::binary);
    if (!ARC.is_open()) {
        printf("Missing file ARC!\n");
        return {};
    }
    unsigned long fileSignature;
    unsigned long fileCount;
    ARC.read((char*)&fileSignature, 4);
    vector<vector<unsigned long>> vecFiles;
    struct ARCFiles {
        unsigned long fileName;
        unsigned long dataPos;
        unsigned long dataSize;
        unsigned long dataSizeReal;
    } ARCFilesRead;
    
    switch (fileSignature) {
        case 64016:
            //Shattered Memories - 10FA0000
            ARC::ARCType = 1;
            ARC.read((char*)&fileCount, 4);
            ARC.seekg(16, ios::beg);
            break;
        case 808333889:
            //Origins UK - 41322E30 (A2.0)
            ARC::ARCType = 2;
            ARC.read((char*)&fileCount, 4);
            ARC.seekg(20, ios::beg);
            break;
        default:
            //Origins LA
            ARC::ARCType = 3;
            fileCount = fileSignature;
            ARC.seekg(4, ios::beg);
            ARC.read((char*)&fileSignature, 4);
            ARC.seekg(16, ios::beg);
            break;
    }

    for (unsigned long i = 0; i < fileCount; i++) {
        ARC.read((char*)&ARCFilesRead, 16);
        vecFiles.push_back(vector<unsigned long>());
        vecFiles[i].push_back(ARCFilesRead.fileName);
        if (ARC::ARCType == 3) {
            vecFiles[i].push_back(ARCFilesRead.dataPos + fileSignature);
        } else {
            vecFiles[i].push_back(ARCFilesRead.dataPos);
        }
        vecFiles[i].push_back(ARCFilesRead.dataSize);
        vecFiles[i].push_back(ARCFilesRead.dataSizeReal);
    }
    return vecFiles;
}