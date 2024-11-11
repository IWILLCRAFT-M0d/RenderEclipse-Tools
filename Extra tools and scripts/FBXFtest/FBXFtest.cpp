#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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

    bool singleAtr = false;
    char tempChar[4];
    unsigned data;
    string text;
    string extratext;
    vector<string> tags;
    unsigned tagsCount;
    bool looped;
    for (int i = 0; i < XMLHeaderRead.dataStart; i += 4) {
        data = char2long(fileData+i);
        if (XMLHeaderRead.dataStart-16 <= data && data <= XMLHeaderRead.fileSize-16) {
            cout << "\n";
            if (data-17 == XMLHeaderRead.dataStart || fileData[data-15] == '\0' || fileData[data-17] == '\0') {
                for (int y = data-16; y < XMLHeaderRead.fileSize; y++) {
                    if (fileData[y] != '\0') {
                        text += fileData[y];
                    } else {
                        tags.push_back(text);
                        tagsCount++;
                        cout << "<" << text;
                        text = "";
                        data = char2long(fileData+i+4);
                        for (i; data == 14 || data == 30 || data == 98310 || data == 98318 || data == 98334; i+=12) {
                            looped = false;
                            singleAtr = false;
                            if (data == 14) {
                                singleAtr = true;
                                extratext = " ";
                            } else {
                                extratext = "\n";
                            }
                            data = char2long(fileData+(i+8))-16;
                            for (int t = data; true; t++) {
                                if (fileData[t] != '\0') {
                                    text += fileData[t];
                                } else {
                                    if (looped) {
                                        cout << text << "\"";
                                        text = "";
                                        break;
                                    } else {
                                        cout << extratext << text << "=\"";
                                        t = char2long(fileData+(i+12))-17;
                                        text = "";
                                        looped = true;
                                    }
                                }
                            }
                            data = char2long(fileData+(i+16));
                        }
                        if (tagsCount != 1) {
                            cout << "\\>\n";
                        } else {
                            cout << ">\n";
                        }
                        if (singleAtr == true) {
                            cout << "--- single atribute, nothing within\n";
                        }
                        break;
                    }
                }
            }
        }
    }
    cout << "\n<" << tags[0] << "\\>";
    delete[] fileData;
    file.close();
    cout << "\nclose the window or introduce a value to end the program";
    cin >> stopValue;
    return 0;
}