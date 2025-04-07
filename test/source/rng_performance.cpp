#include <catch2/catch_test_macros.hpp>
#include <nanobench.h>
#include <random>

#include <fluky/fluky.hpp>

namespace fluky::test {
namespace {

template<typename Rng>
auto bench(ankerl::nanobench::Bench* bench, char const* name) -> void {
    std::random_device dev;
    Rng rng(dev());
    bench->run(name, [&]() {
        auto r = std::uniform_int_distribution<uint64_t>{}(rng);
        ankerl::nanobench::doNotOptimizeAway(r);
    });
}

TEST_CASE("xoshiro256** performance", "[performance]") {
    ankerl::nanobench::Bench b;
    b.title("Xoshiro256**")
        .unit("uint64_t")
        .warmup(1000)
        .relative(true);
    b.performanceCounters(true);
    bench<fluky::xoshiro256ss>(&b, "fluky::xoshiro256**");
}

TEST_CASE("rng performance", "[performance]") {
    ankerl::nanobench::Bench b;
    b.title("Random Number Generators")
        .unit("uint64_t")
        .warmup(100)
        .relative(true);
    b.performanceCounters(true);

    bench<fluky::jsf64>(&b, "fluky::jsf64");
    bench<fluky::pcg64_dxsm>(&b, "fluky::pcg64_dxsm");
    bench<fluky::romu_duo>(&b, "fluky::romu_duo");
    bench<fluky::romu_trio>(&b, "fluky::romu_trio");
    bench<fluky::sfc64>(&b, "fluky::sfc64");
    bench<fluky::splitmix64>(&b, "fluky::splitmix64");
    bench<fluky::wyrand>(&b, "fluky::wyrand");
    bench<fluky::xoshiro256ss>(&b, "fluky::xoshiro256**");
}
} // namespace
} // namespace fluky::test
