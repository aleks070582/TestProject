#include"utility.h"

void ParseToml(const std::string &config_path, ParsedToml &parsed_struct){
    if(!fs::exists(config_path)){
        spdlog::error("toml file is not exist");
        std::exit(EXIT_FAILURE);
    }
    toml::table toml_parse;
    try{
       toml_parse = toml::parse_file(config_path);
    }
    catch(toml::parse_error){
        spdlog::error("failure toml parsing");
        std::exit(EXIT_FAILURE);
    }
    if (!toml_parse.contains("main")){
        spdlog::error("toml does not contains main");
        std::exit(EXIT_FAILURE);
    }
    auto config = toml_parse["main"];
    if (!config["input"]) {
      spdlog::error("Missing required option 'input' in [main] section");
      std::exit(EXIT_FAILURE);
    }
    auto& input_path =parsed_struct.input_path;
    input_path = config["input"].value_or<std::string>("");
    auto& output_path = parsed_struct.output_path;
    output_path = config["output"].value_or<std::string>("");
    if(output_path.empty()){
        fs::path current_work_dir = fs::current_path();
        fs::create_directories("output");
        output_path = current_work_dir;
        output_path.append("output");
    }
   auto file_masks = config["filename_mask"].as_array();
   auto& file_masks_s = parsed_struct.masks;
    if(file_masks != nullptr){
        for(auto& ch:*file_masks){
            file_masks_s.push_back(ch.as_string()->get());
        }
    }
}

void GetCvsFiles(const ParsedToml &parsed_toml, std::vector<fs::path> &files) {
    fs::directory_iterator end_it;
    for (auto it = fs::directory_iterator(parsed_toml.input_path); it != end_it; ++it) {
        fs::path file_name = *it;
        if (file_name.extension().string() == ".csv") {
            bool file_matches_mask = false;
            if (parsed_toml.masks.empty()) {
                files.push_back(file_name);
            } else {
                for (const auto& mask : parsed_toml.masks) {
                    if (file_name.filename().string().find(mask) != std::string::npos) {
                        files.push_back(file_name);
                        file_matches_mask = true;
                        break;
                    }
                }
            }
        }
    }
}


ParsedCsvLine ParseString(std::string &line){
    
    ParsedCsvLine parsed_line;
    std::vector<int> token;
    for(int i = 0; i < line.size(); ++i){
        if (line[i] == csv_delimetr || i == line.size() - 1){
            token.push_back(i);
        }
    }
    if(token.size()<5){
        std::cout << line <<std::endl;
        spdlog::error("missing column in csv file");
        std::exit(EXIT_FAILURE);
    }
    token.push_back(line.size());
    try{
      parsed_line.receive_ts = std::stoll(line.substr(0,token[0])); 
      parsed_line.exchange_ts = std::stoll(line.substr(token[0]+1,token[1] - token[0] - 1));
      parsed_line.price = std::stod(line.substr(token[1]+1,token[2] - token[1] - 1));
      parsed_line.quantity = std::stod(line.substr(token[2]+1,token[3] - token [2] - 1));
      if( line.substr(token[3]+1,token[4] - token[3] - 1) == "ask"){
         parsed_line.side = Side::Ask;
      }
      else if(line.substr(token[3]+1,token[4] - token[3] - 1) == "bid"){
        parsed_line.side = Side::Bid;
      }
      if(token.size()>5){
          if(line.substr(token[4]+1, token[5] - 1) == "1")
              parsed_line.build.emplace(true);
          else{
              parsed_line.build.emplace(false);
          }
      }
    }
    catch(...){
        spdlog::error("can not correct parse csv file");
        std::exit(EXIT_FAILURE);
    }
    return parsed_line;
}

fs::path GetExecutablePath() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return fs::absolute(fs::path(buffer));
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return fs::canonical(fs::path(buffer));
    }
    return fs::absolute(fs::path(""));
#endif
}

void ParseCommandString(int argc, char *argv[],std::string& config_path){
    //чтобы можно использовать Boost.Comand_line с одним тире а не двумя
    for (int i = 0; i < argc; ++i) {
        if (std::string(argv[i]) == "-config"){
            argv[i] = (char*)"--config";
        }
        else if(std::string(argv[i]) == "-cfg"){
            argv[i] = (char*)"--config";
        }
        else if(std::string(argv[i]) =="--config"){
            throw boost::program_options::error("incorect comand line");
        }
    }
    fs::path exec_dir = GetExecutablePath();
    fs::path current_path = exec_dir.parent_path();
    current_path.append("config.toml");
    po::options_description parametrs;
    config_path = current_path.string();
    parametrs.add_options()
    ("config", po::value<std::string>(&config_path)->default_value(current_path.string()));
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, parametrs), vm);
    po::notify(vm);
    return; 
}
