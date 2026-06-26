#ifndef FLUKY_CONCEPTS_HPP
#define FLUKY_CONCEPTS_HPP

#include <concepts>

namespace fluky {

template<typename Rng>
concept jumpable_rng = requires(Rng r) {
    r.jump();
    r.long_jump();
};

} // namespace fluky
#endif
