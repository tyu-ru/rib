#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "rib/Math/Taylor.hpp"
#include "rib/Math/NumericAnalysis.hpp"

template <class F_Actual, class F_Expected>
long double make_function_value_series(F_Actual fa, F_Expected fe)
{
    std::vector<long double> actual, expected;
    for (int i = -1000; i <= 1000; ++i) {
        double x = i / 1000.;
        actual.push_back(fa(x));
        expected.push_back(fe(x));
    }
    return rib::math::rss(actual.begin(), actual.end(), expected.begin()) / actual.size();
}

TEST_CASE("maclaurin", "[Math]")
{
    constexpr double l = -1., r = 1., d = 0.1;
    SECTION("exponential")
    {
        for (auto x = l; x <= r; x += d) {
            DYNAMIC_SECTION("x = " << x)
            {
                REQUIRE(rib::math::maclaurin::exp(x, 16) == Approx(std::exp(x)));
            }
        }
    }
    SECTION("sin")
    {
        for (auto x = l; x <= r; x += d) {
            DYNAMIC_SECTION("x = " << x)
            {
                REQUIRE(rib::math::maclaurin::sin(x, 16) == Approx(std::sin(x)));
            }
        }
    }
    SECTION("cos")
    {
        for (auto x = l; x <= r; x += d) {
            DYNAMIC_SECTION("x = " << x)
            {
                REQUIRE(rib::math::maclaurin::cos(x, 16) == Approx(std::cos(x)));
            }
        }
    }
    SECTION("sinh")
    {
        for (auto x = l; x <= r; x += d) {
            DYNAMIC_SECTION("x = " << x)
            {
                REQUIRE(rib::math::maclaurin::sinh(x, 16) == Approx(std::sinh(x)));
            }
        }
    }
    SECTION("cosh")
    {
        for (auto x = l; x <= r; x += d) {
            DYNAMIC_SECTION("x = " << x)
            {
                REQUIRE(rib::math::maclaurin::cosh(x, 16) == Approx(std::cosh(x)));
            }
        }
    }
}
