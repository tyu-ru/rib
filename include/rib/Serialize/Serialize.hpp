#pragma once

#include <type_traits>
#include <iterator>
#include <cstring>
#include "../Traits/TypeTraits.hpp"

namespace rib
{

template <class T>
struct is_serializable : std::disjunction<std::is_arithmetic<T>, std::is_enum<T>>
{
};

template <class T>
inline constexpr bool is_serializable_v = is_serializable<T>::value;

template <class T>
struct is_bynary_iterator : std::bool_constant<std::is_pointer_v<T> && sizeof(typename std::iterator_traits<T>::value_type) == 1>
{
};

template <class OutputIterator, class T, trait::concept_t<is_serializable_v<T>> = nullptr>
inline OutputIterator serialize(OutputIterator dst, const T& src)
{
    if constexpr (std::is_pointer_v<OutputIterator>) {
        std::memcpy(dst, static_cast<const void*>(&src), sizeof(T));
        return dst + sizeof(T);
    } else {
        return std::copy(static_cast<const char*>(src),
                         static_cast<const char*>(src) + sizeof(T),
                         dst);
    }
}

template <class OutputIterator, class T, trait::concept_t<is_serializable_v<T>> = nullptr>
inline OutputIterator serialize(OutputIterator dst, const T* src, std::size_t n)
{
    if constexpr (std::is_pointer_v<OutputIterator>) {
        std::memcpy(dst, src, sizeof(T) * n);
        return dst + sizeof(T) * n;
    } else {
        return std::copy(static_cast<const char*>(src),
                         static_cast<const char*>(src) + sizeof(T) * n,
                         dst);
    }
}

template <class T, class InputIterator, trait::concept_t<is_serializable_v<T>> = nullptr>
inline InputIterator deserialize(T& dst, InputIterator src)
{
    if constexpr (std::is_pointer_v<InputIterator>) {
        std::memcpy(static_cast<void*>(&dst), src, sizeof(T));
        return src + sizeof(T);
    } else {
        return std::copy(src, std::next(src, sizeof(T)),
                         static_cast<char*>(dst));
    }
}

template <class T, class InputIterator, trait::concept_t<is_serializable_v<T>> = nullptr>
inline InputIterator deserialize(T* dst, std::size_t n, InputIterator src)
{
    if constexpr (std::is_pointer_v<InputIterator>) {
        std::memcpy(dst, src, sizeof(T) * n);
        return src + sizeof(T) * n;
    } else {
        return std::copy(src, std::next(src, sizeof(T) * n),
                         static_cast<char*>(dst));
    }
}

} // namespace rib
