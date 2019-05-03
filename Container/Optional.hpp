#pragma once

#include <optional>
#include <type_traits>
#include <functional>

#include "../Type/TypeTraits.hpp"
#include "../functional_util.hpp"

namespace rib
{

template <class T>
class OptionalIterator;

template <class T>
class Optional : public std::optional<T>
{
    static constexpr bool is_nested = type::is_template_specialized_by_type_v<Optional, T>;

    template <class U>
    struct deepest
    {
        using type = std::remove_reference_t<U>;
    };
    template <class U>
    struct deepest<Optional<U>>
    {
        using type = std::remove_reference_t<U>;
    };
    template <class U>
    struct deepest<std::optional<U>>
    {
        using type = std::remove_reference_t<U>;
    };

    template <class U>
    using deepest_t = typename deepest<U>::type;

public:
    template <class... Args, std::enable_if_t<std::is_constructible_v<std::optional<T>, Args...>, std::nullptr_t> = nullptr>
    constexpr Optional(Args&&... args) : std::optional<T>(std::forward<Args>(args)...)
    {
    }

    constexpr auto unwrap() const -> Optional<deepest_t<T>>
    {
        if constexpr (is_nested) {
            return *this;
        } else {
            return **this;
        }
    }

    template <class Func, class... Args>
    constexpr Optional<T>& visit(Func&& func, Args&&... args) const&
    {
        if (this->has_value()) {
            std::invoke(std::forward<Func>(func), **this, std::forward<Args>(args)...);
        }
        return *this;
    }
    template <class Func, class... Args>
    constexpr Optional<T>& visit(Func&& func, Args&&... args) &
    {
        if (this->has_value()) {
            std::invoke(std::forward<Func>(func), **this, std::forward<Args>(args)...);
        }
        return *this;
    }
    template <class Func, class... Args>
    constexpr Optional<T>& visit(Func&& func, Args&&... args) &&
    {
        if (this->has_value()) {
            std::invoke(std::forward<Func>(func), std::move(**this), std::forward<Args>(args)...);
        }
        return *this;
    }

    template <class Func, class... Args, class R = std::invoke_result_t<Func, T, Args...>>
    constexpr auto invoke(Func&& func, Args&&... args) const& -> Optional<deepest_t<R>>
    {
        if (this->has_value()) {
            return std::invoke(std::forward<Func>(func), **this, std::forward<Args>(args)...);
        }
        return std::nullopt;
    }
    template <class Func, class... Args, class R = std::invoke_result_t<Func, T, Args...>>
    constexpr auto invoke(Func&& func, Args&&... args) & -> Optional<deepest_t<R>>
    {
        if (this->has_value()) {
            return std::invoke(std::forward<Func>(func), **this, std::forward<Args>(args)...);
        }
        return std::nullopt;
    }
    template <class Func, class... Args, class R = std::invoke_result_t<Func, T, Args...>>
    constexpr auto invoke(Func&& func, Args&&... args) && -> Optional<deepest_t<R>>
    {
        if (this->has_value()) {
            return std::invoke(std::forward<Func>(func), std::move(**this), std::forward<Args>(args)...);
        }
        return std::nullopt;
    }

    template <class Func>
    constexpr auto operator|(Func&& func) const& -> Optional<deepest_t<std::invoke_result_t<Func, T>>>
    {
        return invoke(std::forward<Func>(func));
    }
    template <class Func>
    constexpr auto operator|(Func&& func) & -> Optional<deepest_t<std::invoke_result_t<Func, T>>>
    {
        return invoke(std::forward<Func>(func));
    }
    template <class Func>
    constexpr auto operator|(Func&& func) && -> Optional<deepest_t<std::invoke_result_t<Func, T>>>
    {
        return invoke(std::forward<Func>(func));
    }

    constexpr OptionalIterator<const T> begin() const { return this; }
    constexpr OptionalIterator<T> begin() { return this; }

    constexpr OptionalIterator<const T> end() const { return nullptr; }
    constexpr OptionalIterator<T> end() { return nullptr; }
};

template <class T>
class OptionalIterator
{
public:
    using difference_type = std::size_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;

private:
    using Optional_type = type::copy_const_t<T, Optional<std::remove_const_t<T>>>;
    friend std::remove_const_t<Optional_type>;

    Optional_type* ptr = nullptr;

    constexpr OptionalIterator(Optional_type* p)
    {
        if (p && p->has_value()) {
            ptr = p;
        } else {
            ptr = nullptr;
        }
    }

public:
    constexpr OptionalIterator() = default;
    constexpr OptionalIterator(const OptionalIterator&) = default;
    constexpr OptionalIterator(OptionalIterator&&) = default;

    constexpr T& operator*() const { return **ptr; }
    constexpr T& operator*() { return **ptr; }

    constexpr OptionalIterator& operator++()
    {
        ptr = nullptr;
        return *this;
    }
    constexpr OptionalIterator operator++(int)
    {
        auto t = *this;
        ptr = nullptr;
        return t;
    }

    constexpr friend bool operator!=(const OptionalIterator& lhs, const OptionalIterator& rhs)
    {
        return lhs.ptr != rhs.ptr;
    }
};

static_assert(std::is_default_constructible_v<Optional<int>>);
static_assert(std::is_constructible_v<Optional<int>, int>);
static_assert(std::is_constructible_v<Optional<int>, std::nullopt_t>);

static_assert(std::is_same_v<RESULT_T(Optional<int>, unwrap()),
                             Optional<int>>);
static_assert(std::is_same_v<RESULT_T(Optional<Optional<int>>, unwrap()),
                             Optional<int>>);

static_assert(std::is_same_v<RESULT_T(Optional<int>, invoke(DECLV(long(int)))),
                             Optional<long>>);
static_assert(std::is_same_v<RESULT_T(Optional<int>, invoke(DECLV(Optional<long>(int)))),
                             Optional<long>>);
static_assert(std::is_same_v<RESULT_T(Optional<int>, invoke(DECLV(std::optional<long>(int)))),
                             Optional<long>>);

static_assert(type::is_result_v<Optional<long>, std::bit_or<>, Optional<int>, long(int)>);
static_assert(type::is_result_v<Optional<long>, std::bit_or<>, Optional<int>, Optional<long>(int)>);
static_assert(type::is_result_v<Optional<long>, std::bit_or<>, Optional<int>, std::optional<long>(int)>);

static_assert(std::is_same_v<RESULT_T(Optional<int>, begin()),
                             OptionalIterator<int>>);
static_assert(std::is_same_v<RESULT_T(const Optional<int>, begin()),
                             OptionalIterator<const int>>);

static_assert(std::is_same_v<decltype(*DECLV(OptionalIterator<int>)), int&>);
static_assert(std::is_same_v<decltype(*DECLV(OptionalIterator<const int>)), const int&>);

static_assert([] {
    int a = 0;
    for (auto&& x : Optional<int>{1}) {
        a = x;
    }
    for ([[maybe_unused]] auto&& x : Optional<int>{}) {
        a = 2;
    }
    return a == 1;
}());

} // namespace rib
