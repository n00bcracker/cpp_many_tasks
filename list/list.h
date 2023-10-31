#pragma once

#include <__type_traits/is_swappable.h>
#include <cstddef>
#include <iterator>
#include <memory>
#include <algorithm>
#include <utility>

template <class T>
class List {
public:
    struct ListNode;
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator() : head_(nullptr), it_(nullptr) {
        }

        Iterator(std::shared_ptr<ListNode> head, std::shared_ptr<ListNode> iter) :  head_(head), it_(iter) {
        }

        const std::shared_ptr<ListNode>& GetNode() const {
            return it_;
        }

        Iterator& operator++() {
            it_ = it_->next;
            return *this;
        }

        Iterator operator++(int) {
            auto tmp = it_;
            it_ = it_->next;
            return Iterator(head_, tmp);
        }

        Iterator& operator--() {
            if (it_) {
                it_ = it_->prev;
            } else {
                it_ = head_->prev;
            }

            return *this;
        }

        Iterator operator--(int) {
            auto tmp = it_;
            if (it_) {
                it_ = it_->prev;
            } else {
                it_ = head_->prev;
            }
            return Iterator(head_, tmp);
        }

        T& operator*() const {
            return it_->value;
        }

        T* operator->() const {
            return &(it_->value);
        }

        bool operator==(const Iterator& other) const {
            return it_ == other.it_;
        }

        bool operator!=(const Iterator& other) const {
            return it_ != other.it_;
        }

    private:
        std::shared_ptr<ListNode> head_;
        std::shared_ptr<ListNode> it_;
    };

    struct ListNode {
        T value;
        std::shared_ptr<ListNode> prev;
        std::shared_ptr<ListNode> next;

        ListNode(const T& t) : value(t), prev(nullptr), next(nullptr) {
        }

        ListNode(T&& t) : value(std::move(t)), prev(nullptr), next(nullptr) {
        }
    };

    List() = default;
    List(const List& other) {
        for (auto it = other.Begin(); it != other.End(); ++it) {
            PushBack(*it);
        }
    }

    List(List&& other) : head_(other.head_), size_(other.size_) {
        other.head_ = nullptr;
        other.size_ = 0;
    }

    ~List() {
        Clear();
    }

    List& operator=(List other) {
        std::swap(this->head_, other.head_);
        std::swap(this->size_, other.size_);
        return *this;
    }

    bool IsEmpty() const {
        return head_ == nullptr;
    }

    size_t Size() const {
        return size_;
    }

    void PushBack(const T& t) {
        auto node = std::shared_ptr<ListNode>(new ListNode(t));
        if (head_) {
            LinkAfter(node, head_->prev);
        } else {
            head_ = node;
            head_->prev = head_;
        }

        ++size_;
    }

    void PushBack(T&& t) {
        auto node = std::shared_ptr<ListNode>(new ListNode(std::move(t)));
        if (head_) {
            LinkAfter(node, head_->prev);
        } else {
            head_ = node;
            head_->prev = head_;
        }

        ++size_;
    }

    void PushFront(const T& t) {
        auto node = std::shared_ptr<ListNode>(new ListNode(t));
        node->next = head_;
        if (head_) {
            node->prev = head_->prev;
            head_->prev = node;
        } else {
            node->prev = node;
        }

        head_ = node;
        ++size_;
    }

    void PushFront(T&& t) {
        auto node = std::shared_ptr<ListNode>(new ListNode(std::move(t)));
        node->next = head_;
        if (head_) {
            node->prev = head_->prev;
            head_->prev = node;
        } else {
            node->prev = node;
        }

        head_ = node;
        ++size_;
    }

    T& Front() {
        return head_->value;
    }

    const T& Front() const {
        return head_->value;
    }

    T& Back() {
        return head_->prev->value;
    }

    const T& Back() const {
        return head_->prev->value;
    }

    void PopBack() {
        auto& node = head_->prev;
        Unlink(node);
        --size_;
    }

    void PopFront() {
        auto& node = head_;
        Unlink(node);
        --size_;
    }

    void Erase(Iterator it) {
        Unlink(it.GetNode());
        --size_;
    }

    Iterator Begin() const {
        return Iterator(head_, head_);
    }

    Iterator End() const {
        return Iterator(head_, nullptr);
    }

private:
    void Clear() {
        auto node = head_;
        head_ = nullptr;
        while (node) {
            node->prev = nullptr;
            node = node->next;
        }

        size_ = 0;
    }

    void LinkAfter(std::shared_ptr<ListNode> target, std::shared_ptr<ListNode> after) {
        target->prev = after;
        target->next = after->next;
        after->next = target;
        if (target->next) {
            target->next->prev = target;
        } else {
            head_->prev = target;
        }
    }

    void Unlink(std::shared_ptr<ListNode> node) {
        if (node == head_) {
            head_ = node->next;
            if (head_) {
                head_->prev = node->prev;
            }
        } else {
            node->prev->next = node->next;
            if (node->next) {
                node->next->prev = node->prev;
            } else {
                head_->prev = node->prev;
            }
        }
    }

    std::shared_ptr<ListNode> head_ = nullptr;
    size_t size_ = 0;
};

template <class T>
List<T>::Iterator begin(List<T>&);

template <class T>
List<T>::Iterator end(List<T>&);
