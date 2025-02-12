#include "fluky/fluky.hpp"

#include <fmt/core.h>
#include <catch2/catch_test_macros.hpp>

namespace fluky::test {
namespace {
    constexpr auto seed{100UL};

    auto test_rng(auto&& rng) {
        for (auto i = 0; i < 4; ++i) {
            fmt::print("{}\n", rng());
        }
    }
} // namespace

TEST_CASE("romu", "[library]")
{
    SECTION("romu duo") { test_rng(fluky::romu_duo{seed}); }
    SECTION("romu trio") { test_rng(fluky::romu_trio{seed}); }
}

TEST_CASE("jsf", "[library]") {
    fmt::print("{}\n", "jsf64");
    test_rng(fluky::jsf64{seed});
}

TEST_CASE("xoshiro256ss", "[library]") {
    fmt::print("{}\n", "xoshiro256ss");
    test_rng(fluky::xoshiro256ss{seed});
}

TEST_CASE("sfc64", "[library]")
{
    fmt::print("{}\n", "sfc64");
    test_rng(fluky::sfc64{seed});
}

}  // namespace fluky::test