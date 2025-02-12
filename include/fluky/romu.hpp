#ifndef FLUKY_ROMU_RANDOM_HPP
#define FLUKY_ROMU_RANDOM_HPP

#include <array>
#include <cstdint>
#include <limits>
#include "detail/splitmix64.hpp"

namespace fluky {
namespace romu_detail {
using romu_result_type = uint64_t;
using romu_duo_state   = std::array<uint64_t, 2>;
using romu_trio_state  = std::array<uint64_t, 3>;
using romu_quad_state  = std::array<uint64_t, 4>;

// constants
static constexpr auto a = 15241094284759029579UL;
static constexpr auto b = 36UL; // used by romu duo
static constexpr auto c = 15UL; // used by romu duo
static constexpr auto d = 12UL; // used by romu trio
static constexpr auto e = 44UL; // used by romu trio
static constexpr auto f = 52UL; // used by romu quad
static constexpr auto g = 19UL; // used by romu quad

static constexpr auto romu_init_default_rounds = 10; // used for initialization

template<typename State = romu_duo_state>
auto next(State& state) -> romu_result_type {
    auto x = state[0];
    state[0] = a * state[1];
    state[1] = std::rotl(state[1], b) + std::rotl(state[1], c) - x;
    return x;
}

template<>
inline auto next<romu_trio_state>(romu_trio_state& state) -> romu_result_type {
    auto [x, y, z] = state;
    state[0] = a * z;
    state[1] = y - x;
    state[1] = std::rotl(state[1], d);
    state[2] = z - y;
    state[2] = std::rotl(state[2], e);
    return x;
}

template<>
inline auto next<romu_quad_state>(romu_quad_state& state) -> romu_result_type {
    auto [w, x, y, z] = state;
    state[0] = a * z;
    state[1] = z + std::rotl(w, f);
    state[1] = y - x;
    state[2] = y + w;
    state[2] = std::rotl(state[2], g);
    return x;
}

template<typename State>
auto init_state(State& state, uint64_t seed) -> void {
    for (auto& s : state) { s = detail::splitmix64(seed); }
    for (int i = 0; i < romu_init_default_rounds; ++i) {
        next<State>(state);
    }
}
} // namespace romu_detail

template<typename State>
class romu final {
    State state_{};

public:
    using result_type = romu_detail::romu_result_type;
    static constexpr auto min() -> uint64_t { return result_type{0}; } // NOLINT
    static constexpr auto max() -> uint64_t { return std::numeric_limits<result_type>::max(); }

    explicit romu() : romu(0) { }
    explicit romu(uint64_t seed) {
        romu_detail::init_state(state_, seed);
    }
    ~romu() noexcept = default;

    romu(romu const&) = delete;
    auto operator=(romu const&) -> romu& = delete;

    romu(romu&&) = default;
    auto operator=(romu&&) noexcept -> romu& = default;

    auto operator()() {
        return romu_detail::next(state_);
    }
};

using romu_duo  = romu<romu_detail::romu_duo_state>;
using romu_trio = romu<romu_detail::romu_trio_state>;
using romu_quad = romu<romu_detail::romu_quad_state>;
} // namespace fluky

#endif