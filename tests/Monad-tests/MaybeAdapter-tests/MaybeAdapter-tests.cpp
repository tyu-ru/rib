#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include <type_traits>
#include "rib/Monad/MaybeAdapter.hpp"

using namespace rib;

using types = std::tuple<int*,
                         std::unique_ptr<int>,
                         std::shared_ptr<int>,
                         std::optional<int>>;

template <class T>
struct maybe_test;
template <class T>
struct maybe_test<T*>
{
    static inline T val;
    using const_type = const T*;

    static T* gen() { return nullptr; }
    static const_type gen_const() { return nullptr; }
    static T* gen(T x)
    {
        val = x;
        return &val;
    }
    static const_type gen_const(T x)
    {
        val = x;
        return &val;
    }
};
template <class T>
struct maybe_test<std::optional<T>>
{
    using const_type = const std::optional<T>;

    static std::optional<T> gen() { return std::nullopt; }
    static const_type gen_const() { return std::nullopt; }
    static std::optional<T> gen(T x) { return x; }
    static const_type gen_const(T x) { return x; }
};
template <class T>
struct maybe_test<std::unique_ptr<T>>
{
    using const_type = std::unique_ptr<const T>;

    static std::unique_ptr<T> gen() { return nullptr; }
    static const_type gen_const() { return nullptr; }
    static std::unique_ptr<T> gen(T x) { return std::make_unique<T>(x); }
    static const_type gen_const(T x) { return std::make_unique<const T>(x); }
};
template <class T>
struct maybe_test<std::shared_ptr<T>>
{
    using const_type = std::shared_ptr<const T>;

    static std::shared_ptr<T> gen() { return nullptr; }
    static const_type gen_const() { return nullptr; }
    static std::shared_ptr<T> gen(T x) { return std::make_unique<T>(x); }
    static const_type gen_const(T x) { return std::make_unique<const T>(x); }
};
template <class T>
using maybe_const_t = typename maybe_test<T>::const_type;

TEMPLATE_LIST_TEST_CASE("copy and move prohibition", "[monad]", types)
{
    STATIC_REQUIRE(std::is_copy_constructible_v<MaybeAdapter<TestType>> == false);
    STATIC_REQUIRE(std::is_move_constructible_v<MaybeAdapter<TestType>> == false);
    STATIC_REQUIRE(std::is_copy_assignable_v<MaybeAdapter<TestType>> == false);
    STATIC_REQUIRE(std::is_move_assignable_v<MaybeAdapter<TestType>> == false);
}

TEMPLATE_LIST_TEST_CASE("for each lvalue", "[monad]", types)
{
    STATIC_REQUIRE(std::is_same_v<decltype(MaybeAdapter(std::declval<TestType&>())),
                                  MaybeAdapter<TestType&>> == true);
    STATIC_REQUIRE(std::is_same_v<typename decltype(MaybeAdapter(std::declval<TestType&>()))::value_type,
                                  int> == true);

    SECTION("null")
    {
        TestType null = maybe_test<TestType>::gen();
        for (auto&& x[[maybe_unused]] : MaybeAdapter(null)) {
            FAIL();
        }
    }
    SECTION("has value")
    {
        bool f = false;
        TestType value = maybe_test<TestType>::gen(42);
        for (auto&& x : MaybeAdapter(value)) {
            f = true;
            REQUIRE(x == 42);
            x = 33;
        }
        REQUIRE(f == true);
        REQUIRE(*value == 33);
    }
}

TEMPLATE_LIST_TEST_CASE("for each const lvalue", "[monad]", types)
{
    STATIC_REQUIRE(std::is_same_v<decltype(MaybeAdapter(std::declval<maybe_const_t<TestType>&>())),
                                  MaybeAdapter<maybe_const_t<TestType>&>> == true);
    STATIC_REQUIRE(std::is_same_v<typename decltype(MaybeAdapter(std::declval<maybe_const_t<TestType>&>()))::value_type,
                                  const int> == true);

    SECTION("null")
    {
        maybe_const_t<TestType> null = maybe_test<TestType>::gen_const();
        for (auto&& x[[maybe_unused]] : MaybeAdapter(null)) {
            FAIL();
        }
    }
    SECTION("has value")
    {
        bool f = false;
        maybe_const_t<TestType> value = maybe_test<TestType>::gen_const(42);
        for (auto&& x : MaybeAdapter(value)) {
            f = true;
            REQUIRE(x == 42);
            STATIC_REQUIRE(std::is_same_v<decltype(x), const int&>);
        }
        REQUIRE(f == true);
    }
}

TEMPLATE_LIST_TEST_CASE("for each rvalue", "[monad]", types)
{
    STATIC_REQUIRE(std::is_same_v<decltype(MaybeAdapter(std::declval<TestType>())),
                                  MaybeAdapter<TestType&&>> == true);
    STATIC_REQUIRE(std::is_same_v<typename decltype(MaybeAdapter(std::declval<TestType>()))::value_type,
                                  int> == true);

    for (auto&& x[[maybe_unused]] : MaybeAdapter(maybe_test<TestType>::gen())) {
        FAIL();
    }

    bool f = false;
    for (auto&& x : MaybeAdapter(maybe_test<TestType>::gen(42))) {
        f = true;
        REQUIRE(x == 42);
    }
    REQUIRE(f == true);
}
