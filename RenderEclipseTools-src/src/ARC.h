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

unsigned long endianChangeULong(std::string EBigToELittle);
std::string endianChangeString(unsigned long ELittleToEBig);
std::vector<std::vector<unsigned long>> readARC(std::string filePath);
extern unsigned int ARCType;
void extractFunc(classValuesARC valuesARC, bool All = true);