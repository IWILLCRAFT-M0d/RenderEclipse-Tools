#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
using namespace std;

unsigned long SHSMWord2Hash(string text) {
    text = text + "";
    unsigned long hash = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        hash = (hash * 33) ^ tolower(text[i]);// & 0xFFFFFFFF;
    }
    return hash;
}

vector<pair<unsigned long, string>> readRETH(char* arg) {
    cout << arg << endl;
    ifstream RETH(arg, ios::in | ios_base::binary);
    if (!RETH.is_open()) {
        printf("Missing file RETH file!\n");
        return {};
    }
    struct RETH_Header {
        long header;
        long hashesCount;
    } RETH_Head;
    RETH.read((char*)&RETH_Head, sizeof(RETH_Head));
    if (RETH_Head.header != 544569205) {
        printf("Not a RETH file!\n");
        RETH.close();
        return {};
    }
    struct RETH_Hashes {
        unsigned long hash;
        unsigned short stringSize;
    } RETH_Hashes_r;
    string fileName;
    vector<pair<unsigned long, string>> hashList;
    for (long i = 0; i < RETH_Head.hashesCount; i++) {
        RETH.read((char*)&RETH_Hashes_r, 6);
        fileName.resize(RETH_Hashes_r.stringSize);
        RETH.read((char*)&fileName[0], RETH_Hashes_r.stringSize);
        hashList.push_back(make_pair(RETH_Hashes_r.hash, fileName));
        fileName.clear();
    }
    RETH.close();
    return hashList;
}

vector<vector<unsigned long>> readARC(string filePath) {
    ifstream ARC(filePath, ios::in | ios_base::binary);
    if (!ARC.is_open()) {
        printf("Missing file ARC!\n");
        return {};
    }
    vector<vector<unsigned long>> vecFiles;
    struct ARCHeaderSM {
        unsigned long fileSignature;
        unsigned long fileCount;
        unsigned long dataStart;
        unsigned long empty;
    } ARCHeaderSMRead;
    ARC.read((char*)&ARCHeaderSMRead, 16);
    struct ARCFilesSM {
        unsigned long hash;
        unsigned long dataPos;
        unsigned long dataSize;
        unsigned long dataSizeReal;
    } ARCFilesSMRead;

    for (unsigned long i = 0; i < ARCHeaderSMRead.fileCount; i++) {
        ARC.read((char*)&ARCFilesSMRead, sizeof(ARCFilesSMRead));
        unsigned long fileData[4] = { ARCFilesSMRead.hash, ARCFilesSMRead.dataPos, ARCFilesSMRead.dataSize, ARCFilesSMRead.dataSizeReal };
        vecFiles.push_back(vector<unsigned long>());
        for (int j = 0; j <= 3; j++) {
            vecFiles[i].push_back(fileData[j]);
        }
    }
    for (unsigned long i = 0; i < vecFiles.size(); i++) {
        for (unsigned long j = vecFiles.size(); j > i; j) {
            j--;
            if (vecFiles[i][1] == vecFiles[j][1]) {
                if (vecFiles[i][3] < vecFiles[j][3]) {
                    vecFiles.erase(vecFiles.begin() + j);
                }
                else if (vecFiles[i][3] > vecFiles[j][3]) {
                    vecFiles.erase(vecFiles.begin() + i);
                    i--;
                }
            }
        }
    }
    ARC.close();
    return vecFiles;
}

