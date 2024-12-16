#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>


#include <cassert>
#include <codecvt>
#include <cstdint>
#include <locale>
 
using namespace std;

unsigned long SHSMStrTableWord2Hash(string text) {
    text += "";
    unsigned long hash = 5381;
    for (int i = 0; text[i] != '\0'; i++) {
        hash = (hash * 0x21) + (tolower(text[i]) & 0xdf);
    }
    return hash;
}

vector<vector<unsigned long>> readStrFile(string filePath) {
    ifstream StrFile(filePath, ios::in | ios_base::binary);
    if (!StrFile.is_open()) {
        printf("Missing file!\n");
        return {};
    }
    StrFile.seekg(0, ios::beg);
    
    struct StrFileHeader {
        unsigned long fileSignature;
        unsigned long stringCount;
    } sStrFileHeader;

    vector<vector<unsigned long>> StrTable;

    unsigned long temp_StrHash, temp_StrPos;
    StrFile.read((char*)&sStrFileHeader, 8);
    
    for (int i = 0; i < sStrFileHeader.stringCount; i++) {
        StrTable.push_back(vector<unsigned long>());
        StrFile.read((char*)&temp_StrHash, 4);
        StrFile.read((char*)&temp_StrPos, 4);
        temp_StrPos = (temp_StrPos * 2) + (sStrFileHeader.stringCount * 8) + 12;
        StrTable[i].push_back(temp_StrHash);
        StrTable[i].push_back(temp_StrPos);
    }


    ifstream MyReadFile("text.txt");
    string text;
    vector<string> IDs;
    while (getline (MyReadFile, text)) {
        IDs.push_back(text);
    }
    char* data = new char [2];
    unsigned long hashedID;
    int y = IDs.size(), i = 0;
    bool skip = true;
    while (y > 0) {
        y--;
        hashedID = SHSMStrTableWord2Hash(IDs[y]);
        
        i = 0;
        while (i < sStrFileHeader.stringCount) {
            if (StrTable[i][0] == hashedID) {
                cout << "\ngotyat! string id (hash): " << StrTable[i][0] << "\nstring id: " << IDs[y] << "\nposition: " << StrTable[i][1] << "\n";
                skip = false;
                break;
            } else {
                skip = true;
            }
            i++;
        }
        
        if (skip != true) {
            text = "";
            StrFile.seekg(StrTable[i][1], ios::beg);
            StrFile.read(data, 2);
            while (*(unsigned short*)&data[0] != 0) {
                text += &data[0];
                StrFile.read(data, 2);
            }
            cout << text << "\n";
        }
    }
    delete[] data;
    return StrTable;
}

int main(int argc, char** argv) {
    string file = argv[1];
    vector<vector<unsigned long>> test = readStrFile(file);
    return 0;
}