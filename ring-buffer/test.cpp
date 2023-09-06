#include <catch.hpp>
#include <util.h>
#include <ring_buffer.h>
#include <ranges>

TEST_CASE("Empty") {
    RingBuffer buffer(1);

    REQUIRE(0u == buffer.Size());
    REQUIRE(buffer.Empty());
}

TEST_CASE("PushAndPop") {
    RingBuffer buffer(2);

    int i;
    REQUIRE(buffer.TryPush(0));
    REQUIRE(buffer.TryPush(1));
    REQUIRE(!buffer.TryPush(2));

    REQUIRE(2u == buffer.Size());
    REQUIRE(!buffer.Empty());

    REQUIRE(buffer.TryPop(&i));
    REQUIRE(0 == i);
    REQUIRE(buffer.TryPop(&i));
    REQUIRE(1 == i);

    REQUIRE(!buffer.TryPop(&i));
    REQUIRE(0u == buffer.Size());
    REQUIRE(buffer.Empty());
}

TEST_CASE("Random") {
    RandomGenerator rnd(73467534);
    RingBuffer buffer(10);

    auto next_element = 0;
    auto next_received_element = 0;
    for (auto i = 0; i < 100'000; ++i) {
        if (rnd.GenInt(0, 1) == 0) {
            if (buffer.TryPush(next_element)) {
                next_element++;
            }
        } else if (int element; buffer.TryPop(&element)) {
            REQUIRE(next_received_element++ == element);
        }
    }
}
