#ifndef FLUKY_PCG64_DXSM_RANDOM_HPP
#define FLUKY_PCG64_DXSM_RANDOM_HPP

#include <cstdint>
#include <limits>
#include "detail/common.hpp"

#ifdef __SIZEOF_INT128__

namespace fluky {
namespace pcg64_dxsm_detail {
    using pcg64_dxsm_result_type = uint64_t;

    // constants
    static constexpr auto a = 64UL;
    static constexpr auto b = 32UL;
    static constexpr auto c = 48UL;

    struct pcg64_dxsm_state {
        uint128_t s;
        uint128_t i;
    };

    inline auto next(pcg64_dxsm_state& state) -> pcg64_dxsm_result_type {
        constexpr auto mul = 15750249268501108917ULL;
        auto const s = state.s;
        state.s = s * mul + state.i;
        auto hi = static_cast<uint64_t>(s >> a);
        auto lo = static_cast<uint64_t>(s | 1U);
        hi ^= hi >> b;
        hi *= mul;
        hi ^= hi >> c;
        hi *= lo;
        return hi;
    }

    inline auto init_state(pcg64_dxsm_state& state, uint64_t seed) -> void {
        state.s = seed;
        state.i = seed;
        state.i = (state.i << 1U) | 1U;
        state.s += state.i;
        next(state);
    }
} // namespace pcg64_dxsm_detail

class pcg64_dxsm final {
    pcg64_dxsm_detail::pcg64_dxsm_state state_{};

public:
    using result_type = pcg64_dxsm_detail::pcg64_dxsm_result_type;
    static constexpr auto min() -> uint64_t { return result_type{0}; } // NOLINT
    static constexpr auto max() -> uint64_t { return std::numeric_limits<result_type>::max(); }

    explicit pcg64_dxsm() : pcg64_dxsm(0) {}
    explicit pcg64_dxsm(uint64_t seed) {
        pcg64_dxsm_detail::init_state(state_, seed);
    }
    ~pcg64_dxsm() noexcept = default;

    pcg64_dxsm(const pcg64_dxsm&) = delete;
    auto operator=(const pcg64_dxsm&) -> pcg64_dxsm& = delete;

    pcg64_dxsm(pcg64_dxsm&&) = default;
    auto operator=(pcg64_dxsm&&) -> pcg64_dxsm& = default;

    auto operator()() {
        return pcg64_dxsm_detail::next(state_);
    }
};
} // namespace fluky

#endif
#endif