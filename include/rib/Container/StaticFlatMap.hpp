#pragma once

#include <type_traits>
#include <algorithm>
#include <array>
#include <optional>

#include "../Iterator/ReinterpretIterator.hpp"

namespace rib
{

template <class Key, class Value, std::size_t n>
class StaticFlatMap
{
public:
    using value_type = std::pair<const Key, Value>;
    class iterator;
    friend iterator;

private:
    std::size_t size_ = 0;
    union Data
    {
        char dummy_;
        std::pair<const Key, Value> v;
        Data() {}
        ~Data() {}
    } data[n];

public:
    StaticFlatMap() = default;
    ~StaticFlatMap()
    {
        for (std::size_t i = 0; i < size_; ++i) {
            data[i].v.~value_type();
        }
    }

    constexpr std::size_t size() const noexcept { return size_; }

    constexpr iterator begin() { return {this, 0}; }
    constexpr iterator end() { return {this, size_}; }

    constexpr std::pair<iterator, bool> insert(const value_type& x)
    {
        if (size_ == n) {
            return {end(), false};
        }

        auto range = std::equal_range(makeReinterpretIterator<&value_type::first>(makeReinterpretIterator<&Data::v>(data)),
                                      makeReinterpretIterator<&value_type::first>(makeReinterpretIterator<&Data::v>(data + size_)),
                                      x.first);
        auto index = static_cast<std::size_t>(range.first.iterator() - data);

        if (range.first != range.second) return {{this, index}, false};

        for (std::size_t i = size_; index < i; --i) {
            new (&(data[i].v)) value_type(std::move(data[i - 1].v));
            data[i - 1].v.~value_type();
        }
        new (&(data[index].v)) value_type(x);

        ++size_;
        return {{this, index}, true};
    }

    constexpr bool contain(const Key& key) const
    {
        return std::binary_search(
            makeReinterpretIterator<&value_type::first>(makeReinterpretIterator<&Data::v>(data)),
            makeReinterpretIterator<&value_type::first>(makeReinterpretIterator<&Data::v>(data + size_)),
            key);
    }

    constexpr Value at(const Key& key) const
    {
        auto it = std::lower_bound(
            makeReinterpretIterator<&value_type::first>(makeReinterpretIterator<&Data::v>(data)),
            makeReinterpretIterator<&value_type::first>(makeReinterpretIterator<&Data::v>(data + size_)),
            key);
        if (it == data + size_) {
            return {};
        }
        return data[std::distance(data, it.iterator().iterator())].v.second;
    }
};

template <class Key, class Value, std::size_t n>
class StaticFlatMap<Key, Value, n>::iterator
{
public:
    using Parent = StaticFlatMap<Key, Value, n>;
    using difference_type = std::ptrdiff_t;
    using value_type = typename Parent::value_type;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::random_access_iterator_tag;

private:
    friend Parent;

    Parent* parent = nullptr;
    std::size_t i = 0;

    constexpr iterator(Parent* parent_, std::size_t i_) : parent(parent_), i(i_) {}

public:
    constexpr iterator() = default;

    constexpr iterator operator+(std::ptrdiff_t d) const { return {parent, i + d}; }
    constexpr iterator operator-(std::ptrdiff_t d) const { return {parent, i - d}; }
    constexpr iterator& operator+=(std::ptrdiff_t d) { return i += d, *this; }
    constexpr iterator& operator-=(std::ptrdiff_t d) { return i -= d, *this; }

    constexpr std::ptrdiff_t operator-(const iterator& rhs) const { return i - rhs.i; }

    constexpr bool operator==(const iterator& rhs) const { return parent == rhs.parent && i == rhs.i; }
    constexpr bool operator!=(const iterator& rhs) const { return !(*this == rhs); }

    constexpr iterator& operator++() { return ++i, *this; }
    constexpr iterator& operator--() { return --i, *this; }
    constexpr iterator operator++(int)
    {
        auto tmp = *this;
        ++i;
        return tmp;
    }
    constexpr iterator operator--(int)
    {
        auto tmp = *this;
        --i;
        return tmp;
    }

    constexpr value_type operator*() const { return parent->data[i].v; }
    constexpr pointer operator->() const { return &(parent->data[i].v); }
};

} // namespace rib
