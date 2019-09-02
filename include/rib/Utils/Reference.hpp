#pragma once

#include <type_traits>

namespace rib
{

/**
 * @brief explicit call by reference helper
 * @tparam T
 * @par example
 * @code
 * void f(Ref<int> r) {
 *     r = 42;
 * }
 * int a;
 * // f(a); // compile error!
 * f(Ref(a)); // explicit call by reference
 * @endcode
 */
template <class T>
struct Ref
{
    static_assert(!std::is_reference_v<T>);

    /// explicit constructor
    constexpr explicit Ref(T& x) : ref(x) {}

    // const T &operator=(const T &) const = delete;
    /// assign
    constexpr T& operator=(const T& x) { return ref = x; }
    /// assign
    constexpr Ref& operator=(const Ref& x) { return ref = x; }

    /// arrow
    constexpr T* operator->() { return &ref; }

    /// type conversion
    constexpr operator const T&() const { return ref; }
    /// type conversion
    constexpr operator T&() { return ref; }

    /// explicit accessor
    constexpr const T& var() const { return ref; }
    /// explicit accessor
    constexpr T& var() { return ref; }

private:
    T& ref;
};

static_assert(std::is_invocable_v<void(Ref<int>), Ref<int>>);
static_assert(std::is_invocable_v<void(Ref<int>), Ref<int>&>);
static_assert(std::is_invocable_v<void(Ref<int>), Ref<int>&&>);
static_assert(!std::is_invocable_v<void(Ref<int>), int>);
static_assert(!std::is_invocable_v<void(Ref<int>), int&>);
static_assert(!std::is_invocable_v<void(Ref<int>), int&&>);

static_assert(std::is_assignable_v<Ref<int>, int>);
static_assert(std::is_assignable_v<int&, Ref<int>>);
static_assert(std::is_assignable_v<Ref<int>, Ref<int>>);

} // namespace rib
