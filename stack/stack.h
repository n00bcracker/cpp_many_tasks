#pragma once

#include <cstddef>
#include <vector>

class Stack {
    std::vector<int> stack_;
public:
    void Push(int x) {
        stack_.push_back(x);
    }

    bool Pop() {
        if (!stack_.empty()) {
            stack_.pop_back();
            return true;
        } else {
            return false;
        }
    }

    int Top() const {
        return stack_.back();
    }

    bool Empty() const {
        return stack_.empty();
    }

    size_t Size() const {
        return stack_.size();
    }
};
