#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <wx/wx.h>

#include "ARC.h"
#include "reth.h"
#include <zlib.h>


using namespace std;
using namespace std::filesystem;
unsigned int ARCType;

string endianChangeString(unsigned long ELittleToEBig) {
    ostringstream tempString0;
    tempString0 << hex << (((ELittleToEBig << 24) & 0xff000000) | ((ELittleToEBig << 8) & 0xff0000) | ((ELittleToEBig >> 8) & 0xff00) | ((ELittleToEBig >> 24) & 0xff));
    string tempString = tempString0.str();
    transform(tempString.begin(), tempString.end(), tempString.begin(), ::toupper);
    return tempString;
}

unsigned long endianChangeULong(string EBigToELittle) {
    transform(EBigToELittle.begin(), EBigToELittle.end(), EBigToELittle.begin(), ::tolower);
    unsigned long tempLong = 0;
    stringstream tempString;
    tempString << hex << EBigToELittle;
    tempString >> tempLong;
    return (((tempLong << 24) & 0xff000000) | ((tempLong << 8) & 0xff0000) | ((tempLong >> 8) & 0xff00) | ((tempLong >> 24) & 0xff));
}

void importFunc(classValuesARC valuesARC, string fileImportPath) {
    ifstream fileImport(fileImportPath, ios_base::binary | ios::ate| ios::in);
    ifstream ARC(valuesARC.pathFileLoaded, ios_base::binary | ios::in);

    // First: it will regenerate the file until the end of the table file
    // Second: replace the file size values and the data position of the next data table values in case of need it
    // Third: regenerate the file until the part where the new data is inserted
    // Fourth: add new data
    // Fifth: lastly, add the rest of the untouched info
    // I have to make it practical so the user could import multiple files at
    // the same time without having to regenerate the whole file everytime just
    // one file is being imported
    // Also add support for other of the Climax's Silent Hills

    // read the arc | first look for the size, the buffer will be of 8 mb/8192 bytes
    
    string fileName = valuesARC.fileItemSelected.ToStdString();
    unsigned long unHashedStringFileName2Int = endianChangeULong(fileName);
    unsigned long hashedStringFileName2Int = SHSMWord2Hash(fileName);
    unsigned long fileCount, dataStartPos, fileImportIndex;
    
    
    switch (ARCType) {
        case 1:
        case 2:
            //Shattered Memories - 10FA0000
            //Origins UK - 41322E30 (A2.0)
            ARC.seekg(4, ios::beg);
            ARC.read((char*)&fileCount, 4);
            ARC.seekg(8, ios::beg);
            ARC.read((char*)&dataStartPos, 4);
            break;
        default:
            //Origins LA
            ARC.read((char*)&fileCount, 4);
            ARC.seekg(4, ios::beg);
            ARC.read((char*)&dataStartPos, 4);
            break;
    }




    //Find the index of the file
    for (unsigned long i = 0; i < valuesARC.ARCdata.size(); i++) {
        if (ARCType == 1) {
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
    
    //Step one: regenerate the ARC until the data start    
    string skibidi = valuesARC.pathFileLoaded += "1";
    fstream newARC(skibidi, ios::binary | ios::in | ios::out | ios::trunc);

    int readableData = dataStartPos;
    char *rawData;
    int dataReadPos = 0;
    int bufferSize = 8192;
    bool endfile = false;
   

    while (true) {
        readableData -= 8192;
        if (0 > readableData) {
            bufferSize = readableData + 8192;
            endfile = true;
        }
        // dont forget to make 0 == condition
        ARC.seekg(dataReadPos, ios::beg);
        rawData = new char[bufferSize];
        ARC.read(rawData, bufferSize);
        newARC.write((char*)&rawData[0], bufferSize);
        
        delete[] rawData;
        if (endfile) {
            break;
        }
        dataReadPos += 8192;
    }

    // Second Step: replace the file size values and the data position of the next data table values in case of need it
    int headerSkip = 16, importRealFileSize = fileImport.tellg();
    unsigned long importFileSpace = 0, importHeader, originalAdress = valuesARC.ARCdata[fileImportIndex+1][1]; // originalAdress is for step four
    if (ARCType == 2) {headerSkip = 20;}

    fileImport.seekg(0, ios::beg);
    fileImport.read((char*)&importHeader, 4);
    newARC.seekg(headerSkip + (fileImportIndex * 16) + 8, ios::beg);
    //Last files will bug out
    if (importHeader == 0xE0FFD8FF || ARCType == 3) {
        //Not compress
        

        //Calculate space to grab
        if (ARCType == 1) {
            while (importRealFileSize > importFileSpace) {
                importFileSpace += 2048; // divide by 4
            }
        }

        valuesARC.ARCdata[fileImportIndex][2] = importRealFileSize;
        valuesARC.ARCdata[fileImportIndex][3] = 0;
    } else {
        //Compress
        

        //Calculate space to grab
        if (ARCType == 1) {
            while (importRealFileSize > importFileSpace) {
                importFileSpace += 2048; // divide by 4
            }
        }

        valuesARC.ARCdata[fileImportIndex][2] = importRealFileSize;
        valuesARC.ARCdata[fileImportIndex][3] = 0;
    }
    newARC.write((char*)&valuesARC.ARCdata[fileImportIndex][2], 4);
    newARC.write((char*)&valuesARC.ARCdata[fileImportIndex][3], 4);

    // Relocate new file addresses
    signed long temp = 0;
    if (importFileSpace < (valuesARC.ARCdata[fileImportIndex+1][1] - valuesARC.ARCdata[fileImportIndex][1])) {
        temp =  -((valuesARC.ARCdata[fileImportIndex+1][1] - valuesARC.ARCdata[fileImportIndex][1]) - importFileSpace);
        wxLogMessage("temp - %i", temp);
    } else {
        temp = importFileSpace - (valuesARC.ARCdata[fileImportIndex+1][1] - valuesARC.ARCdata[fileImportIndex][1]);
    }
    wxLogMessage("\nfileImportIndex + %i\nfileImportIndex+1 + %i\nimportFileSpace %i",valuesARC.ARCdata[fileImportIndex][1], valuesARC.ARCdata[fileImportIndex+1][1], importFileSpace);
    for (unsigned int i = fileImportIndex+1; fileCount > i; i++) {
        newARC.seekg(headerSkip + (i * 16) + 4, ios::beg);
        valuesARC.ARCdata[i][1] = valuesARC.ARCdata[i][1] + temp;
        newARC.write((char*)&valuesARC.ARCdata[i][1], 4);
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
        // dont forget to make 0 == condition
        ARC.seekg(dataReadPos, ios::beg);
        rawData = new char[bufferSize];
        ARC.read(rawData, bufferSize);
        newARC.write((char*)&rawData[0], bufferSize);
        
        delete[] rawData;
        if (endfile) {
            break;
        }
        dataReadPos += 8192;
    }


    // Fourth step: add new data
    readableData = importRealFileSize;
    dataReadPos = 0;
    endfile = false;
    newARC.seekg(valuesARC.ARCdata[fileImportIndex][1], ios::beg);
    fileImport.seekg(0, ios::beg);
    bufferSize = 8192;
    while (true) {
        readableData -= 8192;
        if (0 > readableData) {
            bufferSize = readableData + 8192;
            endfile = true;
        }
        // dont forget to make 0 == condition
        fileImport.seekg(dataReadPos, ios::beg);
        rawData = new char[bufferSize];
        fileImport.read(rawData, bufferSize);
        newARC.write((char*)&rawData[0], bufferSize);
        
        delete[] rawData;
        if (endfile) {
            break;
        }
        dataReadPos += 8192;
    }
    bufferSize = importFileSpace - importRealFileSize;
    while (bufferSize > 0) {
        newARC.write((char*)"", 1);
        bufferSize--;
    }
    fileImport.close();

    // // Fifth: lastly, add the rest of the untouched info
    ARC.seekg(0, ios::end);
    unsigned long originalAdress1 = ARC.tellg();
    readableData = originalAdress1 - originalAdress;
    dataReadPos = originalAdress;
    bufferSize = 8192;
    endfile = false;
    while (true) {
        readableData -= 8192;
        if (0 > readableData) {
            bufferSize = readableData + 8192;
            endfile = true;
        }
        ARC.seekg(dataReadPos, ios::beg);
        rawData = new char[bufferSize];
        ARC.read(rawData, bufferSize);
        newARC.write((char*)&rawData[0], bufferSize);
        
        delete[] rawData;
        if (endfile) {
            break;
        }
        dataReadPos += 8192;
    }



    ARC.close();
    newARC.close();
};

void extractFunc(classValuesARC valuesARC, bool All) {
    char *rawData, *unCompData;
    bool nameFinded, rethEmpty = false;
    unsigned long hashedStringFileName2Int, unHashedStringFileName2Int;
    string extractPath, fileName = valuesARC.fileItemSelected.ToStdString();
    
    if (All == true) {
        extractPath = "./" + path(valuesARC.pathFileLoaded).stem().string();
        if (!is_directory(extractPath) || !exists(extractPath)) { create_directory(extractPath); }
    } else {
        unHashedStringFileName2Int = endianChangeULong(fileName);
        hashedStringFileName2Int = SHSMWord2Hash(valuesARC.fileItemSelected.ToStdString());
    }
    if (valuesARC.RETH.size() == 0) { rethEmpty = true; }
    ifstream ARC(valuesARC.pathFileLoaded, ios::in | ios_base::binary);

    for (unsigned long i = 0; i < valuesARC.ARCdata.size(); i++) {
        nameFinded = false;
        if (All == false) {
            ofstream fileExt("./" + fileName, ios::out | ios::binary | ios::trunc);
            if (ARCType == 1) {
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
            if (ARCType > 1) {
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
                fileName = "/Unknown Names/" + endianChangeString(valuesARC.ARCdata[i][0]) + ".dat";
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

vector<vector<unsigned long>> readARC(string filePath) {
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
            ARCType = 1;
            ARC.read((char*)&fileCount, 4);
            ARC.seekg(16, ios::beg);
            break;
        case 808333889:
            //Origins UK - 41322E30 (A2.0)
            ARCType = 2;
            ARC.read((char*)&fileCount, 4);
            ARC.seekg(20, ios::beg);
            break;
        default:
            //Origins LA
            ARCType = 3;
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
        if (ARCType == 3) {
            vecFiles[i].push_back(ARCFilesRead.dataPos + fileSignature);
        } else {
            vecFiles[i].push_back(ARCFilesRead.dataPos);
        }
        vecFiles[i].push_back(ARCFilesRead.dataSize);
        vecFiles[i].push_back(ARCFilesRead.dataSizeReal);
    }
    return vecFiles;
}