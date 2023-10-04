#pragma once

#include <cstddef>
#include <vector>

class RingBuffer {
    std::vector<int> buffer_;
    size_t capacity_, first_, last_;

public:
    explicit RingBuffer(size_t capacity)
        : buffer_(capacity), capacity_(capacity), first_(0), last_(0) {
    }

    size_t Size() const {
        return last_ - first_;
    }

    bool Empty() const {
        return last_ == first_;
    }

    bool TryPush(int element) {
        if (Size() == capacity_) {
            return false;
        } else {
            buffer_[last_++ % capacity_] = element;
            return true;
        }
    }

    bool TryPop(int* element) {
        if (Empty()) {
            return false;
        } else {
            *element = buffer_[first_++ % capacity_];
            return true;
        }
    }
};
