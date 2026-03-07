#ifndef __PARSER_H__
#define __PARSER_H__

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>
#include <regex>

enum LineType{
    StructDeclaration,
    StructMemberDeclarion,
    EnumDeclaration,
    EnumMemberDeclaration,
    EndObject
};

namespace HTraits{
    struct HMember{
        std::string name;
        std::string type;
        std::string value;
    };

    struct HObject{
        enum Type{
            Struct,
            Enum
        } type;
        std::string name;
        std::vector<HMember> members;
    };
}

class HParser{
    public:
        HParser(const std::filesystem::path& file_path);

        std::vector<HTraits::HObject> get_objects() const { return objects; };

    private:
        HTraits::HObject get_struct_name(const std::string& line);
        void get_struct_member(HTraits::HObject& object, const std::string& line);
        HTraits::HObject get_enum_name(const std::string& line);
        void get_enum_member(HTraits::HObject& object, const std::string& line);

    private:
        std::unordered_map<const LineType, std::regex> rules;
        std::vector<HTraits::HObject> objects;
};

#endif