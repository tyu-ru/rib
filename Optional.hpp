#pragma once

#include <optional>
#include <type_traits>

#include "Type/TypeTraits.hpp"

namespace rib
{

template <class T>
class Optional : public std::optional<T>
{
    template <class>
    struct is_std_optional : std::false_type
    {
    };
    template <class U>
    struct is_std_optional<std::optional<U>> : std::true_type
    {
    };
    template <class>
    struct is_optional : std::false_type
    {
    };
    template <class U>
    struct is_optional<Optional<U>> : std::true_type
    {
    };

public:
    template <class... Args, std::enable_if_t<std::is_constructible_v<std::optional<T>, Args...>, std::nullptr_t> = nullptr>
    constexpr Optional(Args&&... args) : std::optional<T>(std::forward<Args>(args)...) {}

    template <class Func, class Result = std::invoke_result_t<Func, T>,
              std::enable_if_t<!is_optional<Result>::value && !is_std_optional<Result>::value, nullptr_t> = nullptr>
    constexpr auto visit(Func&& func) -> Optional<Result>
    {
        if (this->has_value()) {
            return func(**this);
        }
        return std::nullopt;
    }
    template <class Func, class Result = std::invoke_result_t<Func, T>,
              std::enable_if_t<is_optional<Result>::value && !is_std_optional<Result>::value, nullptr_t> = nullptr>
    constexpr auto visit(Func&& func) -> Result
    {
        if (this->has_value()) {
            return func(**this);
        }
        return std::nullopt;
    }
    template <class Func, class Result = std::invoke_result_t<Func, T>,
              std::enable_if_t<!is_optional<Result>::value && is_std_optional<Result>::value, nullptr_t> = nullptr>
    constexpr auto visit(Func&& func) -> Optional<typename Result::value_type>
    {
        if (this->has_value()) {
            return func(**this);
        }
        return std::nullopt;
    }
};

static_assert(std::is_default_constructible_v<Optional<int>>);
static_assert(std::is_constructible_v<Optional<int>, int>);
static_assert(std::is_constructible_v<Optional<int>, std::nullopt_t>);

static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().visit(std::declval<long(int)>())),
                             Optional<long>>);
static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().visit(std::declval<Optional<long>(int)>())),
                             Optional<long>>);
static_assert(std::is_same_v<decltype(std::declval<Optional<int>>().visit(std::declval<std::optional<long>(int)>())),
                             Optional<long>>);

} // namespace rib
