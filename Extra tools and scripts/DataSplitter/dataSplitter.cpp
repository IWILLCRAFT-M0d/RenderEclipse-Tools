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

unsigned long char2Long(char* dataPos) {
    char tempChar[4];
    memcpy(tempChar, dataPos, 4);
    return *(unsigned long*)tempChar;
}

int main(int argc, char** argv) {
    string stopValue;
    if (argv[1] == NULL) {
        cout << "No file has been detected. Drag the files you want to split over the executable.\nIntroduce a value to end the program: ";
        cin >> stopValue;
        return 0;
    }

    bool Brokenfile = 0;

    for(int t=1; argv[t] != NULL; t++) {
        unsigned long pos = 0, chunkSel = 0, fullFileSize, animationFormatCheck;
        bool BigEndian = false;
        ifstream fileLoaded(argv[t], ios::in | ios_base::binary | ios::ate);
        string mainFileName = path(argv[t]).filename().string();

        fullFileSize = fileLoaded.tellg();
        fileLoaded.seekg(0, ios::beg);
        char* fileData, *fullData = new char [fullFileSize], tempChar[4];
        fileLoaded.read(fullData, fullFileSize);
        fileLoaded.close();

        struct sRWStruct {
            unsigned long ID;
            unsigned long chunkSize;
            unsigned long RWID;
        } RWStruct;


        struct sfileType {
            unsigned short rwID_TEXDICTIONARY = 0;
            unsigned short rwID_AUDIODATA = 0;
            unsigned short rwID_AUDIOCUES = 0;
            unsigned short rwID_STATETRANSITION = 0;
            unsigned short rwID_HANIMANIMATION = 0;
            unsigned short rwID_SPLINE = 0;
            unsigned short rwID_RWS = 0;
            unsigned short rwID_WORLD = 0;
            unsigned short rwID_CLUMP = 0;
            unsigned short rwID_DMORPHANMSTREAM = 0;
            unsigned short Unknown = 0;
        } fileType;


        cout << "Starting File Data Spliting | File name: " << mainFileName << "\n";
        if (!is_directory("./Extracted Data/") || !exists("./Extracted Data/")) { create_directory("./Extracted Data/");  create_directory("./Extracted Data/Textures"); }
        do {
            memcpy(&RWStruct, fullData+pos, 12); BigEndian = false; pos += 12; chunkSel++;
            cout << "\n-- Chunk selected: " << chunkSel << "\n";
            

            if (RWStruct.ID == 1814) {
                unsigned long fileInfo = char2Long(fullData+pos), nextChunkPos = RWStruct.chunkSize + pos;
                pos += 4;

                if (fileInfo > 4096) {
                    BigEndian = true;
                }

                unsigned long fileNameSize = changeEndian(char2Long(fullData+pos), BigEndian);
                string fileName = "", RWIDString = "";
                for(int i = 4; fullData[pos+i] != '\0'; i++) {
                    fileName += fullData[pos+i];
                }
                if (fileName != "") {
                    cout << "- File name: " << fileName << endl;
                }

                unsigned long fileTypeSize = changeEndian(char2Long(fullData + 20 + fileNameSize), BigEndian);
                for(int i = 24 + fileNameSize; fullData[pos+i] != '\0'; i++) {
                    RWIDString += fullData[pos+i];
                }
                cout << "- RW ID: " << RWIDString << endl;
                if (fileName == "") {
                    if (RWIDString == "rwID_TEXDICTIONARY") {
                        fileName = mainFileName + "_" + to_string(fileType.rwID_TEXDICTIONARY) + ".txd";
                        fileType.rwID_TEXDICTIONARY += 1;
                    } else if (RWIDString == "rwID_AUDIODATA") {
                        fileName = mainFileName + "_" + to_string(fileType.rwID_AUDIODATA) + ".snd";
                        fileType.rwID_AUDIODATA += 1;
                    } else if (RWIDString == "rwID_AUDIOCUES") {
                        fileName = mainFileName + "_" + to_string(fileType.rwID_AUDIOCUES) + ".cue";
                        fileType.rwID_AUDIOCUES += 1;
                    } else if (RWIDString == "rwID_STATETRANSITION") {
                        fileName = mainFileName + "_" + to_string(fileType.rwID_STATETRANSITION) + ".rst";
                        fileType.rwID_STATETRANSITION += 1;
                    } else if (RWIDString == "rwID_HANIMANIMATION") {
                        fileName = mainFileName + "_" + to_string(fileType.rwID_HANIMANIMATION) + ".anm";
                        fileType.rwID_HANIMANIMATION += 1;
                    } else if (RWIDString == "rwID_SPLINE") {
                        fileName = mainFileName + "_" + to_string(fileType.rwID_SPLINE) + ".spl";
                        fileType.rwID_SPLINE += 1;
                    } else if (RWIDString == "rwID_RWS" || RWIDString == "rwpID_BODYDEF") {
                        fileName = mainFileName + "_" + to_string(fileType.rwID_RWS) + ".rws";
                        fileType.rwID_RWS += 1;
                    } else if (RWIDString == "rwID_WORLD") {
                        fileName = mainFileName + "_" + to_string(fileType.rwID_WORLD) + ".bsp";
                        fileType.rwID_WORLD += 1;
                    } else if (RWIDString == "rwID_CLUMP") {
                        fileName = mainFileName + "_" + to_string(fileType.rwID_CLUMP) + ".dff";
                        fileType.rwID_CLUMP += 1;
                    } else if (RWIDString == "rwID_DMORPHANMSTREAM") {
                        fileName = mainFileName + "_" + to_string(fileType.rwID_DMORPHANMSTREAM) + ".dma";
                        fileType.rwID_DMORPHANMSTREAM += 1;
                    } else {
                        fileName = mainFileName + "_" + to_string(fileType.Unknown) + ".bin";
                        fileType.Unknown += 1;
                    }
                }

                pos += changeEndian(fileInfo, BigEndian);
                unsigned long fileSize = changeEndian(char2Long(fullData+pos), BigEndian);


                fileData = new char[fileSize];
                pos += 4;

                if (RWIDString == "rwID_HANIMANIMATION") {
                    animationFormatCheck = char2Long(fullData+pos+16);
                    switch (animationFormatCheck) {
                        case 1:
                        case 2:
                            cout << "- Base RenderWare Animation format version\n";
                            break;
                        case 4355:
                            cout << "- General Climax games format version\n";
                            break;
                        case 4357:
                            cout << "- Silent Hill: Shattered Memories version\n";
                            break;
                        default:
                            cout << "- Unknown format version\n";
                            break;
                    }
                }

                memcpy(fileData, fullData+pos, fileSize);
                ofstream fileExt("./Extracted Data/" + fileName, ios::out | ios::binary | ios::trunc);
                fileExt.write(fileData, fileSize);
                fileExt.close();
                delete[] fileData;
                pos = nextChunkPos;
            } else if (RWStruct.ID == 3841) {
                cout << "Shattered Memories *.SND file\n";
                pos += 72;
                unsigned long FEVSize = char2Long(fullData+pos), FEVPos = char2Long(fullData+pos+4) + 20;
                string FEVFile = "", FSBFile = "";
                for(int i = 8; fullData[pos+i] != '\0'; i++) {
                    FEVFile += fullData[pos+i];
                }

                pos += 72;
                unsigned long FSBSize = char2Long(fullData+pos), FSBPos = char2Long(fullData+pos+4) + 20;
                for(int i = 8; fullData[pos+i] != '\0'; i++) {
                    FSBFile += fullData[pos+i];
                }
                

                cout << "- File names: " << FEVFile  << " + " << FSBFile << endl;
                
                char* FEVData = new char[FEVSize];
                memcpy(FEVData, fullData+FEVPos, FEVSize);
                ofstream FEVFileExt("./Extracted Data/" + FEVFile, ios::out | ios::binary | ios::trunc);
                FEVFileExt.write(FEVData, FEVSize);
                FEVFileExt.close();
                delete[] FEVData;

                char* FSBData = new char[FSBSize];
                memcpy(FSBData, fullData+FSBPos, FSBSize);
                ofstream FSBFileExt("./Extracted Data/" + FSBFile, ios::out | ios::binary | ios::trunc);
                FSBFileExt.write(FSBData, FSBSize);
                FSBFileExt.close();
                delete[] FSBData;
                break;
            } else {
                if (RWStruct.ID == 1820) {
                    cout << "Origins RenderWare Stream file attributes\n";
                } else if (RWStruct.ID == 1796) {
                    cout << "Climax special attributes?\n";
                } else if (RWStruct.ID == 0) {
                    cout << "Empty file\n";
                }
                fileData = new char[RWStruct.chunkSize];
                memcpy(fileData, fullData+pos-12, RWStruct.chunkSize);
                ofstream fileExt("./Extracted Data/" + mainFileName + "_" + to_string(fileType.Unknown) + ".bin", ios::out | ios::binary | ios::trunc);
                fileType.Unknown += 1;
                fileExt.write(fileData, RWStruct.chunkSize);
                fileExt.close();
                delete[] fileData;

                pos += RWStruct.chunkSize;
            }
        } while (pos < fullFileSize);
        
    }
    cout << "\n\nData has been fully extracted - close the window or introduce a value to end the program: ";
    cin >> stopValue;
    return 0;
}