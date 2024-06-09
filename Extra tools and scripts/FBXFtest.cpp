#include <iostream>
#include <fstream>
#include <string>

using namespace std;

unsigned long char2long(char* dataPos) {
    char tempChar[4];
    memcpy(tempChar, dataPos, 4);
    return *(unsigned long*)tempChar;
}

int main(int argc, char** argv) {
    ifstream file(argv[1], ios::in | ios_base::binary);
    string stopValue;
    struct XMLHeader {
        unsigned long unk;
        unsigned long dataStart;
        unsigned long fileSignature;
        unsigned long fileSize;
    } XMLHeaderRead;
    file.read((char*)&XMLHeaderRead, 16);
    if (XMLHeaderRead.fileSignature != 1180189254) {
        file.close();
        return 0;
    }

    char* fileData = new char [XMLHeaderRead.fileSize];
    file.read(fileData, XMLHeaderRead.fileSize);

    char tempChar[4];
    unsigned data;
    string text;
    bool looped;
    for (int i = 0; i < XMLHeaderRead.dataStart; i += 4) {
        data = char2long(fileData+i);
        if (XMLHeaderRead.dataStart-16 <= data && data <= XMLHeaderRead.fileSize-16) {
            cout << "\n";
            for (int y = data-16; y < XMLHeaderRead.fileSize; y++) {
                if (fileData[y] != '\0') {
                    text += fileData[y];
                } else {
                    cout << "<" << text << "/>\n";
                    text = "";
                    break;
                }
            }
        } else if (data == 14 || data == 30 || data == 98310 || data == 98318 || data == 98334) {
            looped = false;
            data = char2long(fileData+i+4)-16;
            for (int y = data; true; y++) {
                if (fileData[y] != '\0') {
                    text += fileData[y];
                } else {
                    if (looped) {
                        cout << text << "\"\n";
                        text = "";
                        break;
                    } else {
                        cout << text << "=\"";
                        y = char2long(fileData+i+8)-17;
                        text = "";
                        looped = true;
                    }
                }
            }
            i += 8;
        }
    }

    delete[] fileData;
    file.close();
    cout << "\nclose the window or introduce a value to end the program";
    cin >> stopValue;
    return 0;
}