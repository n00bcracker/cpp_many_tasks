#pragma once

#include <exception>
#include <queue>
#include <utility>
#include <memory>

template <typename It, typename P, typename F>
void TransformIf(It begin, It end, P p, F f) {
    std::queue<std::pair<It, std::unique_ptr<typename It::value_type>>> changed_elems;
    for (auto it = begin; it != end; ++it) {
        std::unique_ptr<typename It::value_type> elem;
        bool predicate;
        int except_type;

        try {
            except_type = 0;
            try {
                predicate = p(*it);
            } catch (...) {
                except_type = 1;
                throw;
            }

            if (predicate) {
                try {
                    elem =
                        std::unique_ptr<typename It::value_type>(new typename It::value_type(*it));
                } catch (...) {
                }

                try {
                    f(*it);
                } catch (...) {
                    except_type = 2;
                    throw;
                }

                try {
                    changed_elems.emplace(it, std::move(elem));
                } catch (...) {
                }
            }
        } catch (...) {
            if (except_type == 2) {
                try {
                    *it = *elem;
                } catch (...) {
                }
            }

            while (!changed_elems.empty()) {
                try {
                    *changed_elems.front().first = *changed_elems.front().second;
                } catch (...) {
                }

                changed_elems.pop();
            }

            throw;
        }
    }
}
