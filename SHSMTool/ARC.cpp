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
            ARC.seekg(16, ios::beg);
            break;
    }

    for (unsigned long i = 0; i < fileCount; i++) {
        ARC.read((char*)&ARCFilesRead, 16);
        vecFiles.push_back(vector<unsigned long>());
        vecFiles[i].push_back(ARCFilesRead.fileName);
        vecFiles[i].push_back(ARCFilesRead.dataPos);
        vecFiles[i].push_back(ARCFilesRead.dataSize);
        vecFiles[i].push_back(ARCFilesRead.dataSizeReal);
    }
    return vecFiles;
}