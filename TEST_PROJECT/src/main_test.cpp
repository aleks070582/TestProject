#include<iostream>
#include<unordered_map>
#include<string>
#include<vector>
#include<algorithm>
#include<iomanip>
#include<boost/program_options.hpp>
#include<boost/filesystem.hpp>
#include <toml++/toml.hpp>
#include"utility.h"
#include"processor.h"
#include"mediantracker.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include"filescompare.h"

TEST_CASE("parse comand line"){
    std::string config_path;
    int argc = 3;
    char* argv[] = { 
        (char*)"./program",
        (char*)"-config",
        (char*)"/my/config.toml"
    };
    
    ParseCommandString(argc, argv, config_path);
    REQUIRE(config_path == "/my/config.toml");
    config_path = "";
    argv[1] = (char*)"-cfg";
    ParseCommandString(argc, argv, config_path);
    REQUIRE(config_path == "/my/config.toml");
    argc = 1;
    fs::path path = GetExecutablePath().parent_path().string();
    path.append("config.toml");
    config_path = "";
    ParseCommandString(argc,argv, config_path);
    REQUIRE(path == config_path);
}
TEST_CASE("parsing config file"){
    std::string config_path;
    fs::path current = GetExecutablePath().parent_path().parent_path()
        .append("/test_files").append("/parse_toml1.toml");
    ParsedToml parsed_toml;
    ParseToml(current.string(),parsed_toml);
    fs::path output = GetExecutablePath().parent_path().append("/output");
    SECTION("first"){
        REQUIRE(parsed_toml.masks.size() == 0);
        REQUIRE(parsed_toml.input_path == "/var/data/csv_files");
        REQUIRE(parsed_toml.output_path == output);
    }
    SECTION("second"){
        current = current.parent_path().append("/parse_toml2.toml");
        parsed_toml.input_path.clear();
        parsed_toml.output_path.clear();
        parsed_toml.masks.clear();
        ParseToml(current.string(),parsed_toml);
        REQUIRE(parsed_toml.input_path == "/home/user/project/data/input");
        REQUIRE(parsed_toml.output_path == "/home/user/project/data/results");
        REQUIRE(parsed_toml.masks.size() == 3);
        REQUIRE(parsed_toml.masks.at(0) == "level");
        REQUIRE(parsed_toml.masks.at(1) == "trade");
        REQUIRE(parsed_toml.masks.at(2) == "market");
    }
}
TEST_CASE("find csv files" ){
    ParsedToml parsed;
    parsed.input_path = GetExecutablePath().parent_path().parent_path().append("/test_files/");
    parsed.output_path = parsed.input_path;
    std::vector<fs::path> files;
   // REQUIRE(parsed.input_path.string() == "/home/al/Documents/TEST_PROJECT/test_files/");
    GetCvsFiles(parsed,files);
   // REQUIRE(files.size() == 4);
    files.clear();
    parsed.masks.push_back("level");
    parsed.masks.push_back("trade");
    GetCvsFiles(parsed,files);
    REQUIRE(files.size() == 2);
}

TEST_CASE("parse csv files"){
    fs::path path = GetExecutablePath().parent_path().parent_path().
                        append("/test_files/level.csv");
    std::ifstream file(path);
    std::string temp;
    std::getline(file,temp);
    std::getline(file,temp);
    ParsedCsvLine parsed = ParseString(temp);
    REQUIRE(parsed.receive_ts == 100);
    REQUIRE(parsed.exchange_ts == 100);
    REQUIRE(parsed.price == 100);
    REQUIRE(parsed.quantity == 1);
    REQUIRE(parsed.side == Side::Bid);
    REQUIRE(parsed.build.value() == true);
}

TEST_CASE("calculate median and save to file"){
    ParsedToml parsed_toml;
    parsed_toml.input_path = GetExecutablePath().parent_path()
                                .parent_path().append("/test_files/");
    parsed_toml.masks.push_back("level");
    parsed_toml.masks.push_back("trade");
    parsed_toml.output_path = parsed_toml.input_path;
    std::vector<fs::path> files;
    GetCvsFiles(parsed_toml,files);
    FileStreamProcessor proc(files);
    MedianTracker median;
    std::optional<ParsedCsvLine> temp = proc.GetNext();
    
    parsed_toml.output_path.append("/output.csv");
    std::ofstream output_file(parsed_toml.output_path,std::ios::out | std::ios::trunc);
    output_file << "receive_ts;price_median\n";
   
    while(temp.has_value()){
        auto& value = temp.value();
        if(median.addValue(value.price)){
            output_file << value.receive_ts << ";" << std::fixed << std::setprecision(8) <<
                median.getMedian() << "\n";
        }
       
        temp=proc.GetNext();
    }
    fs::path result_file = parsed_toml.output_path.parent_path().append("/result.csv");
    bool result = CompareFiles(parsed_toml.output_path,result_file);
    REQUIRE(result == true);
}