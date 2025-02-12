#include <fluky/pcg64_dxsm.hpp>
#include "util.hpp"

auto main(int, char**) -> int {
    fluky::pcg64_dxsm rng{1234};
    fluky::test::write_bytes(rng);
    return 0;
}