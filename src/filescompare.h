#include <boost/filesystem.hpp>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>

namespace fs = boost::filesystem;

std::string Normalize(const std::string& line)
{
    std::string trimmed;
    trimmed.reserve(line.size());
    for (unsigned char c : line)
    {
        if (!std::isspace(c))
            trimmed.push_back(c);
    }
    return trimmed;
}
bool ReadNextNormalizedLine(std::ifstream& file, std::string& out)
{
    std::string line;
    while (std::getline(file, line))
    {
        line = Normalize(line);
        if (!line.empty()) {
            out = std::move(line);
            return true;
        }
    }
    return false;
}

bool CompareFiles(const fs::path& file1, const fs::path& file2)
{
    std::ifstream f1(file1.string());
    std::ifstream f2(file2.string());

    if (!f1.is_open() || !f2.is_open())
        return false;
    std::string line1, line2;

    while (true)
    {
        bool has1 = ReadNextNormalizedLine(f1, line1);
        bool has2 = ReadNextNormalizedLine(f2, line2);
        if (!has1 && !has2)
            return true;
        if (has1 != has2)
            continue; 
        if (line1 != line2)
            return false;
    }
}