#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <Windows.h>

using namespace std;
using namespace std::filesystem;

unsigned long changeEndian(unsigned long value, bool trueornot) {
    if (trueornot == true) {
        return (((value << 24) & 0xff000000)
            | ((value << 8) & 0xff0000)
            | ((value >> 8) & 0xff00)
            | ((value >> 24) & 0xff));
    } else {
        return value;
    }
}

unsigned long char2ULong(char* dataPos) {
    char tempChar[4];
    memcpy(tempChar, dataPos, 4);
    return *(unsigned long*)tempChar;
}

unsigned long char2UShort(char* dataPos) {
    char tempChar[2];
    memcpy(tempChar, dataPos, 2);
    return *(unsigned short*)tempChar;
}

int main(int argc, char** argv) {
    string stopValue;
    if (argv[1] == NULL) {
        cout << "No file has been detected. Drag the files you want to split over the executable.\nIntroduce a value to end the program: ";
        cin >> stopValue;
        return 0;
    }

    for(int t=1; argv[t] != NULL; t++) {
        unsigned long pos = 0, fullFileSize, animationFormatCheck;
        bool BigEndian = false;
        ifstream fileLoaded(argv[t], ios::in | ios_base::binary | ios::ate);
        string mainFileName = path(argv[t]).filename().string();

        fullFileSize = fileLoaded.tellg();
        fileLoaded.seekg(0, ios::beg);
        char* fileData, *fullData = new char [fullFileSize], tempChar[4];
        fileLoaded.read(fullData, fullFileSize);
        fileLoaded.close();

        
        struct sIGCStruct {
            unsigned long headerSize;
            unsigned long dataSize;
            float  cutsceneDuration; // this is a float - cutscene duration???
            unsigned short objectCount; // object amount
            unsigned short animCount; // animation amount - if 0 all objects are animations
        } IGCStruct;


        struct sHeaderDataUnk0 {
            float data_unk_0;
            float data_unk_1;
            unsigned short data_unk_2;
        }headerData;


        cout << "Starting File Data Spliting | File name: " << mainFileName << "\n";
        if (!is_directory("./Extracted IGCStream Data/") || !exists("./Extracted IGCStream Data/")) { create_directory("./Extracted IGCStream Data/"); }

        memcpy(&IGCStruct, fullData+pos, 16); BigEndian = false; pos += 16;


        unsigned long i;
        for (i = 0; i < IGCStruct.objectCount; i++) {
            memcpy(&headerData, fullData+pos, 10); pos += 10;
        }
        for (i = 0; i < IGCStruct.objectCount; i++) {
            cout << pos << " | " << i <<"\n";
            if (IGCStruct.animCount == 0 || i < IGCStruct.animCount) {
                unsigned long data_unk_0 = char2ULong(fullData+pos); pos += 6;
                
                if (data_unk_0 == 1) {
                    pos += 8;
                    unsigned short data_unk_4 = char2UShort(fullData+pos); pos += 8;
                }
                
                if (data_unk_0 != 0) {
                    unsigned short anim_object = char2UShort(fullData+pos); pos += 2 + anim_object;
                    unsigned short anim_file = char2UShort(fullData+pos); pos += 2 + anim_file;
                    unsigned short anim_facial = char2UShort(fullData+pos); pos += 4 + anim_facial;
                    unsigned short command_0 = char2UShort(fullData+pos); pos += 2 + command_0;
                    unsigned short command_1 = char2UShort(fullData+pos); pos += 2 + command_1;
                } else {
                    unsigned short anim_file = char2UShort(fullData+pos);  pos += 2 + anim_file;
                    unsigned short unk_string_0 = char2UShort(fullData+pos);  pos += 7 + unk_string_0;
                }
            } else {
                unsigned short data_unk_0 = char2UShort(fullData+pos);
                if (data_unk_0 != 4) {
                    unsigned long  data_unk_0 = char2ULong(fullData+pos); pos += 6;
                    
                    if (data_unk_0 == 1) {
                        pos += 8;
                        unsigned short data_unk_4 = char2UShort(fullData+pos); pos += 8;
                    }
                    
                    unsigned short fileName = char2UShort(fullData+pos); pos += 2 + fileName;
                    pos += 4;
                    unsigned short data_unk_4 = char2UShort(fullData+pos); pos += 6;
                    unsigned short FEVdataSize = char2UShort(fullData+pos); pos += 2 + FEVdataSize;
                } else {
                    pos += 2;
                    unsigned short command = char2UShort(fullData+pos); pos += 4 + command;
                }
            }
        }



    cout << pos << "\n";

        //fileData = new char[RWStruct.chunkSize];
        //memcpy(fileData, fullData+pos-12, RWStruct.chunkSize);
        //ofstream fileExt("./Extracted Data/" + mainFileName + "_" + to_string(fileType.Unknown) + ".bin", ios::out | ios::binary | ios::trunc);
        //fileType.Unknown += 1;
        //fileExt.write(fileData, RWStruct.chunkSize);
        //fileExt.close();
        //delete[] fileData;
        
    }
    cout << "\n\nData has been fully extracted - close the window or introduce a value to end the program: ";
    cin >> stopValue;
    return 0;
}
