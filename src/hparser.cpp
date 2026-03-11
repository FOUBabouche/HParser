#include <hparser.h>

#include <algorithm>
#include <fstream>

#include <iostream>

HParser::HParser(const std::filesystem::path &file_path)
{
    rules = {
        {LineType::StructDeclaration, std::regex(R"(^\s*struct\s+[A-Za-z0-9_]*\s*\{?\s*$)")},
        {LineType::StructMemberDeclarion, std::regex(R"(^\s*([A-Za-z_][A-Za-z0-9_]*\s+)*[A-Za-z_][A-Za-z0-9_]*(::[A-Za-z_][A-Za-z0-9_]*)*(<[A-Za-z0-9_,\s]*(::[A-Za-z_][A-Za-z0-9_]*)*>)?(\*)*\s+[A-Za-z_][A-Za-z0-9_]*\s*(=\s*("[^"]*"|([0-9]*(.[0-9A-Za-z]*)*))\s*)?;\s*$)")},
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

    std::vector<std::pair<LineType, std::string>> lines;

    std::string line;
    while(std::getline(file, line)){
        auto it = std::find_if(rules.begin(), rules.end(), [&line](const std::pair<const LineType, std::regex>& rule){
            return std::regex_match(line, rule.second);
        });
        if(it == rules.end()) continue;
        std::pair<LineType, std::string> good_line = {it->first, line};
        lines.push_back(good_line);
    }

    HTraits::HObject object = {};

    ////////////////////////////////////////////////////////////////////////////////////
    /////////                 Register enum and strcut                        //////////
    ////////////////////////////////////////////////////////////////////////////////////
    for(auto [type, line] : lines){
        switch (type)
        {
        case LineType::StructDeclaration :
            object = get_struct_name(line);
            break;
        case LineType::StructMemberDeclarion :
            get_struct_member(object, line);
            break;
        case LineType::EnumDeclaration :
            object = get_enum_name(line);
            break;
        case LineType::EnumMemberDeclaration :
            get_enum_member(object, line);
            break;
        case LineType::EndObject :
            objects.push_back(object);
            break;
        
        default:
            break;
        }
    }
}

HTraits::HObject HParser::get_struct_name(const std::string &line)
{
    auto begin_index = line.find_first_of("struct");
    begin_index += 6;
    while(line[begin_index] == ' '){
        begin_index++;
    }
    auto raw_name = std::string(line.begin()+begin_index, line.end());
    auto end_index = raw_name.find_first_of(' ');
    if(end_index >= raw_name.size()-1){
        end_index = raw_name.find_first_of('{');
    }
    return {HTraits::HObject::Struct, std::string(raw_name.begin(), (end_index > raw_name.size()-1 ? raw_name.end() : raw_name.begin()+end_index)), {}};
}

void HParser::get_struct_member(HTraits::HObject &object, const std::string &line)
{
    enum WordType{
        Type,
        Name,
        Value
    } word_type = Type;

    HTraits::HMember member;
    bool not_a_word = false;

    std::string word = "";
    for(auto character : line){
        if(character == ';'){
            if(!word.empty()){
                if(word_type == Value){
                    while(!std::isdigit(word.back())){
                        word.pop_back();
                    }
                    member.value = word;
                }else if(word_type == Name){
                    member.name = word;
                }
            }
            break;
        }
        else if(character == ' '){
            if(not_a_word) continue;
            not_a_word = true;
        }else{
            not_a_word = false;
            if(character != '=')
                word += character;
            continue;
        }
        if(not_a_word){
            if(word.empty()) continue;

            if(word_type == Type){
                if(word == "const" || word == "unsigned"){
                    member.type += word + " ";
                    word.clear();
                    continue;
                }else{
                    member.type += word;
                    word.clear();
                    word_type = Name;
                    continue;
                }
            }
            if(word_type == Name){
                member.name = word;
                word.clear();
                word_type = Value;
            }
        }
    }
    object.members.push_back(member);
}

HTraits::HObject HParser::get_enum_name(const std::string &line)
{
    auto begin_index = line.find_first_of("enum");
    begin_index += 4;
    while(line[begin_index] == ' '){
        begin_index++;
    }
    auto raw_name = std::string(line.begin()+begin_index, line.end());
    auto end_index = raw_name.find_first_of(' ');
    if(end_index >= raw_name.size()-1){
        end_index = raw_name.find_first_of('{');
    }
    return {HTraits::HObject::Enum, std::string(raw_name.begin(), (end_index >= raw_name.size()-1 ? raw_name.end() : raw_name.begin()+end_index)), {}};
}

void HParser::get_enum_member(HTraits::HObject &object, const std::string &line)
{
    enum WordType{
        Name,
        Value
    } word_type = Name;

    HTraits::HMember member = {"", "uint8_t", ""};

    bool not_a_word = false;
    std::string word;
    for(auto character : line){
        if(character == ',') break;
        if(character == ' '){
            if(not_a_word) continue;
            not_a_word = true;
        }
        else{
            if(character != '=')
                word += character;
            not_a_word = false;
        }

        if(not_a_word){
            if(word.empty()) continue;
            
            if(word_type == Name){
                member.name = word;
                word.clear();
                word_type = Value;
            }else{
                member.type = word;
                break;
            }
        }
    }
    if(word_type == Name){
        member.name = word;
    }else{
        member.value = word;
    }

    object.members.push_back(member);
}