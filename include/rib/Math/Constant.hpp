#pragma once

/// math contsnts
namespace rib::math::constant
{

/**
 * @breaf pi
 * @details hex fp literal. quad presicion(113bit mantissa)
 * @sa https://www.wolframalpha.com/input/?i=BaseForm%5BN%5BPI,+50%5D,+16%5D
 */
template <class T>
constexpr T pi = 0x3.243f'6a88'85a3'08d3'1319'8a2e'0370'7344'a409'3822p0L;

/**
 * @brief naipiers constant
 * @details  hex fp literal. quad presicion(113bit mantissa)
 * @sa https://www.wolframalpha.com/input/?i=BaseForm%5BN%5BE,+50%5D,+16%5D
 */
template <class T>
constexpr T e = 0x2.b7e1'5162'8aed'2a6a'bf71'5880'9cf4'f3c7'62e7'160fp0L;

} // namespace rib::math::constant
