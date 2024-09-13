#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>

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