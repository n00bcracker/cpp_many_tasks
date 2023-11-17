#pragma once

#include <stdexcept>
#include <system_error>
#include <utility>
#include <variant>
#include <exception>
#include <type_traits>

namespace tag {

inline constexpr struct Exception {
} kEx;

}  // namespace tag

class BadClass {
public:
    operator std::exception_ptr() {
        throw 42;
    }
};

template <class T>
class Try {
public:
    Try() {
        try {
            res_.template emplace<1>(BadClass());
        } catch (...) {
        }
    }

    explicit Try(const T& t) : res_(std::in_place_index<0>, t) {
    }

    explicit Try(T&& t) : res_(std::in_place_index<0>, std::move(t)) {
    }

    template <class Err>
    Try(tag::Exception, Err&& err) : res_(std::make_exception_ptr(std::forward<Err>(err))) {
    }

    Try(tag::Exception, std::exception_ptr e_ptr) : res_(e_ptr) {
    }

    Try(const Try<T>& other) = delete;
    Try(Try<T>&& other) = delete;
    const Try& operator=(const Try<T>& other) = delete;
    const Try& operator=(Try<T>&& other) = delete;

    const T& Value() const {
        auto alt_idx = res_.index();
        if (alt_idx == 0) {
            return std::get<0>(res_);
        } else if (alt_idx == 1) {
            std::rethrow_exception(std::get<1>(res_));
        } else {
            throw std::domain_error("Object is empty");
        }
    }

    void Throw() const {
        if (res_.index() == 1) {
            std::rethrow_exception(std::get<1>(res_));
        } else {
            throw std::domain_error("No exception");
        }
    }

    bool IsFailed() const {
        return res_.index() == 1;
    }

private:
    std::variant<T, std::exception_ptr> res_;
};

template <>
class Try<void> {
public:
    Try() {
        try {
            res_.template emplace<0>(BadClass());
        } catch (...) {
        }
    }

    template <class Err>
    Try(tag::Exception, Err&& err) : res_(std::make_exception_ptr(std::forward<Err>(err))) {
    }

    Try(tag::Exception, std::exception_ptr e_ptr) : res_(e_ptr) {
    }

    Try(const Try<void>& other) = delete;
    Try(Try<void>&& other) = delete;
    const Try& operator=(const Try<void>& other) = delete;
    const Try& operator=(Try<void>&& other) = delete;

    void Throw() const {
        if (std::holds_alternative<std::exception_ptr>(res_)) {
            std::rethrow_exception(std::get<std::exception_ptr>(res_));
        } else {
            throw std::domain_error("No exception");
        }
    }

    bool IsFailed() const {
        return std::holds_alternative<std::exception_ptr>(res_);
    }

private:
    std::variant<std::exception_ptr> res_;
};

template <class Function, class... Args>
auto TryRun(Function func, Args&&... args) {
    using ReturnType = decltype(func(std::forward<Args>(args)...));
    try {
        if constexpr (std::is_void_v<ReturnType>) {
            func(std::forward<Args>(args)...);
            return Try<ReturnType>();
        } else {
            return Try<ReturnType>(func(std::forward<Args>(args)...));
        }
    } catch (const std::exception&) {
        return Try<ReturnType>(tag::kEx, std::current_exception());
    } catch (const char* message) {
        return Try<ReturnType>(tag::kEx, std::runtime_error(message));
    } catch (int err) {
        return Try<ReturnType>(tag::kEx, std::system_error(err, std::generic_category()));
    } catch (...) {
        return Try<ReturnType>(tag::kEx, std::runtime_error("Unknown exception"));
    }
}
