#ifndef FLUKY_SQUARES64_RANDOM_HPP
#define FLUKY_SQUARES64_RANDOM_HPP

#include <cstdint>
#include <limits>
#include "splitmix64.hpp"

namespace fluky {
namespace squares64_detail {
    using squares64_result_type = uint64_t;

    struct squares64_state {
        uint64_t ctr;
        uint64_t key;
    };

    inline auto next(squares64_state& state) -> squares64_result_type {
        uint64_t x, y, z;
        y = x = state.ctr++ * state.key; z = y + state.key;
        x = x*x + y; x = (x >> 32) | (x << 32); // round 1
        x = x*x + z; x = (x >> 32) | (x << 32); // round 2
        x = x*x + y; x = (x >> 32) | (x << 32); // round 3
        x = x*x + z; return x >> 32 ^ ((x*x + y) >> 32); // round 4
    }

    inline auto init_state(squares64_state& state, uint64_t seed) -> void {
        state.ctr = 0;
        // derive a key with good bit distribution; must be non-zero and odd
        state.key = splitmix64_detail::next(seed) | UINT64_C(1);
    }

    inline auto advance(squares64_state& state, uint64_t n) -> void {
        state.ctr += n;
    }
} // namespace squares64_detail

class squares64 final {
    squares64_detail::squares64_state state_{};

public:
    using result_type = squares64_detail::squares64_result_type;
    static constexpr auto min() -> uint64_t { return result_type{0}; } // NOLINT
    static constexpr auto max() -> uint64_t { return std::numeric_limits<result_type>::max(); }

    explicit squares64() : squares64(0) {}
    explicit squares64(uint64_t seed) {
        squares64_detail::init_state(state_, seed);
    }
    ~squares64() noexcept = default;

    squares64(squares64 const&) = default;
    auto operator=(squares64 const&) -> squares64& = default;

    squares64(squares64&&) = default;
    auto operator=(squares64&&) noexcept -> squares64& = default;

    auto operator()() -> result_type {
        return squares64_detail::next(state_);
    }

    auto jump()      -> void { squares64_detail::advance(state_, uint64_t{1} << 32); }
    auto long_jump() -> void { squares64_detail::advance(state_, uint64_t{1} << 48); }
    auto advance(uint64_t n) -> void { squares64_detail::advance(state_, n); }

    [[nodiscard]] auto state() const noexcept -> squares64_detail::squares64_state const& { return state_; }
    auto set_state(squares64_detail::squares64_state const& s) noexcept -> void { state_ = s; }
};
} // namespace fluky

#endif
