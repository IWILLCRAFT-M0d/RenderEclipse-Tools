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
    file.seekg(0, ios::beg);
    file.read(fileData, XMLHeaderRead.fileSize);

    unsigned long data;
    bool looped;
    char tempChar[4];
    string text, extratext;
    vector<string> tags;
    for (unsigned long i = 16; i < XMLHeaderRead.dataStart; i += 4) {
        data = char2long(fileData+i);
        if (XMLHeaderRead.dataStart <= data && data <= XMLHeaderRead.fileSize) {
            cout << "\n";
            if (data == XMLHeaderRead.dataStart || fileData[data-1] == '\0') {
                for (unsigned long y = data; y < XMLHeaderRead.fileSize; y++) {
                    if (fileData[y] != '\0') {
                        text += fileData[y];
                    } else {
                        cout << "<" << text;
                        
                        
                        tags.push_back(text); text = ""; i += 4;


                        // data = char2long(fileData+i);
                        data = (char2long(fileData+i) & 6) >> 1;

                        for (i; data == 3; i+=12) {
                            looped = false;
                            data = char2long(fileData+i+4);
                            while (true) {
                                if (fileData[data] != '\0') {
                                    text += fileData[data]; data++;
                                } else {
                                    if (!looped) {
                                        cout << "\n" << text << "=\"";
                                        data = char2long(fileData+(i+8));
                                        text = "";
                                        looped = true;
                                    } else {
                                        cout << text << "\"";
                                        text = "";
                                        break;
                                    }
                                }
                            }
                            if (i+12 >= XMLHeaderRead.dataStart) {
                                i += 12;
                                break;
                            }
                            data = (char2long(fileData+i+12) & 6) >> 1;
                        }
                        cout << ">\n";
                        break;
                    }
                }
            }
        }
    }
    // cout << "\n<" << tags[0] << "\\>";
    delete[] fileData;
    file.close();
    cout << "\nclose the window or introduce a value to end the program";
    cin >> stopValue;
    return 0;
}