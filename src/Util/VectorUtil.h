#pragma once

namespace VectorUtil {
	template<class T>
    bool contains(std::vector<T> haystack, T needle) {
        return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
    }
}