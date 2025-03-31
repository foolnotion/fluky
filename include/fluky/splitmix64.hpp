#ifndef FLUKY_SPLITMIX64_RANDOM_HPP
#define FLUKY_SPLITMIX64_RANDOM_HPP

#include <cstdint>
#include <limits>

namespace fluky {
namespace splitmix64_detail {
    using splitmix64_result_type = uint64_t;
    using splitmix64_state = uint64_t;

    // constants
    static constexpr uint64_t a = UINT64_C(0x9E3779B97F4A7C15);
    static constexpr uint64_t b = UINT64_C(0xBF58476D1CE4E5B9);
    static constexpr uint64_t c = UINT64_C(0x94D049BB133111EB);

    inline auto next(splitmix64_state& state) -> splitmix64_result_type {
         state += a;
         auto z = state;
         // NOLINTBEGIN(*-magic-numbers)
         z = (z ^ (z >> 30U)) * b;
         z = (z ^ (z >> 27U)) * c;
         return z ^ (z >> 31U);
         // NOLINTEND(*-magic-numbers)
    }

    inline auto init_state(splitmix64_state& state, uint64_t seed) -> void {
        state = seed;
    }
} // namespace splitmix64_detail

class splitmix64 final {
    splitmix64_detail::splitmix64_state state_{};

public:
    using result_type = splitmix64_detail::splitmix64_result_type;
    static constexpr auto min() -> uint64_t { return result_type{0}; } // NOLINT
    static constexpr auto max() -> uint64_t { return std::numeric_limits<result_type>::max(); }

    explicit splitmix64() : splitmix64(0) { }
    explicit splitmix64(uint64_t seed) {
        splitmix64_detail::init_state(state_, seed);
    }
    ~splitmix64() = default;

    splitmix64(splitmix64 const&) = delete;
    auto operator=(splitmix64 const&) -> splitmix64& = delete;

    splitmix64(splitmix64&&) = default;
    auto operator=(splitmix64&&) -> splitmix64& = default;

    auto operator()() -> result_type {
        return splitmix64_detail::next(state_);
    }
};
} // namespace fluky

#endif
