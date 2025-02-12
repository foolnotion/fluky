#include <fluky/xoshiro256ss.hpp>
#include "util.hpp"

auto main(int, char**) -> int {
    fluky::xoshiro256ss rng{1234};
    fluky::test::write_bytes(rng);
    return 0;
}