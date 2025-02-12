#ifndef FLUKY_XOSHIRO256SS_RANDOM_HPP
#define FLUKY_XOSHIRO256SS_RANDOM_HPP

#include <array>
#include <cstdint>
#include <limits>
#include "detail/splitmix64.hpp"

namespace fluky {
namespace xoshiro256ss_detail {
    using xoshiro256ss_result_type = uint64_t;
    using xoshiro256ss_state = std::array<uint64_t, 4>;

    static constexpr auto a =  5UL;
    static constexpr auto b =  7UL;
    static constexpr auto c =  9UL;
    static constexpr auto d = 17UL;
    static constexpr auto e = 45UL;

    static constexpr std::array short_jump = {
        0X180EC6D33CFD0ABAUL,
        0XD5A61266F0C9392CUL,
        0XA9582618E03FC9AAUL,
        0X39ABDC4529B1661CUL
    };
    static constexpr std::array long_jump {
        0X76E15D3EFEFDCBBFUL,
        0XC5004E441C522FB3UL,
        0X77710069854EE241UL,
        0X39109BB02ACBE635UL
    };
    static constexpr auto n = 64UL;

    inline auto init_state(xoshiro256ss_state& state, uint64_t seed) -> void {
        for (auto& s : state) { s = detail::splitmix64(seed); }
    }

    inline auto next(xoshiro256ss_state& state) -> xoshiro256ss_result_type {
        auto const r = std::rotl(state[1] * a, b) * c;
        auto const t = state[1] << d;

        state[2] ^= state[0];
        state[3] ^= state[1];
        state[1] ^= state[2];
        state[0] ^= state[3];

        state[2] ^= t;
        state[3] = std::rotl(state[3], e);
        return r;
    }

    inline auto jump(xoshiro256ss_state& state, std::array<uint64_t, 4> const& jump_table) -> void {
        auto s0 = 0UL;
        auto s1 = 0UL;
        auto s2 = 0UL;
        auto s3 = 0UL;

        for (auto u : jump_table) {
            for (auto v = 0UL; v < n; ++v) {
                if ((u & UINT64_C(1) << v) != 0U) {
                    s0 ^= state[0];
                    s1 ^= state[1];
                    s2 ^= state[2];
                    s3 ^= state[3];
                }
                next(state);
            }
        }
        state = {s0, s1, s2, s3};
    }
} // namespace xoshiro256ss_detail

class xoshiro256ss final {
    xoshiro256ss_detail::xoshiro256ss_state state_{};

public:
    using result_type = xoshiro256ss_detail::xoshiro256ss_result_type;
    static constexpr auto min() -> uint64_t { return result_type{0}; } // NOLINT
    static constexpr auto max() -> uint64_t { return std::numeric_limits<result_type>::max(); }

    explicit xoshiro256ss(uint64_t seed) {
        xoshiro256ss_detail::init_state(state_, seed);
    }
    ~xoshiro256ss() noexcept = default;

    xoshiro256ss(xoshiro256ss const&) = delete;
    auto operator=(xoshiro256ss const&) -> xoshiro256ss& = delete;

    xoshiro256ss(xoshiro256ss&&) = default;
    auto operator=(xoshiro256ss&&) noexcept -> xoshiro256ss& = default;

    auto operator()() {
        return xoshiro256ss_detail::next(state_);
    }

    auto jump() { xoshiro256ss_detail::jump(state_, xoshiro256ss_detail::short_jump); }
    auto long_jump() { xoshiro256ss_detail::jump(state_, xoshiro256ss_detail::long_jump); }
};
} // namespace fluky

#endif