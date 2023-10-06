#pragma once

#include <cstddef>
#include <string>
#include <cstring>
#include <algorithm>

class StringView {
    const char* string_ptr_;
    const size_t length_;

public:
    StringView(const std::string& str, size_t start_pos = 0, size_t len = std::string::npos)
        : string_ptr_(str.data() + start_pos), length_(std::min(str.size() - start_pos, len)) {
    }

    StringView(const char* str) : string_ptr_(str), length_(std::strlen(str)) {
    }

    StringView(const char* str, size_t len) : string_ptr_(str), length_(len) {
    }

    char operator[](size_t pos) const {
        return string_ptr_[pos];
    }

    const char* Data() const {
        return string_ptr_;
    }

    size_t Size() const {
        return length_;
    }
};
