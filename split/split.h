#pragma once

#include <string>
#include <stdexcept>
#include <vector>

std::vector<std::string> Split(const std::string& string, const std::string& delimiter = " ") {
    std::vector<std::string> res;
    if (!string.empty()) {
        int start_pos = 0;
        size_t fpos;
        while ((fpos = string.find(delimiter, start_pos)) != std::string::npos){
            res.push_back(string.substr(start_pos, fpos - start_pos));
            start_pos = fpos + delimiter.size();
        }

        res.push_back(string.substr(start_pos));
    }

    return res;
}
