# VSync atomics - formally-verified atomic operations library

vatomic is a header library of *atomics operations*, supporting mainstream
architectures: ARMv7, ARMv8 (AArch32 and AArch64), RISC-V, and x86_64. The
memory ordering guarantees provided by the atomic interface are formally
described in the [VSync Memory Model (VMM)](vmm.cat) file.

Users can use the `vmm.cat` file to verify the correctness of their algorithms
with a model checker such as [Dartagnan][] or [vsyncer][].

The atomics implementations are being gradually verified to comply with VMM. At
the moment, we have completed the verification of ARMv8 64-bits with and
without LSE instructions, and RISC-V using compiler builtins.

The C implementation is C99-compatible: the build system enforces `-std=c99`
with compiler extensions disabled, so every shipped header and test builds
cleanly in that dialect out of the box. The C++ bindings and tests are likewise
compiled as C++11 (no compiler extensions), which we exercise in the same
comprehensive build.

This project is a spinoff of the VSync project and a key component in
[libvsync][]. Refer to our ASPLOS'21 [publication][paper] describing part of the
research effort put into this library.

## Getting started

In order to build and install `vatomic` run:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
cmake --install build --prefix /desired/prefix
```

`vatomic` installs headers under `include/vsync/` and a CMake package file,
allowing downstream projects to simply `find_package(vatomic CONFIG REQUIRED)`
and link against the `vatomic::vatomic` interface target.

### Using the C API

```c
#include <vsync/atomic.h>

void example(void)
{
    vatomic32_t counter;
    vatomic32_write_rlx(&counter, 0);
    vuint32_t old = vatomic32_add(&counter, 2);
    (void)old;
}
```

### Using the C++ API

```c++
#include <vsync/atomic.hpp>

void example()
{
    vsync::atomic<int> counter{};
    counter.store(42, vsync::memory_order_relaxed);
    auto prev = counter.fetch_add(1);
    (void)prev;
}
```

## Testing and verification

Besides the unit tests (`ctest`) the repository ships a Boogie-based
verification harness. To run it locally:

```sh
cmake -S . -B build -DVATOMIC_DEV=ON
cmake --build build --target build_boogie_lse
ctest --test-dir build/verify -R lse --output-on-failure
```

Targets exist for `lse`, `llsc`, `lxe`, and `no_polite_await`. Refer to
[`doc/VERIFICATION.md`](doc/VERIFICATION.md) for dependency details.

## Releases and support

Changes between versions are tracked in [CHANGELOG.md](CHANGELOG.md). Issues
and pull requests are welcome in this repository; please include details about
the architecture, compiler, and toolchain you are using.

## Contributing

See [doc/CONTRIBUTING.md](doc/CONTRIBUTING.md) for guidelines on filing issues,
running the test and verification suites, and preparing pull requests.

## Acknowledgements

This project was initially developed under the support of
[OpenHarmony Concurrency & Coordination TSG, 并发与协同TSG][tsg].

[libvsync]: https://github.com/open-s4c/libvsync
[vsyncer]: https://github.com/open-s4c/vsyncer
[Dartagnan]: https://github.com/hernanponcedeleon/dat3m
[paper]: https://dl.acm.org/doi/abs/10.1145/3445814.3446748
[tsg]: https://www.openharmony.cn/techCommittee/aboutTSG
