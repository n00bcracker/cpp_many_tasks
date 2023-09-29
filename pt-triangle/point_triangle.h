#pragma once

#include <stdexcept>

struct Point {
    int x, y;
};

struct Triangle {
    Point a, b, c;
};

bool IsPointInTriangle(const Triangle& t, const Point& pt) {
    int64_t prod1, prod2, prod3;
    prod1 = static_cast<int64_t>(t.a.x - pt.x) * (t.b.y - t.a.y) - (t.b.x - t.a.x) * (t.a.y - pt.y);
    prod2 = static_cast<int64_t>(t.b.x - pt.x) * (t.c.y - t.b.y) - (t.c.x - t.b.x) * (t.b.y - pt.y);
    prod3 = static_cast<int64_t>(t.c.x - pt.x) * (t.a.y - t.c.y) - (t.a.x - t.c.x) * (t.c.y - pt.y);

    if ((prod1 >= 0 && prod2 >= 0 && prod3 >= 0) || (prod1 <= 0 && prod2 <= 0 && prod3 <= 0)) {
        return true;
    } else {
        return false;
    }
}
