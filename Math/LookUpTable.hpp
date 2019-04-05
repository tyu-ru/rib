#pragma once

#include <algorithm>
#include <array>
#include "Angle.hpp"
#include "Taylor.hpp"
#include "Interpolation.hpp"

namespace rib::math
{

template <std::size_t n>
struct Sin
{
    static constexpr std::array<double, 91> table = [] {
        std::array<double, 91> result{};
        for (std::size_t i = 0; i < result.size(); ++i) {
            result[i] = maclaurin::sin(deg2rad<double>(i), n);
        }
        return result;
    }();

    constexpr double operator()(double x) const
    {
        x = rib::math::rad2deg(x);
        x = x - 360 * static_cast<int>(x / 360);
        if (x < 0) x += 360;
        int s = x < 180 ? 1 : -1;
        if (180 < x) x -= 180;
        if (90 < x) x = 180 - x;
        auto i = std::clamp(static_cast<int>(x), 0, 90 - 1);
        return lerp(table[i], table[i + 1], x - i) * s;
    }
};

} // namespace rib::math
