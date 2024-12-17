#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <Windows.h>

#include "fileLoader.h"

using namespace std;
using namespace std::filesystem;

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