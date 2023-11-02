#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>

template <class T>
class List {
public:
    struct ListNode;
    struct BaseListNode;
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator() = default;

        Iterator(BaseListNode* iter) : it_(iter) {
        }

        Iterator& operator++() {
            it_ = it_->next;
            return *this;
        }

        Iterator operator++(int) {
            auto tmp = it_;
            it_ = it_->next;
            return Iterator(tmp);
        }

        Iterator& operator--() {
            it_ = it_->prev;
            return *this;
        }

        Iterator operator--(int) {
            auto tmp = it_;
            it_ = it_->prev;
            return Iterator(tmp);
        }

        BaseListNode* GetNode() {
            return it_;
        }

        T& operator*() const {
            return static_cast<ListNode*>(it_)->value;
        }

        T* operator->() const {
            return &(static_cast<ListNode*>(it_)->value);
        }

        bool operator==(const Iterator& other) const {
            return it_ == other.it_;
        }

        bool operator!=(const Iterator& other) const {
            return it_ != other.it_;
        }

    private:
        BaseListNode* it_ = nullptr;
    };

    struct BaseListNode {
        BaseListNode* prev;
        BaseListNode* next;

        BaseListNode() : prev(nullptr), next(nullptr) {
        }
    };

    struct ListNode : BaseListNode {
        T value;

        ListNode(const T& t) : value(t) {
        }

        ListNode(T&& t) : value(std::move(t)) {
        }
    };

    List() {
        head_ = new BaseListNode();
        head_->prev = head_;
    }

    List(const List& other) : List() {
        for (auto it = other.Begin(); it != other.End(); ++it) {
            PushBack(*it);
        }
    }

    List(List&& other) : head_(other.head_), size_(other.size_) {
        other.head_ = new BaseListNode();
        other.head_->prev = other.head_;
        other.size_ = 0;
    }

    ~List() {
        Clear();
        delete head_;
    }

    List& operator=(List other) {
        std::swap(this->head_, other.head_);
        std::swap(this->size_, other.size_);
        return *this;
    }

    bool IsEmpty() const {
        return size_ == 0;
    }

    size_t Size() const {
        return size_;
    }

    void PushBack(const T& t) {
        if (!IsEmpty()) {
            auto node = new ListNode(t);
            LinkAfter(node, head_->prev->prev);
            ++size_;
        } else {
            PushFront(t);
        }
    }

    void PushBack(T&& t) {
        if (!IsEmpty()) {
            auto node = new ListNode(std::move(t));
            LinkAfter(node, head_->prev->prev);
            ++size_;
        } else {
            PushFront(std::move(t));
        }
    }

    void PushFront(const T& t) {
        auto node = new ListNode(t);
        node->next = head_;
        node->prev = head_->prev;
        head_->prev = node;
        head_ = node;
        ++size_;
    }

    void PushFront(T&& t) {
        auto node = new ListNode(std::move(t));
        node->next = head_;
        node->prev = head_->prev;
        head_->prev = node;
        head_ = node;
        ++size_;
    }

    T& Front() {
        return static_cast<ListNode*>(head_)->value;
    }

    const T& Front() const {
        return static_cast<ListNode*>(head_)->value;
    }

    T& Back() {
        return static_cast<ListNode*>(head_->prev->prev)->value;
    }

    const T& Back() const {
        return static_cast<ListNode*>(head_->prev->prev)->value;
    }

    void PopBack() {
        auto node = head_->prev->prev;
        Unlink(node);
        --size_;
    }

    void PopFront() {
        auto node = head_;
        Unlink(node);
        --size_;
    }

    void Erase(Iterator it) {
        Unlink(it.GetNode());
        --size_;
    }

    Iterator Begin() const {
        return Iterator(head_);
    }

    Iterator End() const {
        return Iterator(head_->prev);
    }

private:
    void Clear() {
        for (auto it = Begin(); it != End();) {
            Erase(it++);
        }
    }

    void LinkAfter(BaseListNode* target, BaseListNode* after) {
        target->prev = after;
        target->next = after->next;
        after->next = target;
        target->next->prev = target;
    }

    void Unlink(BaseListNode* node) {
        if (node == head_) {
            head_ = node->next;
        } else {
            node->prev->next = node->next;
        }

        node->next->prev = node->prev;
        delete static_cast<ListNode*>(node);
    }

    BaseListNode* head_ = nullptr;
    size_t size_ = 0;
};

template <class T>
List<T>::Iterator begin(List<T>&);

template <class T>
List<T>::Iterator end(List<T>&);
