#include <catch.hpp>
#include <stack.h>
#include <ranges>

TEST_CASE("Simple") {
    Stack s;
    s.Push(0);
    s.Push(1);
    s.Push(2);
    REQUIRE(2 == s.Top());
    REQUIRE(s.Pop());
    REQUIRE(1 == s.Top());
    REQUIRE(s.Pop());
    REQUIRE(0 == s.Top());
    REQUIRE(s.Pop());
    REQUIRE(!s.Pop());
}

TEST_CASE("Empty") {
    Stack s;
    REQUIRE(s.Empty());
    REQUIRE(0u == s.Size());
    s.Push(1);
    REQUIRE(!s.Empty());
    REQUIRE(1u == s.Size());
    REQUIRE(s.Pop());
}

TEST_CASE("Long") {
    Stack s;
    constexpr auto kNumElements = 50'000;
    for (auto i = 0; i < kNumElements; ++i) {
        s.Push(i);
    }
    for (auto i = 0; i < kNumElements; ++i) {
        REQUIRE(kNumElements - i - 1 == s.Top());
        REQUIRE(s.Pop());
    }
    REQUIRE(s.Empty());
    REQUIRE(0u == s.Size());
}
