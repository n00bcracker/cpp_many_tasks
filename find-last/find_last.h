#pragma once

#include <stdexcept>

template <class Iterator, class T>
Iterator FindLast(Iterator first, Iterator last, const T& value) {
    for (auto iter = last; iter != first;) {
        if (*--iter == value) {
            return iter;
        }
    }

    return last;
}
