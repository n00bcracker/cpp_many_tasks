#pragma once

#include <cstddef>
#include <initializer_list>
#include <cstring>
#include <new>
#include <stdlib.h>
#include <algorithm>

constexpr size_t kBlockSize = 128;

class Deque {
    class Block {
    public:
        Block() = default;

        Block(size_t size) : begin_(0), end_(size) {
            data_ = static_cast<int*>(calloc(kBlockSize, sizeof(int)));
        }

        Block(const Block& other) : begin_(other.begin_), end_(other.end_) {
            if (other.data_) {
                data_ = static_cast<int*>(calloc(kBlockSize, sizeof(int)));
                std::memcpy(data_, other.data_, sizeof(int) * kBlockSize);
            }
        }

        Block(Block&& other) : begin_(other.begin_), end_(other.end_) {
            data_ = other.data_;
            other.data_ = nullptr;
        }

        ~Block() {
            free(data_);
        }

        bool PushFront(int value) {
            if (!data_) {
                data_ = static_cast<int*>(calloc(kBlockSize, sizeof(int)));
                begin_ = end_ = kBlockSize;
            }

            if (begin_ > 0) {
                *(data_ + begin_ - 1) = value;
                --begin_;
                return true;
            } else {
                return false;
            }
        }

        bool PushBack(int value) {
            if (!data_) {
                data_ = static_cast<int*>(calloc(kBlockSize, sizeof(int)));
                begin_ = end_ = 0;
            }

            if (end_ < kBlockSize) {
                *(data_ + end_) = value;
                ++end_;
                return true;
            } else {
                return false;
            }
        }

        void PopFront() {
            if (++begin_ == end_) {
                free(data_);
                data_ = nullptr;
            }
        }

        void PopBack() {
            if (--end_ == begin_) {
                free(data_);
                data_ = nullptr;
            }
        }

        int& operator[](size_t index) {
            return *(data_ + begin_ + index);
        }

        const int& operator[](size_t index) const {
            return *(data_ + begin_ + index);
        }

        size_t Size() const {
            return end_ - begin_;
        }

        bool IsEmpty() const {
            return data_ == nullptr;
        }

        size_t BeginIdx() const {
            return begin_;
        }

    private:
        int* data_ = nullptr;
        size_t begin_;
        size_t end_;
    };

public:
    Deque() {
    }

    Deque(const Deque& other) {
        size_ = other.size_;
        block_cnt_ = other.block_cnt_;
        left_block_ = other.left_block_;
        if (other.data_) {
            data_ = new Block*[block_cnt_];
            for (size_t i = 0; i < block_cnt_; ++i) {
                data_[i] = new Block(*other.data_[i]);
            }
        }
    }

    Deque(Deque&& other) {
        size_ = other.size_;
        block_cnt_ = other.block_cnt_;
        left_block_ = other.left_block_;
        data_ = other.data_;
        other.size_ = 0;
        other.block_cnt_ = 0;
        other.data_ = nullptr;
    }

    explicit Deque(size_t size) {
        if (size > 0) {
            block_cnt_ = (size - 1) / kBlockSize + 1;
            size_ = size;
            data_ = new Block*[block_cnt_];
            left_block_ = 0;
            for (size_t i = 0; i < block_cnt_; ++i) {
                if (kBlockSize * (i + 1) <= size) {
                    data_[i] = new Block(kBlockSize);
                } else {
                    data_[i] = new Block(size - kBlockSize * i);
                }
            }
        }
    }

    Deque(std::initializer_list<int> list) : Deque(list.size()) {
        for (size_t i = 0; int elem : list) {
            (*this)[i++] = elem;
        }
    }

    const Deque& operator=(const Deque& other) {
        if (this != &other) {
            Clear();
            size_ = other.size_;
            block_cnt_ = other.block_cnt_;
            left_block_ = other.left_block_;
            if (other.data_) {
                data_ = new Block*[block_cnt_];
                for (size_t i = 0; i < block_cnt_; ++i) {
                    data_[i] = new Block(*other.data_[i]);
                }
            }
        }

        return *this;
    }

    const Deque& operator=(Deque&& other) {
        if (this != &other) {
            Clear();
            size_ = other.size_;
            block_cnt_ = other.block_cnt_;
            left_block_ = other.left_block_;
            data_ = other.data_;
            other.size_ = 0;
            other.block_cnt_ = 0;
            other.data_ = nullptr;
        }

        return *this;
    }

    ~Deque() {
        Clear();
    }

    void Swap(Deque& other) {
        std::swap(data_, other.data_);
        std::swap(left_block_, other.left_block_);
        std::swap(block_cnt_, other.block_cnt_);
        std::swap(size_, other.size_);
    }

    void PushBack(int value) {
        size_t block_idx = BlockIdx(size_);
        if (!data_ || !data_[block_idx]->PushBack(value)) {
            Realloc();
            PushBack(value);
        } else {
            ++size_;
        }
    }

    void PopBack() {
        size_t block_idx = BlockIdx(size_ - 1);
        data_[block_idx]->PopBack();
        if (!--size_) {
            left_block_ = 0;
        }
    }

    void PushFront(int value) {
        size_t block_idx = BlockIdx(-1);
        if (!data_ || !data_[block_idx]->PushFront(value)) {
            Realloc();
            PushFront(value);
        } else {
            left_block_ = block_idx;
            ++size_;
        }
    }

    void PopFront() {
        size_t block_idx = BlockIdx(0);
        data_[block_idx]->PopFront();
        if (data_[block_idx]->IsEmpty()) {
            left_block_ = (left_block_ + 1) % block_cnt_;
        }

        if (!--size_) {
            left_block_ = 0;
        }
    }

    int& operator[](size_t index) {
        const size_t block_idx = BlockIdx(index);
        const size_t elem_idx =
            (data_[left_block_]->BeginIdx() + index) % kBlockSize - data_[block_idx]->BeginIdx();
        return (*data_[block_idx])[elem_idx];
    }

    const int& operator[](size_t index) const {
        const size_t block_idx = BlockIdx(index);
        const size_t elem_idx =
            (data_[left_block_]->BeginIdx() + index) % kBlockSize - data_[block_idx]->BeginIdx();
        return (*data_[block_idx])[elem_idx];
    }

    size_t Size() const {
        return size_;
    }

    void Clear() {
        for (size_t i = 0; i < block_cnt_; ++i) {
            delete data_[i];
            data_[i] = nullptr;
        }

        delete[] data_;
        data_ = nullptr;
        size_ = 0;
        block_cnt_ = 0;
    }

private:
    void Realloc() {
        const size_t new_block_cnt = block_cnt_ > 0 ? 2 * block_cnt_ : 1;
        Block** new_data = new Block*[new_block_cnt];
        for (size_t i = 0; i < new_block_cnt; ++i) {
            if (i < block_cnt_) {
                new_data[i] = data_[(left_block_ + i) % block_cnt_];
            } else {
                new_data[i] = new Block();
            }
        }

        delete[] data_;
        data_ = new_data;
        left_block_ = 0;
        block_cnt_ = new_block_cnt;
    }

    size_t BlockIdx(int64_t elem_idx) const {
        if (!size_) {
            return 0;
        } else if (elem_idx >= 0) {
            return (left_block_ + (data_[left_block_]->BeginIdx() + elem_idx) / kBlockSize) %
                   block_cnt_;
        } else {
            return BlockIdx(block_cnt_ * kBlockSize + elem_idx);
        }
    }

    Block** data_ = nullptr;
    size_t size_ = 0;
    size_t block_cnt_ = 0;
    size_t left_block_;
};
