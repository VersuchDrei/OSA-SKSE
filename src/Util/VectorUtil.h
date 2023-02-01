#pragma once

#include "Util.h"
#include "Util/Constants.h"

namespace VectorUtil {
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

    template <class T>
    int getIndex(std::vector<T> haystack, T needle) {
        auto it = std::find(haystack.begin(), haystack.end(), needle);
        if (it == haystack.end()) {
            return -1;
        }
        return it - haystack.begin();
    }

    template <class T>
    T randomElement(std::vector<T>* vec) {
        return vec->at(std::uniform_int_distribution<>(1, vec->size())(Constants::RNG) - 1);
    }

    bool contains(std::vector<int> haystack, int needle);
    std::vector<int> stoiv(std::string string);
    std::vector<std::vector<int>> stoim(std::string string);
    std::vector<int> getElementOrEmpty(std::vector<std::vector<int>> matrix, int index);
}