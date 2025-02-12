#ifndef FLUKY_DETAIL_SPLITMIX64_HPP
#define FLUKY_DETAIL_SPLITMIX64_HPP

#include <cstdint>

namespace fluky::detail {
static constexpr auto splitmix64(uint64_t& state) noexcept -> uint64_t
{
    uint64_t z = (state += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30U)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27U)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31U);
}
} // namespace fluky::detail

#endif