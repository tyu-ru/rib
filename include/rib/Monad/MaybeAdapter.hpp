#pragma once

#include "../Mixin/CopyMoveTrait.hpp"

namespace rib
{
template <class T>
class MaybeAdapter : private mixin::NonCopyableNonMovable<MaybeAdapter<T>>
{
public:
    using value_type = std::remove_reference_t<decltype(*std::declval<T>())>;

private:
    class iterator
    {

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = typename MaybeAdapter<T>::value_type;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::forward_iterator_tag;

    public:
        iterator(pointer p_) : p(p_) {}

        reference operator*() { return *p; }
        iterator& operator++()
        {
            p = nullptr;
            return *this;
        }
        bool operator!=(const iterator& rhs) { return p != rhs.p; }

    private:
        pointer p;
    };

    std::conditional_t<!std::is_rvalue_reference_v<T>, T&, std::remove_reference_t<T>> v;

public:
    template <class U = T, std::enable_if_t<!std::is_rvalue_reference_v<U>, std::nullptr_t> = nullptr>
    constexpr MaybeAdapter(T& v_) : v(v_) {}
    template <class U = T, std::enable_if_t<std::is_rvalue_reference_v<U>, std::nullptr_t> = nullptr>
    constexpr MaybeAdapter(T&& v_) : v(std::move(v_)) {}

    iterator begin() { return v ? &*v : nullptr; }
    iterator end() { return nullptr; }
};
template <class T>
MaybeAdapter(T&)->MaybeAdapter<T&>;
template <class T>
MaybeAdapter(T &&)->MaybeAdapter<T&&>;

} // namespace rib
