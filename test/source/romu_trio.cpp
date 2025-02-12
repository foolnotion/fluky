#include <fluky/romu.hpp>
#include "util.hpp"

auto main(int, char**) -> int {
    fluky::romu_trio rng{1234};
    fluky::test::write_bytes(rng);
    return 0;
}