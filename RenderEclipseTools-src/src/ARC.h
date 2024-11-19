#pragma once
#include <fstream>
#include <vector>
#include <wx/string.h>

class classValuesARC {
    public:
	    wxString fileItemSelected;
	    std::string pathFileLoaded;
	    std::vector<std::vector<unsigned long>> ARCdata;
	    std::vector<std::pair<unsigned long, std::string>> RETH;
	    unsigned short fileNameTest;
	    unsigned long rethSize;
};

extern classValuesARC valuesARC;

namespace ARC {
	extern unsigned int ARCType;
	extern bool compressImport;


	std::vector<std::vector<unsigned long>> readARC(std::string filePath);
	void extractFunc(bool All = true);
	void importFunc(std::string fileImportPath);
	unsigned long endianChangeULong(std::string EBigToELittle);
	std::string endianChangeString(unsigned long ELittleToEBig);
}