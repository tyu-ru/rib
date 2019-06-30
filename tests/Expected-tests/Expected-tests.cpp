#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "rib/Container/Expected.hpp"

#include "test_util.hpp"

using namespace rib;

using TestTypeList = make_type_permutation_t<std::pair, 2, true, int, std::string, std::tuple<int, std::string>>;

TEMPLATE_LIST_TEST_CASE("Expected constructivle trait", "[Container]", TestTypeList)
{
    using Ok = typename TestType::first_type;
    using Err = typename TestType::second_type;
    using Exp = Expected<Ok, Err>;

    CHECK_FALSE(std::is_default_constructible_v<Exp>);
    CHECK(std::is_copy_constructible_v<Exp>);
    CHECK(std::is_move_constructible_v<Exp>);

    CHECK(std::is_constructible_v<Exp, Ok>);
    CHECK(std::is_constructible_v<Exp, Ok&&>);
    CHECK(std::is_constructible_v<Exp, Expect<Ok>>);
    CHECK(std::is_constructible_v<Exp, Expect<Ok>&&>);
    CHECK(std::is_constructible_v<Exp, Unexpect<Err>>);
    CHECK(std::is_constructible_v<Exp, Unexpect<Err>&&>);
    CHECK(std::is_constructible_v<Exp, ExpectTag, Ok>);
    CHECK(std::is_constructible_v<Exp, UnexpectTag, Err>);

    CHECK(std::is_copy_assignable_v<Exp>);
    CHECK(std::is_move_assignable_v<Exp>);

    CHECK(std::is_assignable_v<Exp, Ok>);
    CHECK(std::is_assignable_v<Exp, Unexpect<Err>>);
}

TEST_CASE("Expected copy-move trait", "[Container]")
{
    SECTION("non-copy non-move")
    {
        using A1 = Expected<int, mixin::NonCopyableNonMovable<void>>;
        using A2 = Expected<mixin::NonCopyableNonMovable<void>, int>;
        CHECK_FALSE(std::is_copy_assignable_v<A1>);
        CHECK_FALSE(std::is_move_assignable_v<A1>);
        CHECK_FALSE(std::is_copy_assignable_v<A2>);
        CHECK_FALSE(std::is_move_assignable_v<A2>);
    }

    SECTION("non-copy movable")
    {
        using B1 = Expected<int, mixin::NonCopyable<void>>;
        using B2 = Expected<mixin::NonCopyable<void>, int>;
        CHECK_FALSE(std::is_copy_assignable_v<B1>);
        CHECK(std::is_move_assignable_v<B1>);
        CHECK_FALSE(std::is_copy_assignable_v<B2>);
        CHECK(std::is_move_assignable_v<B2>);
    }
}

TEST_CASE("Expected construction & access", "[Container]")
{
    SECTION("Normal")
    {
        Expected<int, int> e = 1;
        REQUIRE(e);
        REQUIRE_FALSE(!e);
        REQUIRE(e.valid());

        CHECK(e.value() == 1);
        CHECK_NOTHROW(e.value());
        CHECK(e.value_noexcept() == 1);
        CHECK(*e == 1);

        CHECK_THROWS_AS(e.error(), BadExpectedAccess);

        CHECK(e.value_or(0) == 1);
        CHECK(e.value_or_default() == 1);
    }
    SECTION("Explicit expect")
    {
        Expected<int, int> e = Expect(1);
        REQUIRE(e);
        CHECK(e.value() == 1);
    }
    SECTION("Expect-Tag")
    {
        Expected<int, int> e(expect_tag_v, 1);
        REQUIRE(e);
        CHECK(e.value() == 1);
    }
    SECTION("Unexpect")
    {
        Expected<int, int> e = Unexpect(1);
        REQUIRE_FALSE(e);
        REQUIRE(!e);
        REQUIRE_FALSE(e.valid());

        CHECK(e.error() == 1);
        CHECK_NOTHROW(e.error());
        CHECK(e.error_noexcept() == 1);

        CHECK_THROWS_AS(e.value(), BadExpectedAccess);

        CHECK(e.value_or(0) == 0);
        CHECK(e.value_or_default() == 0);
    }
    SECTION("Unexpect-tag")
    {
        Expected<int, int> e(unexpect_tag_v, 1);
        REQUIRE_FALSE(e);
        CHECK(e.error() == 1);
    }
    SECTION("Structure")
    {
        struct A
        {
            int a;
        };
        Expected<A, long> e = A{1};
        CHECK(e->a == 1);
    }
}

TEST_CASE("Expected assign", "[container]")
{
    Expected<int, int> e = Unexpect(1);

    e = 1;
    REQUIRE(e.valid());
    CHECK(e == 1);

    e = Unexpect(2);
    REQUIRE(!e);
    CHECK(e.error() == 2);
}

TEST_CASE("Expected to optional", "[container]")
{
    Expected<int, int> e = 1;
    CHECK(std::is_same_v<decltype(e.optional()), std::optional<int>>);
    CHECK(e.optional() == 1);
    e = Unexpect(1);
    CHECK(e.optional() == std::nullopt);
}

TEST_CASE("Expected to Unexpect", "[container]")
{
    Expected<int, int> e1 = 1;
    Expected<int, int> e2 = Unexpect(1);

    CHECK_THROWS_AS(e1.unexpected(), BadExpectedAccess);
    CHECK(e2.unexpected().value() == 1);
}

