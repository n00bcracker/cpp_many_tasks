#pragma once

#include <__type_traits/is_swappable.h>
#include <cstddef>
#include <iterator>
#include <array>
#include <memory>
#include <utility>

template <class T>
class Vector {
public:
    class Node {
    public:
        Node() = default;
        Node(T value) : value_(new T(std::move(value))) {
        }

        Node(const Node& other) : children_(other.children_) {
            if (other.value_) {
                value_ = std::unique_ptr<T>(new T(*other.value_));
            }
        }

        Node(Node&& other) : children_(std::move(other.children_)), value_(std::move(other.value_)) {
        }

        const Node& operator=(Node other) {
            std::swap(children_, other.children_);
            std::swap(value_, other.value_);
            return *this;
        }

        void SetValue(T value) {
            value_ = std::unique_ptr<T>(new T(std::move(value)));
        }

        const T& GetValue() const {
            return *value_;
        }

        const std::shared_ptr<Node>& operator[](size_t index) const{
            return children_[index];
        }

        std::shared_ptr<Node>& operator[](size_t index) {
            return children_[index];
        }

    private:
        std::array<std::shared_ptr<Node>, 32> children_;
        std::unique_ptr<T> value_ = nullptr;
    };

    Vector() {
    }

    Vector(const Vector& other) : size_(other.size_), root_(other.root_) {
    }

    Vector& operator=(const Vector&) = delete;

    explicit Vector(size_t count, const T& value = {}) {
        for (size_t index = 0; index < count; ++index) {
            GetNode(index).SetValue(value);
            ++size_;
        }
    }

    template <class Iterator>
    Vector(Iterator first, Iterator last) {
        size_t index = 0;
        for (auto it = first; it != last; ++it) {
            GetNode(index).SetValue(*it);
            ++index;
            ++size_;
        }
    }

    Vector(std::initializer_list<T> l) {
        for (size_t index = 0; const auto& elem : l) {
            GetNode(index).SetValue(elem);
            ++index;
            ++size_;
        }
    }

    Vector Set(size_t index, const T& value) const {
        Vector res(*this);
        res.CopyPath(index).SetValue(value);
        return res;
    }

    const T& Get(size_t index) const {
        return GetNode(index).GetValue();
    }

    Vector PushBack(const T& value) const {
        Vector res(*this);
        res.CopyPath(size_).SetValue(value);
        ++res.size_;
        return res;
    }

    Vector PopBack() const {
        Vector res(*this);
        res.EraseNode(size_ - 1);
        --res.size_;
        return res;
    }

    size_t Size() const {
        return size_;
    }

private:
    const Node& GetNode(size_t index) const {
        const Node* cur_node = &root_;
        do {
            auto local_index = Index(index);
            cur_node = (*cur_node)[local_index].get();
            index = index >> 5;
        } while (index != 0);

        return *cur_node;
    }

    Node& GetNode(size_t index) {
        Node* cur_node = &root_;
        do {
            auto local_index = Index(index);
            if(!(*cur_node)[local_index]) {
                (*cur_node)[local_index] = std::shared_ptr<Node>(new Node());
            }

            cur_node = (*cur_node)[local_index].get();
            index = index >> 5;
        } while (index != 0);

        return *cur_node;
    }

    Node& CopyPath(size_t index) {
        Node* cur_node = &root_;
        do {
            auto local_index = Index(index);
            if ((*cur_node)[local_index]) {
                (*cur_node)[local_index] = std::shared_ptr<Node>(new Node(*(*cur_node)[local_index]));
            } else {
                (*cur_node)[local_index] = std::shared_ptr<Node>(new Node());
            }

            cur_node = (*cur_node)[local_index].get();
            index = index >> 5;
        } while (index != 0);

        return *cur_node;
    }

    void EraseNode(size_t index) {
        Node* cur_node = &root_;
        while ((index >> 5) != 0) {
            auto local_index = Index(index);
            (*cur_node)[local_index] = std::shared_ptr<Node>(new Node(*(*cur_node)[local_index]));
            cur_node = (*cur_node)[local_index].get();
            index = index >> 5;
        }

        (*cur_node)[Index(index)] = nullptr;
    }

    static size_t Index(size_t index) {
        return index & ((1 << 5) - 1);
    }

    size_t size_ = 0;
    Node root_;
};

template <class Iterator>
Vector(Iterator, Iterator) -> Vector<std::iter_value_t<Iterator>>;
