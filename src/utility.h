#pragma once
#include<iostream>
#include<vector>
#include<fstream>
#include<optional>
#include<sstream>
#include<string>
#include<cstdlib>
#include<boost/filesystem.hpp>
#include<boost/program_options.hpp>
//#include <toml.hpp>
#include <toml++/toml.hpp> 
#include"logger.h"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif



namespace fs = boost::filesystem;
namespace po = boost::program_options;
const char csv_delimetr = ';';

fs::path GetExecutablePath();

void ParseCommandString(int argc, char* argv[],std::string& config_path);

struct ParsedToml{
    fs::path input_path;
    fs::path output_path;
    std::vector<std::string> masks;
};

void ParseToml(const std::string& path, ParsedToml& parsed_struct);

void GetCvsFiles(const ParsedToml& parsed_toml,std::vector<fs::path>& files);

void OpenFiles(const std::vector<fs::path>& files, std::vector<std::ifstream>& file_stream);

enum class Side{Ask,Bid,Unknown};

struct ParsedCsvLine{
    uint64_t receive_ts;
    uint64_t exchange_ts;
    double price;
    double quantity;
    Side side=Side::Unknown;
    std::optional<bool> build=std::nullopt;
};

ParsedCsvLine ParseString(std::string& line);