#include "StringUtil.h"

#include "Util.h"

namespace StringUtil {
    void toLower(std::string* string) {
        std::transform(string->begin(), string->end(), string->begin(), ::tolower);
    }

    void toLower(std::vector<std::string>* strings) {
        for (std::string string : *strings) {
            toLower(&string);
        }
    }

    std::vector<std::string> toTagVector(std::string string) {
        toLower(&string);
        return stl::string_split(string, ',');
    }

    std::vector<std::vector<std::string>> toTagMatrix(std::string string) {
        std::vector<std::vector<std::string>> ret;
        auto lists = stl::string_split(string, ';');
        for (auto &list : lists) {
            ret.push_back(toTagVector(list));
        }
        return ret;
    }
}