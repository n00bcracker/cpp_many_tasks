#pragma once

#include <stdexcept>

template <class Iterator>
void Swap(Iterator first, Iterator second) {
    auto tmp = *first;
    *first = *second;
    *second = tmp;
}

template <class Iterator, class Predicate>
Iterator Partition(Iterator first, Iterator last, Predicate pred) {
    Iterator slow_iter = first;
    for (auto iter = first; iter != last; ++iter) {
        if (pred(*iter)) {
            Swap(slow_iter, iter);
            ++slow_iter;
        }
    }

    return slow_iter;
}
