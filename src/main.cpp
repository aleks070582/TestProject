#include<iostream>
#include<unordered_map>
#include<vector>
#include<algorithm>
#include<format>
#include<ranges>
#include<boost/program_options.hpp>
#include<boost/filesystem.hpp>
#include<iomanip>
#include"utility.h"
#include"processor.h"
#include"mediantracker.h"
#include"logger.h"
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/ranges.h> 

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[]){
    InitLogger();
    spdlog::info("running app");
    std::string config_path;
    try {
    ParseCommandString(argc,argv,config_path);
    }
    catch(po::error){
        spdlog::error("incorrect command line");
        std::exit(EXIT_FAILURE);
    }
    spdlog::info("command line parsed successfully. config path: {}",config_path);
    ParsedToml parsed_toml;
    ParseToml(config_path,parsed_toml);
    if(!fs::exists(parsed_toml.input_path)){
        spdlog::error("input directory is incorrect");
        std::exit(EXIT_FAILURE);
    }
    if(!fs::exists(parsed_toml.output_path)){
        spdlog::error("output directory is incorrect");
        std::exit(EXIT_FAILURE);
    }
    spdlog::info("toml file parsed successfuly");
    spdlog::info("input path: {}",parsed_toml.input_path.string());
    spdlog::info("output path: {}",fmt::join(parsed_toml.output_path.string(),", "));
    spdlog::info("file masks {}",parsed_toml.masks);
    std::vector<fs::path> files;
    GetCvsFiles(parsed_toml,files);
    if(files.empty()){
        spdlog::error("input directory does not contain required CSV files");
        std::exit(EXIT_FAILURE);
    }
    spdlog::info("input directory contains next files:");
    for(const auto&file:files){
        spdlog::info(" {}",file.string());
    }
    FileStreamProcessor proc(files);
    MedianTracker median;
    std::optional<ParsedCsvLine> temp = proc.GetNext();
    parsed_toml.output_path.append("/output.csv");
    std::ofstream output_file(parsed_toml.output_path,std::ios::out | std::ios::trunc);
    if(!output_file){
        spdlog::error("cannot open: {}",parsed_toml.output_path.string());
        std::exit(EXIT_FAILURE);
    }
    output_file << "receive_ts;price_median\n";
     while(temp.has_value()){
        auto& value = temp.value();
        if(median.addValue(value.price)){
            output_file << value.receive_ts << ";" << std::fixed << std::setprecision(8) <<
                median.getMedian() << "\n";
        }
        temp=proc.GetNext();
    }
    spdlog::info("application finished successfully");
}