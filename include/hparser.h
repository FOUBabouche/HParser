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

namespace Traits{
    struct HMember{
        const char* name;
        const char* type;
        const char* value;
    };

    struct HStruct{
        const char* name;
        std::vector<HMember> members;
    };
}

class HParser{
    public:
        HParser(const std::filesystem::path& file_path);

    private:
        std::unordered_map<const LineType, std::regex> rules;
};

#endif