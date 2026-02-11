
#pragma once
#include<vector>
#include<iostream>
#include<vector>
#include<fstream>
#include<optional>
#include<sstream>
#include<queue>
#include<format>
#include<optional>
#include<boost/filesystem.hpp>
#include<boost/program_options.hpp>
#include <toml++/toml.hpp>
#include"logger.h"
#include"utility.h"
#include"asyncbuffer.h"

struct MinComparator {
    bool operator()(const std::pair<int64_t, int>& a, const std::pair<int64_t,
         int>& b) const {
        return a.first > b.first;
    }
};

using MinPriorityQueue = std::priority_queue<std::pair<int64_t,int>,
                            std::vector<std::pair<int64_t,int>>,MinComparator>;

class FileStreamProcessor{
    std::vector<ParsedCsvLine> file_content;
    MinPriorityQueue queue;
    std::vector<std::unique_ptr<Buffer>> file_streams;
    public:
    FileStreamProcessor(const std::vector<fs::path>& files);
    std::optional<ParsedCsvLine> GetNext();
   
};
