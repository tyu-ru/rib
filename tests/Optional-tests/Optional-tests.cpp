#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "rib/Container/Optional.hpp"
#include "rib/Utils/Lambda.hpp"

#include "test_util.hpp"

using namespace rib;

TEST_CASE("Optional - constructible", "[container]")
{
    CHECK(std::is_default_constructible_v<Optional<int>>);
    CHECK(std::is_constructible_v<Optional<int>, int>);
    CHECK(std::is_constructible_v<Optional<int>, std::nullopt_t>);
}

TEST_CASE("Optional - unwrap", "[container]")
{
    CHECK(std::is_same_v<decltype(std::declval<Optional<int>>().unwrap()),
                         Optional<int>>);
    CHECK(std::is_same_v<decltype(std::declval<Optional<Optional<int>>>().unwrap()),
                         Optional<int>>);

    CHECK(Optional<int>(1).unwrap() == 1);
    CHECK(Optional<Optional<int>>(Optional<int>(1)).unwrap() == 1);
}

TEST_CASE("Optional - visit", "[container]")
{
    CHECK(std::is_same_v<decltype(std::declval<const Optional<int>&>().visit(std::declval<long(int)>())),
                         const Optional<int>&>);
    CHECK(std::is_same_v<decltype(std::declval<Optional<int>&>().visit(std::declval<long(int)>())),
                         Optional<int>&>);
    CHECK(std::is_same_v<decltype(std::declval<Optional<int>>().visit(std::declval<long(int)>())),
                         Optional<int>>);

    CHECK(Optional<int>(1).visit(VISITOR(x, x += 1;)) == 2);
    CHECK(Optional<int>().visit(VISITOR(x, x += 1;)) == std::nullopt);
}

TEST_CASE("Optional - map", "[container]")
{
    CHECK(std::is_same_v<decltype(std::declval<Optional<int>>().map(std::declval<long(int)>())),
                         Optional<long>>);
    CHECK(std::is_same_v<decltype(std::declval<Optional<int>>().map(std::declval<Optional<long>(int)>())),
                         Optional<long>>);
    CHECK(std::is_same_v<decltype(std::declval<Optional<int>>().map(std::declval<std::optional<long>(int)>())),
                         Optional<long>>);

    SECTION("normal func")
    {
        auto lmd = [](int x) { return x + 1; };
        CHECK(Optional<int>(1).map(lmd) == 2);
        CHECK(Optional<int>().map(lmd) == std::nullopt);
        CHECK((Optional<int>(1) | lmd) == 2);
        CHECK((Optional<int>() | lmd) == std::nullopt);
    }
    SECTION("returns Optional func")
    {
        auto lmd = [](int x) { return Optional<int>(x + 1); };
        CHECK(Optional<int>(1).map(lmd) == 2);
        CHECK(Optional<int>().map(lmd) == std::nullopt);
        CHECK((Optional<int>(1) | lmd) == 2);
        CHECK((Optional<int>() | lmd) == std::nullopt);
    };
}

TEST_CASE("Optional - Range Iterator", "[container]")
{
    bool f = true;
    for ([[maybe_unused]] auto&& x : Optional<int>{}) {
        static_assert(std::is_same_v<decltype(x), int&>);
        f = false;
    }
    CHECK(f);
    f = false;
    for (auto&& x : Optional<int>{1}) {
        CHECK(x == 1);
        f = true;
    }
    CHECK(f);
}
