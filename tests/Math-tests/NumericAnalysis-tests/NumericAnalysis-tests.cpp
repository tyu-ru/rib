#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "rib/Math/NumericAnalysis.hpp"

TEST_CASE("residual sum of squares - self", "[Math]")
{
    int a[3] = {1, 2, 3};
    REQUIRE(rib::math::rss(a, a + 3, a) == 0);
}

TEST_CASE("residual sum of squares - a", "[Math]")
{
    int a[3] = {1, 2, 3};
    int b[3] = {0, 3, 1};
    REQUIRE(rib::math::rss(a, a + 3, b) == 1 + 1 + 4);
}
