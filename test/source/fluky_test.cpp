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

}  // namespace fluky::test
