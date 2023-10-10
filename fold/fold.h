#pragma once

#include <cstddef>

struct Sum {
    template <typename T>
    T operator()(const T& lhr, const T& rhr) {
        return lhr + rhr;
    }
};

struct Prod {
    template <typename T>
    T operator()(const T& lhr, const T& rhr) {
        return lhr * rhr;
    }
};

struct Concat {
    template <typename T>
    T operator()(const T& lhr, const T& rhr) {
        T res = lhr;
        res.reserve(res.size() + rhr.size());

        for (auto iter = rhr.begin(); iter != rhr.end(); ++iter) {
            res.push_back(*iter);
        }

        return res;
    }
};

class Length {
    size_t* length_;

public:
    Length(size_t* length) : length_(length) {
    }

    template <typename T>
    T operator()(const T&, const T& rhr) {
        ++(*length_);
        return rhr;
    }

    bool operator()(bool, bool second) {
        ++(*length_);
        return second;
    }
};

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    T result = init;
    for (auto iter = first; iter != last; ++iter) {
        result = func(result, *iter);
    }

    return result;
}
