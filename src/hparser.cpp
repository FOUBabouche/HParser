#include <hparser.h>

#include <algorithm>
#include <fstream>

#include <iostream>

HParser::HParser(const std::filesystem::path &file_path)
{
    rules = {
        {LineType::StructDeclaration, std::regex(R"(^\s*struct\s+[A-Za-z0-9_]*\s*\{?\s*$)")},
        {LineType::StructMemberDeclarion, std::regex(R"(^\s*[A-Za-z_][A-Za-z0-9_]*(::[A-Za-z_][A-Za-z0-9_]*)*(<[A-Za-z0-9_,\s]*>)?\s+[A-Za-z_][A-Za-z0-9_]*\s*(=\s*("[^"]*"|\d+)\s*)?;\s*$)")},
        {LineType::EnumDeclaration, std::regex(R"(^\s*enum\s+[A-Za-z0-9_]*\s*\{?\s*$)")},
        {LineType::EnumMemberDeclaration, std::regex(R"(^\s*[A-Za-z0-9_]+\s*(=\s*[0-9]+)*,?\s*$)")},
        {LineType::EndObject, std::regex(R"(^\s*\}\s*;$)")}
    };

    ////////////////////////////////////////////////////////////////////////////////////
    /////////                      Get importants lines                       //////////
    ////////////////////////////////////////////////////////////////////////////////////
    std::ifstream file(file_path);
    if(!file.is_open()){
        std::cerr << "File not found" << std::endl;
        return;
    }

    std::vector<std::pair<const LineType, std::string>> lines;

    std::string line;
    while(std::getline(file, line)){
        auto it = std::find_if(rules.begin(), rules.end(), [&line](std::pair<const LineType, std::regex>& rule){
            return std::regex_match(line, rule.second);
        });
        if(it == rules.end()) continue;
        std::pair<const LineType, std::string> good_line = {it->first, line};
        lines.push_back(good_line);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    /////////                 Register enum and component                     //////////
    ////////////////////////////////////////////////////////////////////////////////////
    for(auto [type, line] : lines){
        switch (type)
        {
        case LineType::StructDeclaration :
            /* code */
            break;
        
        default:
            break;
        }
    }
}