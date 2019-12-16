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
    SECTION("exponential")
    {
        REQUIRE(make_function_value_series(
                    [](auto x) { return rib::math::maclaurin::exp(x, 16); },
                    [](auto x) { return std::exp(x); }) < 1e-16);
    }
    SECTION("sin")
    {
        REQUIRE(make_function_value_series(
                    [](auto x) { return rib::math::maclaurin::sin(x, 16); },
                    [](auto x) { return std::sin(x); }) < 1e-16);
    }
    SECTION("cos")
    {
        REQUIRE(make_function_value_series(
                    [](auto x) { return rib::math::maclaurin::cos(x, 16); },
                    [](auto x) { return std::cos(x); }) < 1e-16);
    }
    SECTION("sinh")
    {
        REQUIRE(make_function_value_series(
                    [](auto x) { return rib::math::maclaurin::sinh(x, 16); },
                    [](auto x) { return std::sinh(x); }) < 1e-16);
    }
    SECTION("cosh")
    {
        REQUIRE(make_function_value_series(
                    [](auto x) { return rib::math::maclaurin::cosh(x, 16); },
                    [](auto x) { return std::cosh(x); }) < 1e-16);
    }
}
