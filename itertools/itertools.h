#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

template <class Iterator>
class Sequence {
public:
    Sequence(Iterator begin, Iterator end) : begin_{begin}, end_{end} {
    }

    Iterator begin() const {
        return begin_;
    }

    Iterator end() const {
        return end_;
    }

private:
    Iterator begin_, end_;
};

template <class Iterator>
class RepeatIterator {
public:
    RepeatIterator(Iterator begin, Iterator end, size_t count = 0)
        : begin_{begin}, it_{begin}, end_{end}, count_{count} {
    }

    RepeatIterator& operator++() {
        if (!(++it_ != end_)) {
            ++count_;
            it_ = begin_;
        }
        return *this;
    }

    auto operator*() const {
        return *it_;
    }

    bool operator==(const RepeatIterator& rhs) const = default;

private:
    Iterator begin_, it_, end_;
    size_t count_;
};

class RangeIterator {
public:
    RangeIterator(int64_t begin, uint64_t step) : value_(begin), step_(step) {
    }

    RangeIterator& operator++() {
        value_ += step_;
        return *this;
    }

    int operator*() const {
        return value_;
    }

    bool operator==(const RangeIterator& rhr) const = default;

private:
    int64_t value_;
    uint64_t step_;
};

template <class FirstIterator, class SecondIterator>
class ZipIterator {
public:
    ZipIterator(const FirstIterator& first, const SecondIterator& second)
        : first_iterator_(first), second_iterator_(second) {
    }

    ZipIterator& operator++() {
        ++first_iterator_;
        ++second_iterator_;
        return *this;
    }

    auto operator*() const {
        return std::make_pair(*first_iterator_, *second_iterator_);
    }

    bool operator==(const ZipIterator& rhr) const {
        return first_iterator_ == rhr.first_iterator_ || second_iterator_ == rhr.second_iterator_;
    }

private:
    FirstIterator first_iterator_;
    SecondIterator second_iterator_;
};

template <class Iterator>
class GroupIterator {
public:
    GroupIterator(Iterator begin, Iterator end) : iter_(begin), end_(end) {
    }

    GroupIterator& operator++() {
        Iterator cur_iter = iter_;
        while (cur_iter != end_ && *cur_iter == *iter_) {
            ++cur_iter;
        }

        iter_ = cur_iter;
        return *this;
    }

    auto operator*() const {
        GroupIterator copy = *this;
        return Sequence(iter_, (++copy).iter_);
    }

    bool operator==(const GroupIterator& rhr) const {
        return iter_ == rhr.iter_;
    }

private:
    Iterator iter_, end_;
};

auto Repeat(const auto& sequence, size_t n) {
    auto begin = sequence.begin();
    auto end = sequence.end();
    if (n && (begin != end)) {
        return Sequence{RepeatIterator{begin, end}, RepeatIterator{begin, end, n}};
    } else {
        return Sequence{RepeatIterator{end, end}, RepeatIterator{end, end}};
    }
}

auto Range(int64_t from, int64_t to, uint64_t step = 1) {
    if (to > from) {
        uint64_t max_k = (to - 1 - from) / step + 1;
        return Sequence(RangeIterator(from, step), RangeIterator(from + max_k * step, step));
    } else {
        return Sequence(RangeIterator(from, step), RangeIterator(from, step));
    }
}

auto Range(int64_t to) {
    return Range(0, to);
}

auto Zip(const auto& sequence1, const auto& sequence2) {
    auto begin_first_seq = sequence1.begin();
    auto begin_second_seq = sequence2.begin();
    auto end_first_seq = sequence1.end();
    auto end_second_seq = sequence2.end();

    return Sequence(ZipIterator(begin_first_seq, begin_second_seq),
                    ZipIterator(end_first_seq, end_second_seq));
}

auto Group(const auto& sequence) {
    auto begin = sequence.begin();
    auto end = sequence.end();
    return Sequence(GroupIterator(begin, end), GroupIterator(end, end));
}