#pragma once

#include <string_view>

namespace rib
{

template <class F>
constexpr void splitString(std::string_view str, std::string_view del, F&& func, bool skipEmpty = true)
{
    if (str.empty() || del.empty()) return;

    auto pos = std::string_view::npos;
    while ((pos = str.find_first_of(del)) != std::string_view::npos) {
        if (!(skipEmpty && pos == 0)) {
            func(str.substr(0, pos));
        }
        str.remove_prefix(pos + 1);
    }
    func(str);
}

} // namespace rib
