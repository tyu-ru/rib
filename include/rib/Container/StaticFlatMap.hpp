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

    std::size_t size() const noexcept { return size_; }

    iterator begin() { return {this, 0}; }
    iterator end() { return {this, size_}; }

    std::pair<iterator, bool> insert(const value_type& x)
    {
        if (size_ == n) {
            return {end(), false};
        }

        auto range = std::equal_range(makeReinterpretIterator<&Data::v, &value_type::first>(data),
                                      makeReinterpretIterator<&Data::v, &value_type::first>(data + size_),
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

    bool contain(const Key& key) const
    {
        return std::binary_search(makeReinterpretIterator<&Data::v, &value_type::first>(data),
                                  makeReinterpretIterator<&Data::v, &value_type::first>(data + size_),
                                  key);
    }

    Value at(const Key& key) const
    {
        auto it = std::lower_bound(makeReinterpretIterator<&Data::v, &value_type::first>(data),
                                   makeReinterpretIterator<&Data::v, &value_type::first>(data + size_), key);
        if (it == data + size_) {
            return {};
        }
        return data[std::distance(data, it.iterator())].v.second;
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

    iterator(Parent* parent_, std::size_t i_) : parent(parent_), i(i_) {}

public:
    iterator() = default;

    iterator operator+(std::ptrdiff_t d) const { return {parent, i + d}; }
    iterator operator-(std::ptrdiff_t d) const { return {parent, i - d}; }
    iterator& operator+=(std::ptrdiff_t d) { return i += d, *this; }
    iterator& operator-=(std::ptrdiff_t d) { return i -= d, *this; }

    std::ptrdiff_t operator-(const iterator& rhs) const { return i - rhs.i; }

    bool operator==(const iterator& rhs) const { return parent == rhs.parent && i == rhs.i; }
    bool operator!=(const iterator& rhs) const { return !(*this == rhs); }

    iterator& operator++() { return ++i, *this; }
    iterator& operator--() { return --i, *this; }
    iterator operator++(int)
    {
        auto tmp = *this;
        ++i;
        return tmp;
    }
    iterator operator--(int)
    {
        auto tmp = *this;
        --i;
        return tmp;
    }

    value_type operator*() const { return parent->data[i].v; }
    pointer operator->() const { return &(parent->data[i].v); }
};

template <class Key, class Value, std::size_t n>
#if __cplusplus > 201703L
[[deprecated]]
#endif
    class ConstexprMap
{
    // in c++20, pair array etc. become constexpr
    Key key[n] = {};
    Value value[n] = {};

public:
    constexpr ConstexprMap(std::array<std::pair<Key, Value>, n> dat)
    {
        // constexpr sort. selection sort
        for (std::size_t i = 0; i < dat.size(); ++i) {
            std::size_t min = i;
            for (std::size_t j = i + 1; j < dat.size(); ++j) {
                if (dat[j].first < dat[min].first) {
                    min = j;
                }
            }
            key[i] = dat[min].first;
            value[i] = dat[min].second;
        }
    }
    template <class... Args, trait::concept_t<std::is_same_v<std::pair<Key, Value>, Args>...> = nullptr>
    constexpr ConstexprMap(Args... dat)
        : ConstexprMap(std::array<std::pair<Key, Value>, n>{dat...})
    {
    }
};
template <class Key, class Value, class... Args,
          trait::concept_t<std::is_same_v<std::pair<Key, Value>, Args>...> = nullptr>
ConstexprMap(std::pair<Key, Value>, Args...)->ConstexprMap<Key, Value, sizeof...(Args) + 1>;

template <class Key, class Value, class... Args>
constexpr ConstexprMap<Key, Value, sizeof...(Args) + 1> make_ConstexprMap(std::pair<Key, Value> top, Args... args)
{
    std::array<std::pair<Key, Value>, sizeof...(Args) + 1> tmp{top, args...};
    return tmp;
}

} // namespace rib
