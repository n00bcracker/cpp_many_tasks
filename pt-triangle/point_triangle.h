#pragma once

#include <stdexcept>

struct Point {
    int x, y;
};

struct Triangle {
    Point a, b, c;
};

#define INT64_CAST(x) static_cast<int64_t>(x)

bool IsPointInTriangle(const Triangle& t, const Point& pt) {
    int64_t prod1, prod2, prod3;
    prod1 = (INT64_CAST(t.a.x) - pt.x) * (INT64_CAST(t.b.y) - t.a.y) -
            (INT64_CAST(t.b.x) - t.a.x) * (INT64_CAST(t.a.y) - pt.y);
    prod2 = (INT64_CAST(t.b.x) - pt.x) * (INT64_CAST(t.c.y) - t.b.y) -
            (INT64_CAST(t.c.x) - t.b.x) * (INT64_CAST(t.b.y) - pt.y);
    prod3 = (INT64_CAST(t.c.x) - pt.x) * (INT64_CAST(t.a.y) - t.c.y) -
            (INT64_CAST(t.a.x) - t.c.x) * (INT64_CAST(t.c.y) - pt.y);

    if ((prod1 >= 0 && prod2 >= 0 && prod3 >= 0) || (prod1 <= 0 && prod2 <= 0 && prod3 <= 0)) {
        return true;
    } else {
        return false;
    }
}
