#include <fluky/splitmix64.hpp>
#include "util.hpp"

auto main(int, char**) -> int {
    fluky::splitmix64 rng{1234};
    fluky::test::write_bytes(rng);
    return 0;
}
