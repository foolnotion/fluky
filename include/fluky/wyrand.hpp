#ifndef FLUKY_WYRAND_RANDOM_HPP
#define FLUKY_WYRAND_RANDOM_HPP

#include <cstdint>
#include <limits>
#include "detail/common.hpp"

#ifdef __SIZEOF_INT128__

namespace fluky {
namespace wyrand_detail {
	using wyrand_result_type = uint64_t;
	using wyrand_state = uint64_t;

	std::size_t constexpr digits = std::numeric_limits<uint64_t>::digits;

	inline auto next(wyrand_state& state) -> wyrand_result_type {
	    state += UINT64_C(0xA0761D6478BD642F);
	    auto t = static_cast<uint128_t>(state) * (state ^ UINT64_C(0xE7037ED1A0B428DB));
	    return static_cast<uint64_t>((t >> digits) ^ t);
	}

	inline auto init_state(wyrand_state& state, uint64_t seed) -> void {
	    state = seed;
	}
} // namespace wyrand_detail

class wyrand final {
    wyrand_detail::wyrand_state state_{};

public:
    using result_type = wyrand_detail::wyrand_result_type;
    static constexpr auto min() -> uint64_t { return result_type{0}; } // NOLINT
    static constexpr auto max() -> uint64_t { return std::numeric_limits<result_type>::max(); }

    explicit wyrand() : wyrand(0) {}
    explicit wyrand(uint64_t seed) {
	wyrand_detail::init_state(state_, seed);
    }
    ~wyrand() = default;

    wyrand(wyrand const&) = delete;
    auto operator=(wyrand const&) -> wyrand& = delete;

    wyrand(wyrand&&) = default;
    auto operator=(wyrand&&) -> wyrand& = default;

    auto operator()() -> result_type {
		return wyrand_detail::next(state_);
    }
};
} // namespace fluky

#endif

#endif
