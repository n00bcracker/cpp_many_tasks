#pragma once

#include <vector>
#include <string>
#include <utility>
#include <ranges>

class StaticMap {
    std::vector<std::pair<std::string, std::string>> ordered_map_;

public:
    explicit StaticMap(const std::vector<std::pair<std::string, std::string>>& items)
        : ordered_map_(items) {
        std::ranges::sort(ordered_map_, std::ranges::less(), [](const auto& e) { return e.first; });
    }

    bool Find(const std::string& key, std::string* value) const {
        const auto l_bound =
            std::ranges::lower_bound(ordered_map_.begin(), ordered_map_.end(), key,
                                     std::ranges::less(), [](const auto& e) { return e.first; });
        if (l_bound == ordered_map_.end() || l_bound->first != key) {
            return false;
        } else {
            *value = l_bound->second;
            return true;
        }
    }
};
