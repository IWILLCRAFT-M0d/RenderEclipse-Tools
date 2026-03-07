#pragma once
#include <vector>

namespace RETH {
    unsigned long SHSMWord2Hash(std::string text);
    std::vector<std::pair<unsigned long, std::string>> readNames(std::string filePath, std::vector<std::vector<unsigned long>> ARCdata);
}