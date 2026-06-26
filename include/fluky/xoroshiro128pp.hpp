#ifndef FLUKY_XOROSHIRO128PP_RANDOM_HPP
#define FLUKY_XOROSHIRO128PP_RANDOM_HPP

#include <array>
#include <cstdint>
#include <limits>
#include "splitmix64.hpp"

namespace fluky {
namespace xoroshiro128pp_detail {
    using xoroshiro128pp_result_type = uint64_t;
    using xoroshiro128pp_state = std::array<uint64_t, 2>;

    static constexpr auto a = 49UL; // rotation in s[0] update
    static constexpr auto b = 21UL; // shift in s[0] update
    static constexpr auto c = 28UL; // rotation in s[1] update
    static constexpr auto d = 17UL; // rotation in result

    static constexpr std::array short_jump = {
        UINT64_C(0x2BD7A6A6E99C2DDC),
        UINT64_C(0x0992CCAF6A6FCA05)
    };
    static constexpr std::array long_jump = {
        UINT64_C(0x360FD5F2CF8D5D99),
        UINT64_C(0x9C6E6877736C46E3)
    };
    static constexpr auto n = 64UL;

    inline auto init_state(xoroshiro128pp_state& state, uint64_t seed) -> void {
        for (auto& s : state) { s = splitmix64_detail::next(seed); }
    }

    inline auto next(xoroshiro128pp_state& state) -> xoroshiro128pp_result_type {
        auto const s0 = state[0];
        auto       s1 = state[1];
        auto const r  = std::rotl(s0 + s1, d) + s0;

        s1 ^= s0;
        state[0] = std::rotl(s0, a) ^ s1 ^ (s1 << b);
        state[1] = std::rotl(s1, c);
        return r;
    }

    inline auto jump(xoroshiro128pp_state& state, std::array<uint64_t, 2> const& jump_table) -> void {
        auto s0 = 0UL;
        auto s1 = 0UL;

        for (auto u : jump_table) {
            for (auto v = 0UL; v < n; ++v) {
                if ((u & UINT64_C(1) << v) != 0U) {
                    s0 ^= state[0];
                    s1 ^= state[1];
                }
                next(state);
            }
        }
        state = {s0, s1};
    }
} // namespace xoroshiro128pp_detail

class xoroshiro128pp final {
    xoroshiro128pp_detail::xoroshiro128pp_state state_{};

public:
    using result_type = xoroshiro128pp_detail::xoroshiro128pp_result_type;
    static constexpr auto min() -> uint64_t { return result_type{0}; } // NOLINT
    static constexpr auto max() -> uint64_t { return std::numeric_limits<result_type>::max(); }

    explicit xoroshiro128pp(uint64_t seed) {
        xoroshiro128pp_detail::init_state(state_, seed);
    }
    ~xoroshiro128pp() noexcept = default;

    xoroshiro128pp(xoroshiro128pp const&) = default;
    auto operator=(xoroshiro128pp const&) -> xoroshiro128pp& = default;

    xoroshiro128pp(xoroshiro128pp&&) = default;
    auto operator=(xoroshiro128pp&&) noexcept -> xoroshiro128pp& = default;

    auto operator()() {
        return xoroshiro128pp_detail::next(state_);
    }

    auto jump()      { xoroshiro128pp_detail::jump(state_, xoroshiro128pp_detail::short_jump); }
    auto long_jump() { xoroshiro128pp_detail::jump(state_, xoroshiro128pp_detail::long_jump); }

    auto fork() -> xoroshiro128pp {
        auto copy = *this;
        jump();
        return copy;
    }

    [[nodiscard]] auto state() const noexcept -> xoroshiro128pp_detail::xoroshiro128pp_state const& { return state_; }
    auto set_state(xoroshiro128pp_detail::xoroshiro128pp_state const& s) noexcept -> void { state_ = s; }
};
} // namespace fluky

#endif
