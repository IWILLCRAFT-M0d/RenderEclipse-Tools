#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <Windows.h>

using namespace std;
using namespace std::filesystem;

unsigned long readULong(char* dataPos, bool BigEndians = false) {
    char tempChar[4];
    memcpy(tempChar, dataPos, 4);
    unsigned long tempInt = *(unsigned long*)tempChar;
    if (BigEndians == true) {
        tempInt = (
            ((tempInt << 24) & 0xff000000)
            | ((tempInt << 8) & 0xff0000)
            | ((tempInt >> 8) & 0xff00)
            | ((tempInt >> 24) & 0xff)
            );
    }
    return tempInt;
}

string readRWString (char* data, unsigned long Position, unsigned long fileNameSize){
    string RWString;
    unsigned long fileNameSizeEnd = Position + fileNameSize;
    while (data[Position] != '\0' && Position < fileNameSizeEnd) {
        RWString += data[Position];
        Position++;
    }
    return RWString;
}

int main(int argc, char** argv) {
    string stopValue;
    if (argv[1] == NULL) {
        cout << "No file has been detected. Drag the files you want to split over the executable.\nIntroduce a value to end the program: ";
        cin >> stopValue;
        return 0;
    }
    struct fileInfo {
        unsigned long RW_TXD = 0;
        unsigned long RW_BSP = 0;
        unsigned long RW_Audio = 0;
        unsigned long RW_Cues = 0;
        unsigned long RW_StateTrans = 0;
        unsigned long RW_Anim = 0;
        unsigned long RW_Spline = 0;
        unsigned long RW_Clump = 0;
        unsigned long RW_DMA = 0;
        unsigned long RW_RWS = 0;
        unsigned long RWSAtr = 0;
        unsigned long Unk = 0;
    } structFileInfo;


    for(int t=1; argv[t] != NULL; t++) {
        ifstream fileLoaded(argv[t], ios::in | ios_base::binary | ios::ate);

        string mainFileName = path(argv[t]).filename().string();

        unsigned long fullFileSize = fileLoaded.tellg();
        fileLoaded.seekg(0, ios::beg);
        char* fullData = new char [fullFileSize];
        fileLoaded.read(fullData, fullFileSize);
        fileLoaded.close();
    
        bool BigEndian = false;
        unsigned long pos = 0;
        unsigned long chunkSel = 0;
        char* fileData;
        cout << "Starting File Data Spliting | File name: " << mainFileName << "\n\n";
        if (!is_directory("./Extracted Data/") || !exists("./Extracted Data/")) { create_directory("./Extracted Data/");  create_directory("./Extracted Data/Textures"); }
        do {
            unsigned long header = readULong(fullData+pos);
            unsigned long chunkSize = readULong(fullData+(pos += 4));
            
            unsigned long chunkEnd = pos + 8 + chunkSize;

            chunkSel++;
            cout << "-- Chunk selected: " << chunkSel << "\n";

            switch (header) {
                case 1814: {
                    cout << "- RenderWare Stream file\n";
                    unsigned long fileInfo = readULong(fullData+(pos += 8), BigEndian);


                    if (fileInfo > 1024 && BigEndian == false) {
                        BigEndian = true;
                        fileInfo = readULong(fullData+pos, BigEndian);
                    }

                    unsigned long preFileData = pos + 4 + fileInfo;
                    unsigned long fileNameSize = readULong(fullData+(pos+=4), BigEndian);
                    string fileName = readRWString(fullData, pos += 4, fileNameSize);
                    if (fileName == "") {
                        unsigned long fileTypeRead = readULong(fullData+(pos+=24), BigEndian);
                        string RWID = readRWString(fullData, pos, fileTypeRead);
                        cout << "- RenderWare ID: " << RWID << "\n";
                        fileName = mainFileName + "_";
                        if (RWID == "rwID_TEXDICTIONARY") {
                            fileName += to_string(structFileInfo.RW_TXD++) + ".txd";
                        } else if (RWID == "rwID_AUDIODATA") {
                            fileName += to_string(structFileInfo.RW_Audio++) + ".snd";
                        } else if (RWID == "rwID_AUDIOCUES") {
                            fileName += to_string(structFileInfo.RW_Cues++) + ".cue";
                        } else if (RWID == "rwID_STATETRANSITION") {
                            fileName += to_string(structFileInfo.RW_StateTrans++) + ".rst";
                        } else if (RWID == "rwID_HANIMANIMATION") {
                            fileName += to_string(structFileInfo.RW_Anim++) + ".anm";
                        } else if (RWID == "rwID_SPLINE") {
                            fileName += to_string(structFileInfo.RW_Spline++) + ".spl";
                        } else if (RWID == "rwID_RWS" || fileName == "rwpID_BODYDEF") {
                            fileName += to_string(structFileInfo.RW_RWS++) + ".rws";
                        } else if (RWID == "rwID_WORLD") {
                            fileName += to_string(structFileInfo.RW_BSP++) + ".bsp";
                        } else if (RWID == "rwID_CLUMP") {
                            fileName += to_string(structFileInfo.RW_Clump++) + ".dff";
                        } else if (RWID == "rwID_DMORPHANMSTREAM") {
                            fileName += to_string(structFileInfo.RW_DMA++) + ".dma";
                        } else {
                            fileName += to_string(structFileInfo.Unk++) + ".bin";
                        }
                    } else {
                        cout << "- File name: " << fileName << endl;
                    }
                    unsigned long fileSizeNonChunk = readULong(fullData+preFileData, BigEndian);
                    pos = chunkEnd;


                    fileData = new char[fileSizeNonChunk];
                    memcpy(fileData, fullData+(preFileData + 4), fileSizeNonChunk);
                    ofstream fileExt("./Extracted Data/" + fileName, ios::out | ios::binary | ios::trunc);
                    fileExt.write(fileData, fileSizeNonChunk);
                    fileExt.close();
                    delete[] fileData;
                    break;
                } default: {
                    fileData = new char[chunkSize];
                    memcpy(fileData, fullData+pos, chunkSize);
                    string fileName = "./Extracted Data/" + mainFileName + "_";
                    if (header == 1796) {
                        cout << "- RWS Attribute?\n";
                        fileName += "Atr_" + to_string(structFileInfo.RWSAtr++) + ".bin";
                    } else {
                        fileName += to_string(structFileInfo.Unk++) + ".bin";
                    }
                    ofstream fileExt(fileName, ios::out | ios::binary | ios::trunc);
                    fileExt.write(fileData, chunkSize);
                    fileExt.close();
                    delete[] fileData;
                    pos = chunkEnd;
                    break;
                }
                
            }
        } while (pos < fullFileSize);
        
    }
    cout << "\n\nData has been fully extracted - close the window or introduce a value to end the program: ";
    cin >> stopValue;
    return 0;
}