#pragma once

#include "Util.h"

namespace VectorUtil {
    inline bool contains(std::vector<int> haystack, int needle) {
        return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
    }

	template<class T>
    bool contains(std::vector<T> haystack, T needle) {
        return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
    }

    template <class T>
    bool containsAny(std::vector<T> haystack, std::vector<T> needles) {
        for (T needle : needles) {
            if (contains(haystack, needle)) {
                return true;
            }
        }
        return false;
    }

    template <class T>
    bool containsAll(std::vector<T> haystack, std::vector<T> needles) {
        for (T needle : needles) {
            if (!contains(haystack, needle)) {
                return false;
            }
        }
        return true;
    }

    inline std::vector<int> stoiv(std::string string) {
        std::vector<int> ret;
        for (std::string part : stl::string_split(string, ',')) {
            try {
                ret.push_back(std::stoi(part));
            } catch (auto ex) {
            }
        }
        return ret;
    }
}