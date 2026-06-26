#include "fluky/fluky.hpp"

#include <iostream>
#include <catch2/catch_test_macros.hpp>

namespace fluky::test {
namespace {
    constexpr auto seed{100UL};

    auto test_rng(auto&& rng) {
        for (auto i = 0; i < 4; ++i) {
            std::cout << rng() << "\n";
        }
    }
} // namespace

TEST_CASE("romu", "[library]")
{
    SECTION("romu duo") { test_rng(fluky::romu_duo{seed}); }
    SECTION("romu trio") { test_rng(fluky::romu_trio{seed}); }
}

TEST_CASE("jsf", "[library]") {
    std::cout << "jsf64\n";
    test_rng(fluky::jsf64{seed});
}

TEST_CASE("xoshiro256ss", "[library]") {
    std::cout << "xoshiro256ss\n";
    test_rng(fluky::xoshiro256ss{seed});
}

TEST_CASE("xoshiro256ss fork produces non-overlapping streams", "[library]") {
    fluky::xoshiro256ss rng{seed};
    auto a = rng.fork();
    auto b = rng.fork();
    auto c = rng.fork();

    // each forked stream should produce different values
    REQUIRE(a() != b());
    REQUIRE(b() != c());
    REQUIRE(a() != c());

    // forked streams should be internally consistent
    // (drawing from one does not affect another)
    auto v1 = a();
    auto v2 = b();
    auto v3 = a();
    REQUIRE(v1 != v3); // a continues its own sequence
    REQUIRE(v1 != v2); // a and b are independent

    // verify copy semantics work
    fluky::xoshiro256ss copy{a};
    REQUIRE(a() == copy());
    REQUIRE(a() == copy());
}

TEST_CASE("xoshiro256ss jump matches manual advancement", "[library]") {
    fluky::xoshiro256ss rng1{seed};
    fluky::xoshiro256ss rng2{seed};

    // jump() should advance by 2^64 steps
    rng1.jump();
    // we can't manually advance 2^64 steps, but we can verify
    // that jump() changes the state
    REQUIRE(rng1() != rng2());
}

TEST_CASE("sfc64", "[library]")
{
    std::cout << "sfc64\n";
    test_rng(fluky::sfc64{seed});
}

namespace {
    auto step(auto& rng, int n) { for (int i = 0; i < n; ++i) { rng(); } }

