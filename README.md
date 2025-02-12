# fluky

_fluky_ is a small, header-only C++20 library of 64-bit pseudo-random number generators. The library is motivated by the need for a collection of reliable PRNGs that is easy to install and package.

### Currently implemented generators:

| Name | Desc | Url |
|------|------|-----|
| `jsf64` | Jenkins Small Fast PRNG | http://burtleburtle.net/bob/rand/smallprng.html |
| `pcg64_dxsm` | Permuted Congruential Generator Double Xor Shift Multiply | https://www.pcg-random.org/index.html |
| `romu` | Rotate-Multiply PRNG | https://www.romu-random.org/ |
| `sfc64` | Small Fast Chaotic PRNG | https://pracrand.sourceforge.net/RNG_engines.txt |
| `xoshiro256ss` | Xor/Shift/Rotate PRNG (the all-purpose ** version) | https://dsiutils.di.unimi.it |

### Acknowledgements

- the great explanations from https://dotat.at/@/2023-06-21-pcg64-dxsm.html
- the C implementations from https://github.com/bashtage/randomgen/
- this thread on HN https://news.ycombinator.com/item?id=22447848
- tweag's random-quality tests https://github.com/tweag/random-quality
- this guide for _PractRand_ https://www.pcg-random.org/posts/how-to-test-with-practrand.html

### Alternatives

- TRNG4 https://github.com/rabauke/trng4
- Random123 https://github.com/DEShawResearch/random123
- RandomGen https://github.com/bashtage/randomgen/
- Xoshiro https://github.com/nessan/xoshiro


# Building and installing

See the [BUILDING](BUILDING.md) document.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

[MIT](LICENSE)