#pragma once

#include <cstddef>
#include <algorithm>

template <class Functor>
class ReverseUnaryFunctor {
    Functor functor_;

public:
    ReverseUnaryFunctor(Functor f) : functor_(f) {
    }

    template <typename T>
    bool operator()(const T& x) {
        return !functor_(x);
    }
};

template <class Functor>
class ReverseBinaryFunctor {
    Functor functor_;

public:
    ReverseBinaryFunctor(Functor f) : functor_(f) {
    }

    template <typename T>
    bool operator()(const T& lhr, const T& rhr) {
        return functor_(rhr, lhr);
    }
};

auto MakeReverseUnaryFunctor(auto functor) {
    return ReverseUnaryFunctor{functor};
}

auto MakeReverseBinaryFunctor(auto functor) {
    return ReverseBinaryFunctor{functor};
}

class Comparator {
    size_t comparisons_cnt_;

public:
    Comparator() : comparisons_cnt_(0) {
    }

    template <typename T>
    bool operator()(const T& lhr, const T& rhr) {
        if (rhr < lhr) {
            ++comparisons_cnt_;
        }

        return lhr < rhr;
    }

    size_t GetComparisons() {
        return comparisons_cnt_;
    }
};

template <class Iterator>
size_t ComparisonsCount(Iterator first, Iterator last) {
    Comparator comparator;
    std::sort(first, last, comparator);
    return comparator.GetComparisons();
}