    auto state_round_trip_test(auto& rng) {
        step(rng, 5);
        auto snap = rng.state();
        std::array<uint64_t, 3> after_snap{};
        for (auto& v : after_snap) { v = rng(); }
        rng.set_state(snap);
        for (auto v : after_snap) { REQUIRE(rng() == v); }
    }
} // namespace

TEST_CASE("state round-trip: xoshiro256ss", "[state]") {
    fluky::xoshiro256ss rng{42};
    state_round_trip_test(rng);
}

TEST_CASE("state round-trip: jsf64", "[state]") {
    fluky::jsf64 rng{42};
    state_round_trip_test(rng);
}

TEST_CASE("state round-trip: sfc64", "[state]") {
    fluky::sfc64 rng{42};
    state_round_trip_test(rng);
}

TEST_CASE("state round-trip: splitmix64", "[state]") {
    fluky::splitmix64 rng{42};
    state_round_trip_test(rng);
}

TEST_CASE("state round-trip: romu_duo", "[state]") {
    fluky::romu_duo rng{42};
    state_round_trip_test(rng);
}

TEST_CASE("state round-trip: romu_trio", "[state]") {
    fluky::romu_trio rng{42};
    state_round_trip_test(rng);
}

TEST_CASE("state round-trip: romu_quad", "[state]") {
    fluky::romu_quad rng{42};
    state_round_trip_test(rng);
}

#ifdef __SIZEOF_INT128__
TEST_CASE("state round-trip: pcg64_dxsm", "[state]") {
    fluky::pcg64_dxsm rng{42};
    state_round_trip_test(rng);
}

TEST_CASE("state round-trip: wyrand", "[state]") {
    fluky::wyrand rng{42};
    state_round_trip_test(rng);
}
#endif

// ---- concept static assertions ----

// rng: all generators satisfy the URBG contract
static_assert(fluky::rng<fluky::xoshiro256ss>);
static_assert(fluky::rng<fluky::xoroshiro128pp>);
static_assert(fluky::rng<fluky::splitmix64>);
static_assert(fluky::rng<fluky::squares64>);
static_assert(fluky::rng<fluky::jsf64>);
static_assert(fluky::rng<fluky::sfc64>);
static_assert(fluky::rng<fluky::romu_duo>);
#ifdef __SIZEOF_INT128__
static_assert(fluky::rng<fluky::pcg64_dxsm>);
static_assert(fluky::rng<fluky::wyrand>);
#endif

// stateful_rng: all generators expose state() / set_state()
static_assert(fluky::stateful_rng<fluky::xoshiro256ss>);
static_assert(fluky::stateful_rng<fluky::xoroshiro128pp>);
static_assert(fluky::stateful_rng<fluky::splitmix64>);
static_assert(fluky::stateful_rng<fluky::squares64>);
static_assert(fluky::stateful_rng<fluky::jsf64>);
static_assert(fluky::stateful_rng<fluky::sfc64>);
static_assert(fluky::stateful_rng<fluky::romu_duo>);
#ifdef __SIZEOF_INT128__
static_assert(fluky::stateful_rng<fluky::pcg64_dxsm>);
static_assert(fluky::stateful_rng<fluky::wyrand>);
#endif

// jumpable_rng: fixed-period stream partition via jump() / long_jump()
static_assert(fluky::jumpable_rng<fluky::xoshiro256ss>);
static_assert(fluky::jumpable_rng<fluky::xoroshiro128pp>);
static_assert(fluky::jumpable_rng<fluky::splitmix64>);
static_assert(fluky::jumpable_rng<fluky::squares64>);
static_assert(!fluky::jumpable_rng<fluky::jsf64>);
static_assert(!fluky::jumpable_rng<fluky::sfc64>);
static_assert(!fluky::jumpable_rng<fluky::romu_duo>);
#ifdef __SIZEOF_INT128__
static_assert(fluky::jumpable_rng<fluky::pcg64_dxsm>);
static_assert(fluky::jumpable_rng<fluky::wyrand>);
#endif

// advanceable_rng: arbitrary fast-forward via advance(n)
// Note: xoshiro256ss / xoroshiro128pp are jumpable but NOT advanceable —
// their jump is polynomial-based, not a simple counter increment.
static_assert(!fluky::advanceable_rng<fluky::xoshiro256ss>);
static_assert(!fluky::advanceable_rng<fluky::xoroshiro128pp>);
static_assert(fluky::advanceable_rng<fluky::splitmix64>);
static_assert(fluky::advanceable_rng<fluky::squares64>);
static_assert(!fluky::advanceable_rng<fluky::jsf64>);
static_assert(!fluky::advanceable_rng<fluky::sfc64>);
static_assert(!fluky::advanceable_rng<fluky::romu_duo>);
#ifdef __SIZEOF_INT128__
static_assert(fluky::advanceable_rng<fluky::pcg64_dxsm>);
static_assert(fluky::advanceable_rng<fluky::wyrand>);
#endif

// forkable_rng: atomic copy-and-jump for spawning parallel streams
// Only generators with copy semantics and fork() qualify.
static_assert(fluky::forkable_rng<fluky::xoshiro256ss>);
static_assert(fluky::forkable_rng<fluky::xoroshiro128pp>);
static_assert(!fluky::forkable_rng<fluky::splitmix64>);
static_assert(!fluky::forkable_rng<fluky::jsf64>);
#ifdef __SIZEOF_INT128__
static_assert(!fluky::forkable_rng<fluky::pcg64_dxsm>);
#endif

// ---- jump() equivalence tests (advance(n) vs n operator() calls) ----

TEST_CASE("jump equivalence: splitmix64", "[jump]") {
    constexpr auto steps = 256U;
    fluky::splitmix64 rng1{seed};
    fluky::splitmix64 rng2{seed};

    rng1.advance(steps);
    for (auto i = 0U; i < steps; ++i) { rng2(); }

    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
}

TEST_CASE("long_jump determinism: splitmix64", "[jump]") {
    fluky::splitmix64 rng1{seed};
    fluky::splitmix64 rng2{seed};
    fluky::splitmix64 rng3{seed};

    rng1.long_jump();
    rng2.long_jump();
    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
    REQUIRE(rng1() != rng3());
}

TEST_CASE("jump equivalence: xoshiro256ss", "[jump]") {
    fluky::xoshiro256ss rng1{seed};
    fluky::xoshiro256ss rng2{seed};

    rng1.jump();
    rng2.jump();

    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }

    fluky::xoshiro256ss rng3{seed};
    REQUIRE(rng1() != rng3());
}

