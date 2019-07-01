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
            return **this;
        } else {
            return *this;
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

} // namespace rib
