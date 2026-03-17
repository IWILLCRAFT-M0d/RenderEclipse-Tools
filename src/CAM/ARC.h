#pragma once
#include <fstream>
#include <vector>
#include <wx/string.h>

#define ARC_SIGNATURE_SOLENT 808333889
#define ARC_SIGNATURE_SHSM   64016
#define RETH_SIGNATURE       544569205u

#define ARC_IsType(id) \
    ARC::loadedARC_Info.type == id

#define ARC_IsNotType(id) \
    ARC::loadedARC_Info.type != id

namespace RETH {
    struct header {
        unsigned long header;
        unsigned long hashesCount;
    };
    struct hashes {
        unsigned long hash;
        unsigned short stringSize;
    };
}

namespace ARC {
    enum type {
        type_LA     = 0, // No file signature - Climax Los Angeles         | Origins (Beta), TEST: Oblivion
        type_Solent = 1, // A2.0              - Climax Solent (PortsMouth) | Origins, Ghost Rider
        type_SM     = 2  // 0x10FA0000        - Climax Solent (PortsMouth) | Shattered Memories
    };
    
    struct header {
        unsigned long fileCount;
        unsigned long dataStart;
        unsigned long namesPos;
        unsigned long namesSize;
    };

    struct fileEntryInfo {
        unsigned long fileName;
        unsigned long dataPos;
        unsigned long dataSize;
        unsigned long dataSizeReal;
    };
    
    struct fileNameEntry {
        unsigned long hash;
        std::string   filename;
    };

    struct loadedInfo {
	    wxString    fileItemSelected;
        std::string pathFileLoaded;
        type        type;

	    std::vector<fileEntryInfo> fileData;
	    std::vector<fileNameEntry> fileNames;
    };

    // Variables
    extern loadedInfo loadedARC_Info;


	unsigned long endianChangeULong(std::string EBigToELittle);
	std::string endianChangeString(unsigned long ELittleToEBig);

    // Functions
	bool read(void);
	void exportFile(bool exportAllFiles = true);
	void importFile(std::string fileImportPath);
    void readFilenames(std::string filePath);

    unsigned long SHSM_FilenameHash(std::string text);
}