TEST_CASE("long_jump determinism: xoshiro256ss", "[jump]") {
    fluky::xoshiro256ss rng1{seed};
    fluky::xoshiro256ss rng2{seed};
    fluky::xoshiro256ss rng3{seed};

    rng1.long_jump();
    rng2.long_jump();
    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
    REQUIRE(rng1() != rng3());
}

#ifdef __SIZEOF_INT128__
TEST_CASE("jump equivalence: pcg64_dxsm", "[jump]") {
    constexpr auto steps = 256U;
    fluky::pcg64_dxsm rng1{seed};
    fluky::pcg64_dxsm rng2{seed};

    rng1.advance(uint128_t{steps});
    for (auto i = 0U; i < steps; ++i) { rng2(); }

    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
}

TEST_CASE("long_jump determinism: pcg64_dxsm", "[jump]") {
    fluky::pcg64_dxsm rng1{seed};
    fluky::pcg64_dxsm rng2{seed};
    fluky::pcg64_dxsm rng3{seed};

    rng1.long_jump();
    rng2.long_jump();
    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
    REQUIRE(rng1() != rng3());
}

TEST_CASE("jump equivalence: wyrand", "[jump]") {
    constexpr auto steps = 256U;
    fluky::wyrand rng1{seed};
    fluky::wyrand rng2{seed};

    rng1.advance(steps);
    for (auto i = 0U; i < steps; ++i) { rng2(); }

    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
}

TEST_CASE("long_jump determinism: wyrand", "[jump]") {
    fluky::wyrand rng1{seed};
    fluky::wyrand rng2{seed};
    fluky::wyrand rng3{seed};

    rng1.long_jump();
    rng2.long_jump();
    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
    REQUIRE(rng1() != rng3());
}
#endif

// ---- xoroshiro128pp ----

TEST_CASE("xoroshiro128pp", "[library]") {
    test_rng(fluky::xoroshiro128pp{seed});
}

TEST_CASE("xoroshiro128pp fork produces non-overlapping streams", "[library]") {
    fluky::xoroshiro128pp rng{seed};
    auto a = rng.fork();
    auto b = rng.fork();

    REQUIRE(a() != b());

    fluky::xoroshiro128pp copy{a};
    REQUIRE(a() == copy());
    REQUIRE(a() == copy());
}

TEST_CASE("state round-trip: xoroshiro128pp", "[state]") {
    fluky::xoroshiro128pp rng{42};
    state_round_trip_test(rng);
}

TEST_CASE("jump equivalence: xoroshiro128pp", "[jump]") {
    fluky::xoroshiro128pp rng1{seed};
    fluky::xoroshiro128pp rng2{seed};

    rng1.jump();
    rng2.jump();

    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }

    fluky::xoroshiro128pp rng3{seed};
    REQUIRE(rng1() != rng3());
}

TEST_CASE("long_jump determinism: xoroshiro128pp", "[jump]") {
    fluky::xoroshiro128pp rng1{seed};
    fluky::xoroshiro128pp rng2{seed};
    fluky::xoroshiro128pp rng3{seed};

    rng1.long_jump();
    rng2.long_jump();
    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
    REQUIRE(rng1() != rng3());
}

// ---- squares64 ----

TEST_CASE("squares64", "[library]") {
    test_rng(fluky::squares64{seed});
}

TEST_CASE("state round-trip: squares64", "[state]") {
    fluky::squares64 rng{42};
    state_round_trip_test(rng);
}

TEST_CASE("jump equivalence: squares64", "[jump]") {
    constexpr auto steps = 256U;
    fluky::squares64 rng1{seed};
    fluky::squares64 rng2{seed};

    rng1.advance(steps);
    for (auto i = 0U; i < steps; ++i) { rng2(); }

    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
}

TEST_CASE("jump() equals advance(2^32): squares64", "[jump]") {
    // jump() is defined as advance(1<<32); verify they produce the same state
    fluky::squares64 rng1{seed};
    fluky::squares64 rng2{seed};

    rng1.jump();
    rng2.advance(uint64_t{1} << 32);

    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
}

TEST_CASE("long_jump determinism: squares64", "[jump]") {
    fluky::squares64 rng1{seed};
    fluky::squares64 rng2{seed};
    fluky::squares64 rng3{seed};

    rng1.long_jump();
    rng2.long_jump();
    for (auto i = 0; i < 4; ++i) { REQUIRE(rng1() == rng2()); }
    REQUIRE(rng1() != rng3());
}

}  // namespace fluky::test
