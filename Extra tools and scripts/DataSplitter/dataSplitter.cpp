#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <Windows.h>

using namespace std;
using namespace std::filesystem;

unsigned long changeEndian(unsigned long value, bool trueornot) {
    if (trueornot == 0) {
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
        ifstream fileLoaded(argv[t], ios::in | ios_base::binary | ios::ate);

        string mainFileName = path(argv[t]).filename().string();

        unsigned long fullFileSize = fileLoaded.tellg();
        fileLoaded.seekg(0, ios::beg);
        char* fullData = new char [fullFileSize];
        fileLoaded.read(fullData, fullFileSize);
        fileLoaded.close();

        // this is very unpractical but seems to work -;=;-
        short SHSMData = 0;
        if ((fullFileSize % 2048) == 0) {
            SHSMData = 1;
        }
        
        char tempChar[4];
        long unsigned pos = 0;
        long unsigned chunkSel = 0;
        long unsigned sussisesval1 = 0;
        char* fileData;
        cout << "Starting File Data Spliting | File name: " << mainFileName << "\n\n";
        if (!is_directory("./Extracted Data/") || !exists("./Extracted Data/")) { create_directory("./Extracted Data/");  create_directory("./Extracted Data/Textures"); }
        do {
            long unsigned header = char2Long(fullData+pos);
            chunkSel++;
            cout << "-- Chunk selected: " << chunkSel << endl;
            if (SHSMData == 1) { while (pos % 4) {pos += 1;} }
            pos += 4;
            long unsigned chunkSize = char2Long(fullData+pos);
            pos += 8;
            while(true) {
                if (((chunkSize > 0) && (chunkSize < 32) && pos + 4 < fullFileSize) || (chunkSize > fullFileSize)) {
                    if (SHSMData == 1) { while (pos % 4) {pos += 1;} }
                    pos += 4;
                    chunkSize = char2Long(fullData+pos);
                    pos += 8;
                } else if (pos >= fullFileSize) {
                    Brokenfile = 1;
                    break;
                } else {
                    break;
                }
            }

            if (Brokenfile == 1) {
                cout << "The file cannot be fully readed\n";
                break;
            }

            if (header == 1814) {
                long unsigned fileInfo = char2Long(fullData+pos);
                pos += 4;

                bool BigEndian = false;
                if (fileInfo < 1024) {
                    BigEndian = true;
                }

                long unsigned fileNameSize = changeEndian(char2Long(fullData+pos), BigEndian);
                string fileName;
                if (fileNameSize > 4) {
                    for(int i=4; fullData[pos+i] != '\0'; i++) {
                        fileName += fullData[pos+i];
                        if (i >= fileNameSize+4) { break; }
                    }
                    cout << "- File name: " << fileName << endl;
                } else {
                    long unsigned fileTypeRead = changeEndian(char2Long(fullData+pos+24), BigEndian);
                    for(int i=28; fullData[pos+i] != '\0'; i++) {
                        fileName += fullData[pos+i];
                        if (i >= fileTypeRead+28) { break; }
                    }
                    cout << "- RW Id: " << fileName << endl;
                    if (fileName == "rwID_TEXDICTIONARY") {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".txd";
                    } else if (fileName == "rwID_AUDIODATA") {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".snd";
                    } else if (fileName == "rwID_AUDIOCUES") {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".cue";
                    } else if (fileName == "rwID_STATETRANSITION") {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".rst";
                    } else if (fileName == "rwID_HANIMANIMATION") {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".anm";
                    } else if (fileName == "rwID_SPLINE") {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".spl";
                    } else if (fileName == "rwID_RWS" || fileName == "rwpID_BODYDEF") {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".rws";
                    } else if (fileName == "rwID_WORLD") {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".bsp";
                    } else if (fileName == "rwID_CLUMP") {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".dff";
                    } else if (fileName == "rwID_DMORPHANMSTREAM") {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".dma";
                    } else {
                        fileName = mainFileName + "_" + to_string(chunkSel) + ".bin";
                    }
                }
                pos += changeEndian(fileInfo, BigEndian);
                long unsigned fileSizeNonChunk = changeEndian(char2Long(fullData+pos), BigEndian);
                // cout << "File Size (no chunk) " << fileSizeNonChunk << endl;


                fileData = new char[fileSizeNonChunk];
                pos += 4;
                memcpy(fileData, fullData+pos, fileSizeNonChunk);
                ofstream fileExt("./Extracted Data/" + fileName, ios::out | ios::binary | ios::trunc);
                fileExt.write(fileData, fileSizeNonChunk);
                fileExt.close();
                delete[] fileData;
                pos += fileSizeNonChunk;
                
                
                if (SHSMData == 1) { while (pos % 4) {pos += 1;} }
                // cout << "Actual position: " << pos << " | Full chunk size: " << chunkSize+12 <<endl;
            } else {
                fileData = new char[chunkSize];
                memcpy(fileData, fullData+pos, chunkSize);
                ofstream fileExt("./Extracted Data/" + mainFileName + "_" + to_string(chunkSel) + ".bin", ios::out | ios::binary | ios::trunc);
                fileExt.write(fileData, chunkSize);
                fileExt.close();
                delete[] fileData;

                pos += chunkSize;
            }
            // cout << "Chunk has been ended | Actual Position: " << pos << " | File size: " << fullFileSize <<endl;
        } while (pos < fullFileSize);
        
    }
    cout << "\n\nData has been fully extracted - close the window or introduce a value to end the program: ";
    cin >> stopValue;
    return 0;
}