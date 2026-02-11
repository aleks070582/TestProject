#include"processor.h"


FileStreamProcessor::FileStreamProcessor(const std::vector<fs::path>& files){
    int size = files.size();
    file_streams.resize(size);
    file_content.resize(size);
     for(int i = 0;i < size; ++i){
        std::ifstream temp(files[i],std::ios_base::in | std::ios_base::binary);
        if(!temp){
            spdlog::error("can not open: {}",files[i].string());
            std::exit(EXIT_FAILURE);
        }
        std::unique_ptr<Buffer> buf = std::make_unique<Buffer>(std::move(temp));
        file_streams[i] = std::move(buf);
    }
    for(auto& file:file_streams){
        std::string temp = file->GetLine();
    }
     for(int i = 0;i < files.size(); ++i){
            std::string temp;
            if(!file_streams[i]->GetLine(temp)){ 
                continue;
            }
            file_content[i] = (ParseString(temp));
            queue.push({file_content[i].receive_ts,i});
        }
}

std::optional<ParsedCsvLine> FileStreamProcessor::GetNext(){
    if (queue.empty() ){
        return std::nullopt;
    }
    auto[time,id] = queue.top();
    ParsedCsvLine result = file_content[id];
    queue.pop();
    std::string temp;
    if(!file_streams[id]->GetLine(temp)){
        return result;
    }
    file_content[id] = ParseString(temp);
    queue.push({file_content[id].receive_ts,id});
    return result;
}

