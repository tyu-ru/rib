#pragma once

#include <optional>
#include <type_traits>
#include <functional>

#include "../Traits/TypeTraits.hpp"
#include "../Traits/FuncTraits.hpp"

namespace rib
{

template <class T>
class OptionalIterator;

template <class T>
class Optional : public std::optional<T>
{
    static constexpr bool is_nested = trait::is_template_specialized_by_type_v<Optional, T>;

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
    template <class... Args, trait::concept_t<std::is_constructible_v<std::optional<T>, Args...>> = nullptr>
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

    template <class F, class... Args>
    constexpr const Optional<T>& visit(F&& f, Args&&... args) const&
    {
        if (this->has_value()) {
            trait::invoke_constexpr(std::forward<F>(f), **this, std::forward<Args>(args)...);
        }
        return *this;
    }
    template <class F, class... Args>
    constexpr Optional<T>& visit(F&& f, Args&&... args) &
    {
        if (this->has_value()) {
            trait::invoke_constexpr(std::forward<F>(f), **this, std::forward<Args>(args)...);
        }
        return *this;
    }
    template <class F, class... Args>
    constexpr Optional<T> visit(F&& f, Args&&... args) &&
    {
        if (this->has_value()) {
            trait::invoke_constexpr(std::forward<F>(f), **this, std::forward<Args>(args)...);
        }
        return std::move(*this);
    }

    template <class F, class... Args>
    constexpr auto map(F&& f, Args&&... args) const& -> Optional<deepest_t<std::invoke_result_t<F, T, Args...>>>
    {
        if (this->has_value()) {
            return trait::invoke_constexpr(std::forward<F>(f), **this, std::forward<Args>(args)...);
        }
        return std::nullopt;
    }
    template <class F, class... Args>
    constexpr auto map(F&& f, Args&&... args) & -> Optional<deepest_t<std::invoke_result_t<F, T, Args...>>>
    {
        if (this->has_value()) {
            return trait::invoke_constexpr(std::forward<F>(f), **this, std::forward<Args>(args)...);
        }
        return std::nullopt;
    }
    template <class F, class... Args>
    constexpr auto map(F&& f, Args&&... args) && -> Optional<deepest_t<std::invoke_result_t<F, T, Args...>>>
    {
        if (this->has_value()) {
            return trait::invoke_constexpr(std::forward<F>(f), std::move(**this), std::forward<Args>(args)...);
        }
        return std::nullopt;
    }

    template <class F>
    constexpr auto operator|(F&& f) const& -> Optional<deepest_t<std::invoke_result_t<F, T>>>
    {
        return map(std::forward<F>(f));
    }
    template <class F>
    constexpr auto operator|(F&& f) & -> Optional<deepest_t<std::invoke_result_t<F, T>>>
    {
        return map(std::forward<F>(f));
    }
    template <class F>
    constexpr auto operator|(F&& f) && -> Optional<deepest_t<std::invoke_result_t<F, T>>>
    {
        return map(std::forward<F>(f));
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
    using Optional_type = trait::copy_const_t<T, Optional<std::remove_const_t<T>>>;
    friend std::remove_const_t<Optional_type>;

    Optional_type* ptr = nullptr;

    constexpr OptionalIterator(Optional_type* p)
        : ptr(p && p->has_value() ? p : nullptr) {}

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

static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().unwrap()),
                             Optional<int>>);
static_assert(std::is_same_v<decltype(std::declval<Optional<Optional<int>>>().unwrap()),
                             Optional<int>>);

static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().visit(std::declval<long(int)>())),
                             Optional<int>>);

static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().map(std::declval<long(int)>())),
                             Optional<long>>);
static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().map(std::declval<Optional<long>(int)>())),
                             Optional<long>>);
static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().map(std::declval<std::optional<long>(int)>())),
                             Optional<long>>);

static_assert([] {
    auto lmd = [](int x) { return x + 1; };
    return Optional<int>(1).map(lmd) == 2 &&
           Optional<int>().map(lmd) == std::nullopt;
}());

static_assert(trait::is_result_v<Optional<long>, std::bit_or<>, Optional<int>, long(int)>);
static_assert(trait::is_result_v<Optional<long>, std::bit_or<>, Optional<int>, Optional<long>(int)>);
static_assert(trait::is_result_v<Optional<long>, std::bit_or<>, Optional<int>, std::optional<long>(int)>);

static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().begin()),
                             OptionalIterator<int>>);
static_assert(std::is_same_v<decltype(std::declval<const Optional<int>>().begin()),
                             OptionalIterator<const int>>);

static_assert(std::is_same_v<decltype(*std::declval<OptionalIterator<int>>()), int&>);
static_assert(std::is_same_v<decltype(*std::declval<OptionalIterator<const int>>()), const int&>);

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
