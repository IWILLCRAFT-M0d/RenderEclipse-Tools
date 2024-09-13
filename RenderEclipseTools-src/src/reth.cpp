#include <algorithm>
#include <fstream>
#include <vector>
#include <filesystem>
#include <string>

#include "reth.h"
#include "arc.h"

#include <wx/wx.h>
#include <wx/string.h>
using namespace std;
using namespace filesystem;

/* RETH (RenderEclipse Tools Hashes) file type
ye I did my own file type just bc I want to try to make one by myself
although is kinda useless and probably I should have go with just making
a whole txt list with real names and then make a process to test if any
hash pairs with a converted to hash name with SHSMWord2Hash

Credits to:
- PatrickHamster (XeNTaX Discord) giving me a guide to manage hashes.
- SPECIAL THANKS to TPU (XeNTaX page [R.I.P]) as they discovered the hashing
method that without it I wouldn't have even tried to make this tool. */

unsigned long SHSMWord2Hash(string text) {
    unsigned long hash = 0; text += "";
    for (int i = 0; text[i] != '\0'; i++) {
        hash = (hash * 33) ^ tolower(text[i]);
    }
    return hash;
}

vector<pair<unsigned long, string>> readNames(string filePath, vector<vector<unsigned long>> ARCdata) {
    ifstream fileNames(filePath.c_str(), ios::in | ios_base::binary);
    string fileName;
    unsigned long stringSize;
    vector<pair<unsigned long, string>> fileNameList;
    struct ARCHeader {
        unsigned long fileCount;
        unsigned long dataStart;
        unsigned long namesPos;
        unsigned long namesSize;
    } ARCHeaderRead;
    switch (ARCType) {
    case 1:
        struct RETH_Header {
            unsigned long header; //75777520
            unsigned long hashesCount;
        } RETH_Head;
        fileNames.read((char*)&RETH_Head, sizeof(RETH_Head));
        if (RETH_Head.header != 544569205) {
            wxLogStatus("Not a RETH file!");
            fileNames.close();
            return {};
        }
        struct RETHHashes {
            unsigned long hash;
            unsigned short stringSize;
        } RETHHashesRead;
        for (unsigned long i = 0; i < RETH_Head.hashesCount; i++) {
            fileNames.read((char*)&RETHHashesRead, 6);
            fileName.resize(RETHHashesRead.stringSize);
            fileNames.read((char*)&fileName[0], RETHHashesRead.stringSize);
            fileNameList.push_back(make_pair(RETHHashesRead.hash, fileName));
            fileName.clear();
        }
        fileNames.close();
        return fileNameList;
    case 2:
        fileNames.seekg(4, ios::beg);
    case 3:
        fileNames.read((char*)&ARCHeaderRead, 16);
        fileNames.seekg(ARCHeaderRead.namesPos, ios::beg);
        for (unsigned long i = 0; i < ARCHeaderRead.fileCount; i++) {
            if (i != 0 && i != ARCHeaderRead.fileCount - 1) {
                stringSize = ARCdata[i + 1][0] - ARCdata[i][0] - 1;
            }
            else if (i == ARCHeaderRead.fileCount - 1) {
                stringSize = ARCHeaderRead.namesSize - ARCdata[i][0] - 1;
            }
            else {
                stringSize = ARCdata[i + 1][0] - 1;
            }
            fileName.resize(stringSize);
            fileNames.read((char*)&fileName[0], stringSize);
            fileNames.seekg(1, ios::cur);
            fileNameList.push_back(make_pair(0, fileName));
            fileName.clear();
        }
        fileNames.close();
        return fileNameList;
    }
    return fileNameList;
}