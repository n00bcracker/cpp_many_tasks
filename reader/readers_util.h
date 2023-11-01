#pragma once

#include <reader.h>

#include <cstddef>
#include <vector>
#include <memory>
#include <ios>
#include <sstream>
#include <string>

class LimitReader : public Reader {
public:
    LimitReader(std::unique_ptr<Reader> reader, size_t limit)
        : reader_(std::move(reader)), limit_(limit) {
    }

    size_t Read(char* buf, size_t len) override {
        size_t read_len = std::min(len, limit_);
        size_t res = reader_->Read(buf, read_len);
        limit_ -= res;
        return res;
    }

private:
    std::unique_ptr<Reader> reader_ = nullptr;
    size_t limit_ = 0;
};

class TeeReader : public Reader {
public:
    explicit TeeReader(std::vector<std::unique_ptr<Reader>> readers) {
    }

    size_t Read(char* buf, size_t len) override {
        size_t res = 0;
        while (it_ != readers_.end() && (res += (*it_)->Read(buf + res, len - res)) < len) {
            ++it_;
        }

        return res;
    }

private:
    std::vector<std::unique_ptr<Reader>> readers_;
    std::vector<std::unique_ptr<Reader>>::iterator it_;
};

class HexDecodingReader : public Reader {
public:
    explicit HexDecodingReader(std::unique_ptr<Reader> reader) {
    }

    size_t Read(char* buf, size_t len) override {
        size_t res = 0;
        std::string num('a', 2);
        uint hex;
        while (res < len) {
            if (reader_->Read(num.data(), 2)) {
                std::istringstream(num) >> std::hex >> hex;
                *(buf + res) = hex;
                ++res;
            } else {
                break;
            }
        }

        return res;
    }

private:
    std::unique_ptr<Reader> reader_;
};
