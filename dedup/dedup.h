#pragma once

#include <memory>
#include <vector>
#include <map>

template <class T>
std::vector<std::unique_ptr<T>> Duplicate(const std::vector<std::shared_ptr<T>>& items) {
    std::vector<std::unique_ptr<T>> result;
    for (const auto& item_ptr : items) {
        result.emplace_back(new T(*item_ptr));
    }

    return result;
}

template <class T>
std::vector<std::shared_ptr<T>> DeDuplicate(const std::vector<std::unique_ptr<T>>& items) {
    std::map<T, std::shared_ptr<T>> map;
    std::vector<std::shared_ptr<T>> result;
    for (const auto& item_ptr: items) {
        if (!map.contains(*item_ptr)) {
            map.emplace(*item_ptr, new T(*item_ptr));
        }
        result.push_back(map[*item_ptr]);
    }

    return result;
}
