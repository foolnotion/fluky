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

TEST_CASE("sfc64", "[library]")
{
    std::cout << "sfc64\n";
    test_rng(fluky::sfc64{seed});
}

}  // namespace fluky::test
