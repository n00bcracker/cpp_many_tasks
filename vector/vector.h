#pragma once

#include <iterator>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <utility>

class Vector {
public:
    Vector() : size_(0), capacity_(0), data_(nullptr) {
    }

    Vector(size_t size) : Vector() {
        if ((data_ = reinterpret_cast<int*>(std::calloc(size, sizeof(int))))) {
            size_ = size;
            capacity_ = size;
        }
    }

    Vector(std::initializer_list<int> list) : Vector() {
        Reserve(list.size());
        for (auto elem : list) {
            PushBack(elem);
        }
    }

    Vector(const Vector& other) : Vector() {
        Reserve(other.size_);
        if (data_ && other.data_) {
            std::memcpy(data_, other.data_, other.size_ * sizeof(int));
        }
        size_ = other.size_;
    }

    Vector(Vector&& other) : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
        other.data_ = nullptr;
        other.capacity_ = 0;
        other.size_ = 0;
    }

    const Vector& operator=(const Vector& other) {
        if (this != &other) {
            if (!data_) {
                std::free(data_);
                size_ = 0;
                capacity_ = 0;
            }

            Vector vector(other);
            *this = std::move(vector);
        }

        return *this;
    }

    const Vector& operator=(Vector&& other) {
        if (this != &other) {
            if (!data_) {
                std::free(data_);
                size_ = 0;
                capacity_ = 0;
            }

            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            other.data_ = nullptr;
            other.capacity_ = 0;
            other.size_ = 0;
        }

        return *this;
    }

    ~Vector() {
        std::free(data_);
    }

    void Swap(Vector& other) {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    int operator[](size_t index) const {
        return data_[index];
    }

    int& operator[](size_t index) {
        return data_[index];
    }

    size_t Size() const {
        return size_;
    }

    size_t Capacity() const {
        return capacity_;
    }

    void Reserve(size_t capacity) {
        if (capacity_ < capacity) {
            if ((data_ = reinterpret_cast<int*>(std::realloc(data_, capacity * sizeof(int))))) {
                capacity_ = capacity;
            }
        }
    }

    void PushBack(int elem) {
        if (size_ == capacity_) {
            Reserve(capacity_ > 0 ? capacity_ * 2 : 1);
        }

        data_[size_++] = elem;
    }

    void PopBack() {
        --size_;
    }

    void Clear() {
        size_ = 0;
    }

    class Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = int;
        using difference_type = ptrdiff_t;
        using pointer = int*;
        using reference = int&;

        Iterator() : iterator_(nullptr) {
        }

        Iterator(int* iterator) : iterator_(iterator) {
        }

        Iterator& operator+=(int n) {
            iterator_ += n;
            return *this;
        }

        Iterator& operator++() {
            return (*this) += 1;
        }

        Iterator operator++(int) {
            Iterator it = *this;
            ++(*this);
            return it;
        }

        Iterator& operator--() {
            return (*this) += -1;
        }

        Iterator operator--(int) {
            Iterator it = *this;
            --(*this);
            return it;
        }

        value_type& operator*() const {
            return *iterator_;
        }

        value_type* operator->() const {
            return iterator_;
        }

        Iterator operator+(int n) const {
            Iterator it = *this;
            return it += n;
        }

        friend Iterator operator+(int n, const Iterator& it);

        Iterator& operator-=(int n) {
            return *this += -n;
        }

        Iterator operator-(int n) const {
            Iterator it = *this;
            return it -= n;
        }

        difference_type operator-(const Iterator& other) const {
            return iterator_ - other.iterator_;
        }

        value_type& operator[](int index) const {
            return *(iterator_ + index);
        }

        auto operator<=>(const Iterator&) const = default;

    private:
        int* iterator_;
    };

    Iterator begin() const {
        return Iterator(data_);
    }

    Iterator end() const {
        return Iterator(data_ + size_);
    }

private:
    size_t size_, capacity_;
    int* data_;
};

Vector::Iterator operator+(int n, const Vector::Iterator& it) {
    return it + n;
}
