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
    static constexpr uint128_t multiplier{15750249268501108917ULL};

    struct pcg64_dxsm_state {
        uint128_t s;
        uint128_t i;
    };

    inline auto next(pcg64_dxsm_state& state) -> pcg64_dxsm_result_type {
        auto const s = state.s;
        state.s = (s * multiplier) + state.i;
        auto hi = static_cast<uint64_t>(s >> a);
        auto lo = static_cast<uint64_t>(s | 1U);
        hi ^= hi >> b;
        hi *= static_cast<uint64_t>(multiplier);
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

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    inline auto lcg_advance(uint128_t state, uint128_t delta, uint128_t cur_mult, uint128_t cur_plus) -> uint128_t {
        uint128_t acc_mult{1};
        uint128_t acc_plus{0};
        while (delta > 0) {
            if ((delta & uint128_t{1}) != 0) { // NOLINT(hicpp-signed-bitwise)
                acc_mult *= cur_mult;
                acc_plus = (acc_plus * cur_mult) + cur_plus;
            }
            cur_plus  = (cur_mult + 1) * cur_plus;
            cur_mult *= cur_mult;
            delta >>= 1U; // NOLINT(hicpp-signed-bitwise)
        }
        return (acc_mult * state) + acc_plus;
    }

    inline auto advance(pcg64_dxsm_state& state, uint128_t n) -> void {
        state.s = lcg_advance(state.s, n, multiplier, state.i);
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

    pcg64_dxsm(const pcg64_dxsm&) = default;
    auto operator=(const pcg64_dxsm&) -> pcg64_dxsm& = default;

    pcg64_dxsm(pcg64_dxsm&&) = default;
    auto operator=(pcg64_dxsm&&) -> pcg64_dxsm& = default;

    auto operator()() {
        return pcg64_dxsm_detail::next(state_);
    }

    auto jump()      -> void { pcg64_dxsm_detail::advance(state_, uint128_t{1} << 64U); } // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    auto long_jump() -> void { pcg64_dxsm_detail::advance(state_, uint128_t{1} << 96U); } // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    auto advance(uint128_t n) -> void { pcg64_dxsm_detail::advance(state_, n); }

    [[nodiscard]] auto state() const noexcept -> pcg64_dxsm_detail::pcg64_dxsm_state const& { return state_; }
    auto set_state(pcg64_dxsm_detail::pcg64_dxsm_state const& s) noexcept -> void { state_ = s; }
};
} // namespace fluky

#endif
#endif