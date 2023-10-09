#pragma once

#include <stdexcept>

template <class Iterator>
Iterator LocalMax(Iterator first, Iterator last) {
    for (auto iter = first; iter != last; ++iter) {
        bool greater_than_left = true;
        bool greater_than_right = true;

        if (iter != first) {
            greater_than_left = *((--iter)++) < *iter;
        }

        if ((++iter)-- != last) {
            greater_than_right = *((++iter)--) < *iter;
        }

        if (greater_than_left && greater_than_right) {
            return iter;
        }
    }

    return last;
}
