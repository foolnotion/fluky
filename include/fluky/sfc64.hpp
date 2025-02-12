#ifndef FLUKY_SFC64_RANDOM_HPP
#define FLUKY_SFC64_RANDOM_HPP

#include <bit>
#include <cstdint>
#include <limits>

namespace fluky {
namespace sfc64_detail {
    using sfc64_result_type = uint64_t;

    struct sfc64_state {
        uint64_t a;
        uint64_t b;
        uint64_t c;
        uint64_t counter;
    };

    static constexpr auto barrel_shift = 24UL;
    static constexpr auto right_shift  = 11UL;
    static constexpr auto left_shift   =  3UL;

    static constexpr auto sfc64_init_default_rounds = 12;

    inline auto next(sfc64_state& state) -> sfc64_result_type {
        auto const t = state.a + state.b + state.counter++;
        state.a = state.b ^ (state.b >> right_shift);
        state.b = state.c + (state.c << left_shift);
        state.c = std::rotl(state.c, barrel_shift) + t;
        return t;
    }

    inline auto init_state(sfc64_state& state, uint64_t seed) -> void {
        state.a = seed;
        state.b = seed;
        state.c = seed;
        state.counter = 1;

        for (int i = 0; i < sfc64_init_default_rounds; ++i) {
            next(state);
        }
    }
}  // namespace sfc64_detail

class sfc64 final {
    sfc64_detail::sfc64_state state_{};

public:
    using result_type = sfc64_detail::sfc64_result_type;
    static constexpr auto min() -> uint64_t { return result_type{0}; } // NOLINT
    static constexpr auto max() -> uint64_t { return std::numeric_limits<result_type>::max(); }

    explicit sfc64() : sfc64(0) { }
    explicit sfc64(uint64_t seed) {
        sfc64_detail::init_state(state_, seed);
    }
    ~sfc64() noexcept = default;

    sfc64(sfc64 const&) = delete;
    auto operator=(sfc64 const&) -> sfc64& = delete;

    sfc64(sfc64&&) = default;
    auto operator=(sfc64&&) -> sfc64& = default;

    auto operator()() -> result_type {
        return sfc64_detail::next(state_);
    }
};
}  // namespace fluky

#endif
