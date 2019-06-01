#pragma once

#include <optional>
#include <type_traits>
#include <functional>

#include "../Traits/TypeTraits.hpp"
#include "../Traits/FuncTraits.hpp"
#include "../Mixin/NullableAccepter.hpp"
#include "../Iterator/MaybeIterator.hpp"

namespace rib
{

template <class T>
class Optional
    : public mixin::NullableAccepter<Optional<T>>,
      public MaybeIteratible<Optional<T>, T>,
      public std::optional<T>
{
    static constexpr bool is_nested = trait::is_template_specialized_by_type_v<Optional, T>;

    template <class U>
    struct unwrap_nest
    {
        using type = std::remove_reference_t<U>;
    };
    template <class U>
    struct unwrap_nest<Optional<U>>
    {
        using type = std::remove_reference_t<U>;
    };
    template <class U>
    struct unwrap_nest<std::optional<U>>
    {
        using type = std::remove_reference_t<U>;
    };

    template <class U>
    using unwrap_nest_t = typename unwrap_nest<U>::type;

public:
    template <class... Args, trait::concept_t<std::is_constructible_v<std::optional<T>, Args...>> = nullptr>
    constexpr Optional(Args&&... args) : std::optional<T>(std::forward<Args>(args)...)
    {
    }

    constexpr auto unwrap() const -> Optional<unwrap_nest_t<T>>
    {
        if constexpr (is_nested) {
            return *this;
        } else {
            return **this;
        }
    }

    template <class F, class... Args>
    constexpr auto map(F&& f, Args&&... args) const& -> Optional<unwrap_nest_t<std::invoke_result_t<F, T, Args...>>>
    {
        if (this->has_value()) {
            return trait::invoke_constexpr(std::forward<F>(f), **this, std::forward<Args>(args)...);
        }
        return std::nullopt;
    }
    template <class F, class... Args>
    constexpr auto map(F&& f, Args&&... args) & -> Optional<unwrap_nest_t<std::invoke_result_t<F, T, Args...>>>
    {
        if (this->has_value()) {
            return trait::invoke_constexpr(std::forward<F>(f), **this, std::forward<Args>(args)...);
        }
        return std::nullopt;
    }
    template <class F, class... Args>
    constexpr auto map(F&& f, Args&&... args) && -> Optional<unwrap_nest_t<std::invoke_result_t<F, T, Args...>>>
    {
        if (this->has_value()) {
            return trait::invoke_constexpr(std::forward<F>(f), std::move(**this), std::forward<Args>(args)...);
        }
        return std::nullopt;
    }

    template <class F>
    constexpr auto operator|(F&& f) const& -> Optional<unwrap_nest_t<std::invoke_result_t<F, T>>>
    {
        return map(std::forward<F>(f));
    }
    template <class F>
    constexpr auto operator|(F&& f) & -> Optional<unwrap_nest_t<std::invoke_result_t<F, T>>>
    {
        return map(std::forward<F>(f));
    }
    template <class F>
    constexpr auto operator|(F&& f) && -> Optional<unwrap_nest_t<std::invoke_result_t<F, T>>>
    {
        return map(std::forward<F>(f));
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
static_assert([] {
    auto lmd = [](int x) { return Optional<int>(x + 1); };
    return Optional<int>(1).map(lmd) == 2 &&
           Optional<int>().map(lmd) == std::nullopt;
}());

static_assert(trait::is_result_v<Optional<long>, std::bit_or<>, Optional<int>, long(int)>);
static_assert(trait::is_result_v<Optional<long>, std::bit_or<>, Optional<int>, Optional<long>(int)>);
static_assert(trait::is_result_v<Optional<long>, std::bit_or<>, Optional<int>, std::optional<long>(int)>);

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