// CHECK(std::is_same_v<Expected<int, int>::Unwrap_t, Expected<int, int>>);
// CHECK(std::is_same_v<Expected<Expected<int, int>, int>::Unwrap_t, Expected<int, int>>);

TEST_CASE("Expected equal compare", "[container]")
{
    Expected<int, int> e1 = 1, e2 = 2;
    Expected<int, int> u1 = Unexpect(1), u2 = Unexpect(2);

    SECTION("Expected fellow")
    {
        CHECK(e1 == e1);
        CHECK(e1 != e2);
        CHECK(e1 != u1);
        CHECK(u1 == u1);
        CHECK(u1 != u2);
    }
    SECTION("Expected vs Unexpect")
    {
        CHECK(e1 != Unexpect(1));
        CHECK(u1 == Unexpect(1));
        CHECK(u2 != Unexpect(1));

        CHECK(Unexpect<int>(1) != e1);
        CHECK(Unexpect<int>(1) == u1);
        CHECK(Unexpect<int>(1) != u2);
    }
    SECTION("Expected vs normal value")
    {
        CHECK(e1 == 1);
        CHECK(e2 != 1);
        CHECK(u1 != 1);

        CHECK(1 == e1);
        CHECK(1 != e2);
        CHECK(1 != u1);
    }
}

TEST_CASE("Expect monad - map", "[container]")
{
    Expected<int, int> e1 = 1, e2 = Unexpect(1);
    auto lmd = [](int x) { return std::to_string(x); };

    SECTION("map")
    {
        REQUIRE(std::is_same_v<decltype(e1.map(lmd)), Expected<std::string, int>>);
        CHECK(e1.map(lmd) == "1");
        CHECK(e2.map(lmd) == Unexpect(1));
    }
    SECTION("emap")
    {
        REQUIRE(std::is_same_v<decltype(e1.emap(lmd)), Expected<int, std::string>>);
        CHECK(e1.emap(lmd) == 1);
        CHECK(e2.emap(lmd).error() == "1");
    }
}

TEST_CASE("Expect monad - and_then-or_else", "[container]")
{
    Expected<int, int> e1 = 1, e2 = 2, e3 = Unexpect(1), e4 = Unexpect(2);
    auto lmd1 = [](int x) -> Expected<std::string, int> {
        if (x == 1) return std::to_string(x + 1);
        return Unexpect(2);
    };
    auto lmd2 = [](int x) -> Expected<int, std::string> {
        if (x == 1) return x + 1;
        return Unexpect(std::to_string(x + 1));
    };

    SECTION("and_then")
    {
        REQUIRE(std::is_same_v<decltype(e1.and_then(lmd1)), Expected<std::string, int>>);
        CHECK(e1.and_then(lmd1) == "2");
        CHECK(e2.and_then(lmd1) == Unexpect(2));
        CHECK(e3.and_then(lmd1) == Unexpect(1));
        CHECK(e4.and_then(lmd1) == Unexpect(2));
    }
    SECTION("or_else")
    {
        REQUIRE(std::is_same_v<decltype(e1.or_else(lmd2)), Expected<int, std::string>>);
        CHECK(e1.or_else(lmd2) == 1);
        CHECK(e2.or_else(lmd2) == 2);
        CHECK(e3.or_else(lmd2) == 2);
        CHECK(e4.or_else(lmd2) == Unexpect("3"));
    }
}

TEST_CASE("Expect monad - then", "[container]")
{
    // static_assert(std::is_same_v<decltype(std::declval<Expected<int, long>>().then(std::declval<Expected<double, float>(Expected<int, long>)>())),
    //                              Expected<double, float>>);
    // static_assert(std::is_same_v<decltype(std::declval<Expected<int, long>>().then(std::declval<double(Expected<int, long>)>())),
    //                              Expected<double, long>>);
    // static_assert([] {
    //     auto lmd1 = [](Expected<int, int>) { return 'a'; };
    //     auto lmd2 = [](Expected<int, int>) { return Expected<long, int>(2); };
    //     return Expected<int, int>(1).then(lmd1) == 'a' &&
    //            Expected<int, int>(Unexpect(1)).then(lmd1) == 'a' &&
    //            Expected<int, int>(1).then(lmd2) == 2 &&
    //            Expected<int, int>(Unexpect(1)).then(lmd2) == 2;
    // }());
}

TEST_CASE("Expect monad - catch_error", "[container]")
{
    Expected<int, long> e1 = 1, e2 = Unexpect(1);
    auto lmd = [](int x) { return x + 1; };

    REQUIRE(std::is_same_v<decltype(e1.catch_error(lmd)), Expected<int, long>>);

    CHECK(e1.catch_error(lmd) == 1);
    CHECK(e2.catch_error(lmd) == 2);
}

TEST_CASE("Expect monad - mach", "[container]")
{
    Expected<int, std::string> e1 = 1, e2 = Unexpect("2");
    auto lmd1 = [](int x) { return x + 1; };
    auto lmd2 = [](std::string x) { return std::stoi(x) * 2; };

    REQUIRE(std::is_same_v<decltype(e1.mach<void>(lmd1, lmd2)), void>);
    REQUIRE(std::is_same_v<decltype(e1.mach<int>(lmd1, lmd2)), int>);
    REQUIRE(std::is_same_v<decltype(e1.mach<long>(lmd1, lmd2)), long>);

    CHECK(e1.mach<int>(lmd1, lmd2) == 2);
    CHECK(e2.mach<int>(lmd1, lmd2) == 4);
}
