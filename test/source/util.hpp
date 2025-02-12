#ifndef FLUKY_TEST_UTIL_HPP
#define FLUKY_TEST_UTIL_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>

namespace fluky::test {
template<typename RNG>
inline auto write_bytes(RNG& rng) {
    constexpr auto buffer_size = 1UL << 20UL;
    static std::array<uint64_t, buffer_size> buffer;

    while(true) {
        std::generate(buffer.begin(), buffer.end(), [&](){ return rng(); });
        (void)fwrite(static_cast<void*>(buffer.data()), sizeof(decltype(buffer)::value_type), buffer_size, stdout);
    }
}
} // namespace fluky::test

#endif