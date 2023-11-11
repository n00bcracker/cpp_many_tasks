#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>

class ListHook {
public:
    bool IsLinked() const {
        if (next_) {
            return true;
        } else {
            return false;
        }
    }

    void Unlink() {
        prev_->next_ = next_;
        next_->prev_ = prev_;
        prev_ = nullptr;
        next_ = nullptr;
    }

    ListHook(const ListHook&) = delete;
    ListHook& operator=(const ListHook&) = delete;

    ListHook* next_ = nullptr;
    ListHook* prev_ = nullptr;

protected:
    ListHook() {
    }

    // Must unlink element from list
    ~ListHook() {
        if (IsLinked()) {
            Unlink();
        }
    }

    // that helper function might be useful
    void LinkBefore(ListHook* other) {
        if (IsLinked()) {
            Unlink();
        }

        prev_ = other->prev_;
        next_ = other;
        prev_->next_ = this;
        next_->prev_ = this;
    }

    template <class T>
    friend class List;
};

template <class T>
class List {
public:
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator() = default;

        Iterator(ListHook* node) : it_(node) {
        }

        Iterator& operator++() {
            it_ = it_->next_;
            return *this;
        }

        Iterator operator++(int) {
            auto tmp = it_;
            it_ = it_->next_;
            return Iterator(tmp);
        }

        Iterator& operator--() {
            it_ = it_->prev_;
            return *this;
        }

        Iterator operator--(int) {
            auto tmp = it_;
            it_ = it_->prev_;
            return Iterator(tmp);
        }

        T& operator*() const {
            return *static_cast<T*>(it_);
        }

        T* operator->() const {
            return static_cast<T*>(it_);
        }

        bool operator==(const Iterator& other) const {
            return it_ == other.it_;
        }

        bool operator!=(const Iterator& other) const {
            return it_ != other.it_;
        }

    private:
        ListHook* it_ = nullptr;
    };

    List() : dummy_() {
        dummy_.prev_ = &dummy_;
        dummy_.next_ = &dummy_;
    }

    List(const List&) = delete;

    List(List&& other) : List() {
        dummy_.LinkBefore(&other.dummy_);
        other.dummy_.Unlink();
        other.dummy_.next_ = &other.dummy_;
        other.dummy_.prev_ = &other.dummy_;
    }

    void Clear() {
        for (auto it = Begin(); it != End();) {
            auto tmp = it++;
            tmp->Unlink();
        }
    }

    // must unlink all elements from list
    ~List() {
        Clear();
    }

    List& operator=(const List&) = delete;
    List& operator=(List&& other) {
        Clear();
        dummy_.LinkBefore(&other.dummy_);
        other.dummy_.Unlink();
        other.dummy_.next_ = &other.dummy_;
        other.dummy_.prev_ = &other.dummy_;
        return *this;
    }

    bool IsEmpty() const {
        return Begin() == End();
    }

    // this method is allowed to be O(n)
    size_t Size() const {
        size_t size = 0;
        for (auto it = Begin(); it != End(); ++it) {
            ++size;
        }

        return size;
    }

    // note that IntrusiveList doesn't own elements,
    // and never copies or moves T
    void PushBack(T* elem) {
        elem->LinkBefore(&dummy_);
    }

    void PushFront(T* elem) {
        elem->LinkBefore(dummy_.next_);
    }

    T& Front() {
        return *static_cast<T*>(dummy_.next_);
    }

    const T& Front() const {
        return *static_cast<T*>(dummy_.next_);
    }

    T& Back() {
        return *static_cast<T*>(dummy_.prev_);
    }

    const T& Back() const {
        return *static_cast<T*>(dummy_.prev_);
    }

    void PopBack() {
        dummy_.prev_->Unlink();
    }

    void PopFront() {
        dummy_.next_->Unlink();
    }

    Iterator Begin() const {
        return Iterator(dummy_.next_);
    }

    Iterator End() const {
        return Iterator(dummy_.prev_->next_);
    }

    // complexity of this function must be O(1)
    Iterator IteratorTo(T* element) {
        return Iterator(element);
    }

private:
    ListHook dummy_;
};

template <class T>
List<T>::Iterator begin(List<T>& list) {
    return list.Begin();
}

template <class T>
List<T>::Iterator end(List<T>& list) {
    return list.End();
}
