#ifndef FLUKY_CONCEPTS_HPP
#define FLUKY_CONCEPTS_HPP

#include <concepts>
#include <cstdint>
#include <random>

namespace fluky {

// Satisfies the C++20 UniformRandomBitGenerator named requirement.
template<typename Rng>
concept rng = std::uniform_random_bit_generator<Rng>;

// Has state() / set_state() for checkpoint and restore.
template<typename Rng>
concept stateful_rng = rng<Rng> && requires(Rng r) {
    r.set_state(r.state());
};

// Has jump() / long_jump() for well-defined stream partitioning.
// Covers both polynomial-based jumps (xoshiro family) and arithmetic ones.
template<typename Rng>
concept jumpable_rng = rng<Rng> && requires(Rng r) {
    r.jump();
    r.long_jump();
};

// Has advance(n): fast-forward by an arbitrary number of steps in O(log N)
// or better. Subsumes jumpable_rng — every advanceable generator also has
// fixed-period jump() / long_jump(). Does not cover polynomial-based
// generators like xoshiro256ss that lack a general fast-forward algorithm.
template<typename Rng>
concept advanceable_rng = jumpable_rng<Rng> && requires(Rng r, uint64_t n) {
    r.advance(n);
};

// Has fork(): atomically copies the current state and advances the
// original by one jump period, returning the pre-jump copy.
// The natural primitive for spawning independent parallel streams.
template<typename Rng>
concept forkable_rng = rng<Rng> && requires(Rng r) {
    { r.fork() } -> std::same_as<Rng>;
};

} // namespace fluky
#endif
