#pragma once

#include <type_traits>
#include <concepts>
#include <typeinfo>
#include <utility>
#include <type_traits>

template <class T>
concept NotAny = !std::same_as<std::remove_cvref_t<T>, class Any>;

class Any {
public:
    Any() {
    }

    // T&& - universal (forwarding) reference
    // use std::forward inside this constructor
    template <NotAny T>
    Any(T&& value) : value_ptr_(new AnyValue<std::remove_cvref_t<T>>(std::forward<T>(value))) {
    }

    Any(const Any& other) {
        if (!other.Empty()) {
            value_ptr_ = other.value_ptr_->Clone();
        }
    }

    Any(Any&& other) {
        value_ptr_ = other.value_ptr_;
        other.value_ptr_ = nullptr;
    }

    Any& operator=(const Any& other) {
        if (this != &other) {
            if (!other.Empty()) {
                AnyValueBase* tmp = other.value_ptr_->Clone();
                delete value_ptr_;
                value_ptr_ = tmp;
            } else {
                delete value_ptr_;
                value_ptr_ = nullptr;
            }
        }
        return *this;
    }

    Any& operator=(Any&& other) {
        if (this != &other) {
            delete value_ptr_;
            value_ptr_ = other.value_ptr_;
            other.value_ptr_ = nullptr;
        }
        return *this;
    }

    ~Any() {
        delete value_ptr_;
    }

    bool Empty() const {
        return value_ptr_ == nullptr;
    }

    void Clear() {
        if (value_ptr_) {
            delete value_ptr_;
            value_ptr_ = nullptr;
        }
    }

    void Swap(Any& other) {
        std::swap(value_ptr_, other.value_ptr_);
    }

    template <class T>
    const T& GetValue() const {
        if (!Empty()) {
            return dynamic_cast<AnyValue<T>&>(*value_ptr_).value_;
        } else {
            throw std::bad_cast();
        }
    }

private:
    class AnyValueBase {
    public:
        virtual AnyValueBase* Clone() const = 0;

        virtual ~AnyValueBase() {
        }
    };

    template <NotAny T>
    class AnyValue : public AnyValueBase {
        friend Any;

        AnyValue(T value) : value_(std::move(value)) {
        }

        AnyValueBase* Clone() const override {
            return new AnyValue(value_);
        }

    private:
        T value_;
    };

    AnyValueBase* value_ptr_ = nullptr;
};
