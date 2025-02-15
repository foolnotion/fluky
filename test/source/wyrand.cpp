#include <fluky/wyrand.hpp>
#include "util.hpp"

auto main(int, char**) -> int {
    fluky::wyrand rng{1234};
    fluky::test::write_bytes(rng);
    return 0;
}
