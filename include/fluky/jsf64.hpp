#ifndef FLUKY_JSF64_RANDOM_HPP
#define FLUKY_JSF64_RANDOM_HPP

#include <array>
#include <cstdint>
#include <limits>
namespace fluky {
namespace jsf64_detail {
    using jsf64_result_type = uint64_t;
    using jsf64_state = std::array<uint64_t, 4>;

    // constants
    static constexpr auto a =  7UL;
    static constexpr auto b = 13UL;
    static constexpr auto c = 37UL;
    static constexpr auto d = 0XF1EA5EEDUL;

    static constexpr auto jsf64_init_default_rounds = 20;

    inline auto next(jsf64_state& state) -> jsf64_result_type {
        auto const e = state[0] - std::rotl(state[1], a);
        state[0]     = state[1] ^ std::rotl(state[2], b);
        state[1]     = state[2] + std::rotl(state[3], c);
        state[2]     = state[3] + e;
        state[3]     = e + state[0];
        return state[3];
    }

    inline auto init_state(jsf64_state& state, uint64_t seed) -> void {
        state[0] = d;
        state[1] = seed;
        state[2] = seed;
        state[3] = seed;
        for (int i = 0; i < jsf64_init_default_rounds; ++i) {
            next(state);
        }
    }
} // namespace jsf64_detail

class jsf64 final {
    jsf64_detail::jsf64_state state_{};
public:
    using result_type = jsf64_detail::jsf64_result_type;
    static constexpr auto min() -> uint64_t { return result_type{0}; } // NOLINT
    static constexpr auto max() -> uint64_t { return std::numeric_limits<result_type>::max(); }

    explicit jsf64() : jsf64(0) {}
    explicit jsf64(uint64_t seed) {
        jsf64_detail::init_state(state_, seed);
    }
    ~jsf64() noexcept = default;

    jsf64(const jsf64&) = delete;
    auto operator=(const jsf64&) -> jsf64& = delete;

    auto operator=(jsf64&&) -> jsf64& = default;
    jsf64(jsf64&&) = default;

    auto operator()() -> result_type {
        return jsf64_detail::next(state_);
    }
};
} // namespace fluky

#endif
