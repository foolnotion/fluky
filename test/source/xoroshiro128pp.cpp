#include <fluky/xoroshiro128pp.hpp>
#include "util.hpp"

auto main(int, char**) -> int {
    fluky::xoroshiro128pp rng{1234};
    fluky::test::write_bytes(rng);
    return 0;
}
