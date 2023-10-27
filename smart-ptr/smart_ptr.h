#pragma once

#include <cstddef>
#include <string>
#include <algorithm>
#include <utility>

class WeakPtr;

struct RefCounter {
    size_t shared_cnt;
    size_t weak_cnt;
};

class SharedPtr {
    friend WeakPtr;

public:
    SharedPtr() = default;

    SharedPtr(const SharedPtr& ptr) : data_(ptr.data_), ref_cnt_(ptr.ref_cnt_) {
        if (data_) {
            ++ref_cnt_->shared_cnt;
        }
    }

    SharedPtr(SharedPtr&& ptr) : data_(ptr.data_), ref_cnt_(ptr.ref_cnt_) {
        ptr.data_ = nullptr;
        ptr.ref_cnt_ = nullptr;
    }

    SharedPtr(std::string* ptr) {
        data_ = ptr;
        if (data_) {
            ref_cnt_ = new RefCounter{1, 0};
        }
    }

    SharedPtr(const WeakPtr& ptr);

    const SharedPtr& operator=(SharedPtr ptr) {
        std::swap(data_, ptr.data_);
        std::swap(ref_cnt_, ptr.ref_cnt_);
        return *this;
    }

    std::string* Get() const {
        return data_;
    }

    std::string& operator*() const {
        return *data_;
    }

    std::string* operator->() const {
        return data_;
    }

    void Reset(std::string* ptr) {
        if (--ref_cnt_->shared_cnt == 0) {
            delete data_;
            if (ref_cnt_->weak_cnt == 0) {
                delete ref_cnt_;
            }
        }

        data_ = ptr;
        if (data_) {
            ref_cnt_ = new RefCounter{1, 0};
        }
    }

    ~SharedPtr() {
        if (data_ && --ref_cnt_->shared_cnt == 0) {
            delete data_;
            if (ref_cnt_->weak_cnt == 0) {
                delete ref_cnt_;
            }
        }
    }

private:
    std::string* data_ = nullptr;
    RefCounter* ref_cnt_ = nullptr;
};

class WeakPtr {
public:
    WeakPtr() : WeakPtr(SharedPtr()) {
    }

    WeakPtr(const SharedPtr& ptr) : data_(ptr.data_), ref_cnt_(ptr.ref_cnt_) {
        if (data_) {
            ++ref_cnt_->weak_cnt;
        }
    }

    WeakPtr(const WeakPtr& ptr) : data_(ptr.data_), ref_cnt_(ptr.ref_cnt_) {
        if (ref_cnt_) {
            ++ref_cnt_->weak_cnt;
        }
    }

    WeakPtr(WeakPtr&& ptr) : data_(ptr.data_), ref_cnt_(ptr.ref_cnt_) {
        ptr.data_ = nullptr;
        ptr.ref_cnt_ = nullptr;
    }

    const WeakPtr& operator=(WeakPtr ptr) {
        std::swap(data_, ptr.data_);
        std::swap(ref_cnt_, ptr.ref_cnt_);
        return *this;
    }

    bool IsExpired() const {
        if (!ref_cnt_ || ref_cnt_->shared_cnt == 0) {
            return true;
        } else {
            return false;
        }
    }

    SharedPtr Lock() const {
        SharedPtr ptr;
        if (!IsExpired()) {
            ptr.data_ = data_;
            ptr.ref_cnt_ = ref_cnt_;
            ++ptr.ref_cnt_->shared_cnt;
        }

        return ptr;
    }

    ~WeakPtr() {
        if (ref_cnt_ && --ref_cnt_->weak_cnt == 0 && ref_cnt_->shared_cnt == 0) {
            delete ref_cnt_;
        }
    }

private:
    std::string* data_ = nullptr;
    RefCounter* ref_cnt_ = nullptr;
};

SharedPtr::SharedPtr(const WeakPtr& ptr) {
    *this = ptr.Lock();
}
