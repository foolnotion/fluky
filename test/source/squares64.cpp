#include <fluky/squares64.hpp>
#include "util.hpp"

auto main(int, char**) -> int {
    fluky::squares64 rng{1234};
    fluky::test::write_bytes(rng);
    return 0;
}
