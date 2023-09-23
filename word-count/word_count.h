#pragma once

#include <string>
#include <stdexcept>
#include <unordered_set>
#include <cctype>

int DifferentWordsCount(const std::string& string) {
    std::unordered_set<std::string> words;
    std::string current_word;
    for (char c : string) {
        if (std::isalpha(c)) {
            current_word += std::tolower(c);
        } else {
            if (!current_word.empty()) {
                words.emplace(current_word);
                current_word.clear();
            }
        }
    }

    if (!current_word.empty()) {
        words.emplace(current_word);
    }

    return words.size();
}
