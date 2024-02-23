#pragma once
#include <fstream>
#include <vector>

unsigned long endianChangeULong(std::string EBigToELittle);
std::string endianChangeString(unsigned long ELittleToEBig);
std::vector<std::vector<unsigned long>> readARC(std::string filePath);
extern unsigned int ARCType;