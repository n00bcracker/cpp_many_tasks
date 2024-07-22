#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include "util.h"

class GenNames {
public:
    GenNames() {
        for (char c = 'A'; c <= 'Z'; ++c) {
            alphabet_.push_back(c);
        }

        for (char c = 'a'; c <= 'z'; ++c) {
            alphabet_.push_back(c);
        }

        for (char c = '0'; c <= '9'; ++c) {
            alphabet_.push_back(c);
        }

        alphabet_.push_back('_');
    }

private:
    static size_t ShiftMix(size_t v) {
        return v ^ (v >> 47);
    }

    static size_t EncodeFullPartData(const std::string& data) {
        size_t res;
        memcpy(&res, data.data(), 8);
        return res;
    }

    size_t FullPartHash(const std::string& data) {
        static size_t seed = static_cast<size_t>(3339675911UL);
        size_t hash = seed ^ (15 * kMul);
        const size_t m_data = ShiftMix(EncodeFullPartData(data) * kMul) * kMul;
        hash ^= m_data;
        hash *= kMul;
        return hash;
    }

    static std::optional<std::string> TryDecodeData(size_t data) {
        const char* buf = reinterpret_cast<const char*>(&data);
        if (buf[7] != 0) {
            return std::nullopt;
        }

        for (size_t i = 0; i < 7; ++i) {
            char c = buf[i];
            if (c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9')) {
                continue;
            } else {
                return std::nullopt;
            }
        }

        return std::string(buf, 7);
    }

    void GenerateNamesTraceback(std::string& cur_name, size_t name_len) {
        if (cur_name.size() == name_len) {
            const size_t hash = FullPartHash(cur_name);
            if (const auto right_part = TryDecodeData(hash)) {
                std::string name = cur_name + *right_part;
                // std::cout << name << ' ' << hash_func(name) << std::endl;
                collisions_.emplace_back(std::move(name));
            }
            return;
        }

        for (const char c : alphabet_) {
            cur_name.push_back(c);
            GenerateNamesTraceback(cur_name, name_len);
            if (collisions_.size() == 15'000) {
                return;
            }
            cur_name.pop_back();
        }
    }

public:
    void Generate() {
        std::string name;
        GenerateNamesTraceback(name, 8);
    }

    std::vector<std::string> GetCollisions() {
        return std::move(collisions_);
    }

private:
    std::string alphabet_;
    std::vector<std::string> collisions_;
    static constexpr size_t kMul =
        (static_cast<size_t>(0xc6a4a793UL) << 32UL) + static_cast<size_t>(0x5bd1e995UL);
};

// (hash1 ^ data1) * mul = p * c + k
// (hash1 ^ data1) = 0

int main() {
    const auto current_dir = GetFileDir(__FILE__);
    std::filesystem::path input_file(current_dir / "../input.txt");
    std::ofstream myfile(input_file);
    GenNames generator;
    generator.Generate();
    std::vector<std::string> result = generator.GetCollisions();
    for (const auto& name : result) {
        myfile << name << std::endl;
    }

    myfile.close();
}