void makeRETH(string ARCfile, bool ARCTxt, char* arg, char* arg1) {
    vector<vector<unsigned long>> ARCdata;
    if (ARCTxt == true) {
        ARCdata = readARC(ARCfile);
    }

    ifstream nameList(arg, ios::in);
    string tempString;
    vector<string> realFileNames;
    while (getline(nameList, tempString)) {
        if (tempString != "") {
            transform(tempString.begin(),
            tempString.end(),
            tempString.begin(),
            [](unsigned char a)
            {return tolower(a);
            });
            realFileNames.push_back(tempString);
            tempString = "";
        }
    }
    nameList.close();
    if (realFileNames.size() == 0) { printf("No names inside \"Filenames.txt\" has been found!\n"); return; }

    fstream RETH(arg1, ios::in | ios::out | ios_base::binary);
    bool RETHexist = true;
    if (!RETH.is_open()) {
        printf("No existing RETH file! Creating a new one.\n");
        RETHexist = false;
        RETH.close();
    }
    unsigned long hashesCount = 0;

    if (RETHexist == true) {
        //addition mode
        struct RETHHeader {
            long header;
            long hashesCount;
        } RETHHead;
        RETH.read((char*)&RETHHead, sizeof(RETHHead));
        if (RETHHead.header != 544569205) {
            printf("Not a RETH file!\n");
            RETH.close();
            return;
        }

        //this checks any duplicate
        hashesCount = RETHHead.hashesCount;
        vector RETHdata = readRETH(arg1);
        for (long i = 0; i < RETHdata.size(); i++) {
            transform(RETHdata[i].second.begin(),
            RETHdata[i].second.end(),
            RETHdata[i].second.begin(),
            [](unsigned char b)
            {return tolower(b);
            });
        }
        for (long i = realFileNames.size(); i >= 0; i--) {
            for (long j = 0; j < RETHdata.size(); j++) {
                if (RETHdata[j].second == realFileNames[i]) {
                    realFileNames.erase(realFileNames.begin() + i);
                    break;
                }
            }
        }
        RETHdata = vector<pair<unsigned long, string>>();
        RETH.seekp(0, ios::end);
    } else {
        //create mode
        RETH.open(arg1, ios::out | ios_base::binary | ios::trunc);
        RETH.write("uwu ", 8);
    }
    short realFileNameSize;
    unsigned long hashedFNR;
    for (long i = 0; i < realFileNames.size(); i++) {
        if (ARCTxt == true) {
            hashedFNR = SHSMWord2Hash(realFileNames[i].c_str());
            for (long j = 0; j < ARCdata.size(); j++) {
                if (hashedFNR == ARCdata[j][0]) {
                    cout << "Found name: " << realFileNames[i] << endl;
                    realFileNameSize = realFileNames[i].size();
                    RETH.write((char*)&ARCdata[j][0], 4);
                    RETH.write((char*)&realFileNameSize, 2);
                    RETH.write((char*)&realFileNames[i][0], realFileNameSize);
                    hashesCount++;
                    break;
                }
            }
        } else {
            cout << "Added name: " << realFileNames[i] << endl;
            realFileNameSize = realFileNames[i].size();
            hashedFNR = SHSMWord2Hash(realFileNames[i].c_str());
            RETH.write((char*)&hashedFNR, 4);
            RETH.write((char*)&realFileNameSize, 2);
            RETH.write((char*)&realFileNames[i][0], realFileNameSize);
            hashesCount++;
        }
    }
    RETH.seekp(4, ios::beg);
    RETH.write((char*)&hashesCount, 4);
    RETH.close();
    return;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage:\nExtract filenames - RETH.exe <path/to/hashes.RETH> <path/to/output>\nGenerate RETH file - RETH.exe <path/to/filelist_to_hash.txt> <path/to/filelist_to_arc.txt> <path/to/output>";
        return 0;
    }
    
    if (argc == 4) {
        bool ARCSTxt = false;
        ifstream nameList(argv[1], ios::in);
        if (!nameList.is_open()) {
            printf("Missing text file!\n");
            return 0;
        }
        nameList.close();
        ifstream ARCList(argv[2], ios::in);
        vector<string> ARCPath;
        if (ARCList.is_open()) {
            ARCSTxt = true;
            string tempString;
            while (getline(ARCList, tempString)) {
                if (tempString != "") {
                    ARCPath.push_back(tempString);
                    tempString = "";
                }
            }
        }
        ARCList.close();

        if (ARCPath.size() != 0) {
            for (long i = 0; i < ARCPath.size(); i++) {
                makeRETH(ARCPath[i], ARCSTxt, argv[1], argv[3]);
            }
        } else {
            makeRETH("", ARCSTxt, argv[1], argv[3]);
        }
    } else if (argc == 3) {
        vector RETH = readRETH(argv[1]);
        ofstream txt(argv[2]);
        for (long i = 0; i < RETH.size(); i++) { txt << RETH[i].second << "\n"; }
        txt.close();
    }

    return 0;
}