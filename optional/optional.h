#pragma once

#include <exception>
#include <utility>
template <class T>
class Optional {
    class NoValueException : public std::exception {};

public:
    Optional() {
    }

    Optional(const T& value) : has_value_(true) {
        new (object_) T(value);
    }

    Optional(T&& value) : has_value_(true) {
        new (object_) T(std::forward<T>(value));
    }

    Optional(const Optional& other) : has_value_(other.has_value_) {
        if (has_value_) {
            new (object_) T(*reinterpret_cast<const T*>(other.object_));
        }
    }

    // This constructor must not change other.HasValue()
    // Just move value (if present)
    Optional(Optional&& other) : has_value_(other.has_value_) {
        if (has_value_) {
            new (object_) T(std::move(*reinterpret_cast<T*>(other.object_)));
        }
    }

    ~Optional() {
        if (has_value_) {
            reinterpret_cast<T*>(object_)->~T();
        }
    }

    Optional& operator=(const Optional& other) {
        if (this != &other) {
            Optional tmp(other);
            Reset();
            has_value_ = tmp.has_value_;
            if (has_value_) {
                new (object_) T(*reinterpret_cast<T*>(tmp.object_));
            }
        }

        return *this;
    }

    // This method must not change other.HasValue()
    // Just move value (if present)
    Optional& operator=(Optional&& other) {
        if (this != &other) {
            Optional tmp(std::move(other));
            Reset();
            has_value_ = tmp.has_value_;
            if (has_value_) {
                new (object_) T(std::move(*reinterpret_cast<T*>(tmp.object_)));
            }
        }

        return *this;
    }

    T& operator*() & {
        return *reinterpret_cast<T*>(object_);
    }

    const T& operator*() const& {
        return *reinterpret_cast<const T*>(object_);
    }

    T&& operator*() && {
        return std::forward<T>(*reinterpret_cast<T*>(object_));
    }

    T* operator->() {
        return reinterpret_cast<T*>(object_);
    }

    const T* operator->() const {
        return reinterpret_cast<const T*>(object_);
    }

    T& Value() & {
        if (has_value_) {
            return **this;
        } else {
            throw NoValueException();
        }
    }

    const T& Value() const& {
        if (has_value_) {
            return **this;
        } else {
            throw NoValueException();
        }
    }

    T&& Value() && {
        if (has_value_) {
            return std::forward<T>(*reinterpret_cast<T*>(object_));
        } else {
            throw NoValueException();
        }
    }

    bool HasValue() const {
        return has_value_;
    }

    void Reset() {
        if (has_value_) {
            reinterpret_cast<T*>(object_)->~T();
            has_value_ = false;
        }
    }

private:
    alignas(T) unsigned char object_[sizeof(T)];
    bool has_value_ = false;
};
