#pragma once

#include <optional>
#include <type_traits>
#include <functional>

#include "Type/TypeTraits.hpp"

namespace rib
{

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
};

static_assert(std::is_default_constructible_v<Optional<int>>);
static_assert(std::is_constructible_v<Optional<int>, int>);
static_assert(std::is_constructible_v<Optional<int>, std::nullopt_t>);

static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().unwrap()), Optional<int>>);
static_assert(std::is_same_v<decltype(std::declval<Optional<Optional<int>>>().unwrap()), Optional<int>>);

static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().invoke(std::declval<long(int)>())),
                             Optional<long>>);
static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().invoke(std::declval<Optional<long>(int)>())),
                             Optional<long>>);
static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().invoke(std::declval<std::optional<long>(int)>())),
                             Optional<long>>);

static_assert(std::is_same_v<decltype(std::declval<Optional<int>>() | std::declval<long(int)>()),
                             Optional<long>>);
static_assert(std::is_same_v<decltype(std::declval<Optional<int>>() | std::declval<Optional<long>(int)>()),
                             Optional<long>>);
static_assert(std::is_same_v<decltype(std::declval<Optional<int>>() | std::declval<std::optional<long>(int)>()),
                             Optional<long>>);

} // namespace rib
