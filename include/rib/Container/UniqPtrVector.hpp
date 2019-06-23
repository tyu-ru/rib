#pragma once

#include <vector>
#include <memory>

namespace rib
{

template <class T>
struct UniqPtrVector : std::vector<std::unique_ptr<T>>
{
    auto comparator() const noexcept
    {
        return [](auto& lhs, auto& rhs) { return *lhs < *rhs; };
    }
};

} // namespace rib
