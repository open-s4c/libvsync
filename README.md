# libvsync

libvsync is a C header-only library that contains most essential building blocks for concurrent applications.
That includes atomic operations, synchronization primitives and concurrent data structures.

All synchronization primitives have been verified and optimized with respect to weak memory models (WMM).
They also served as benchmarks for our ASPLOS'21 publication:
[VSync: Push-Button Verification and Optimization for Synchronization Primitives on Weak Memory Models](https://dl.acm.org/doi/abs/10.1145/3445814.3446748).

All data structures have been verified with respect to WMM.

The currently open sourced implementations
are algorithms from the literature implemented on top of our atomics, verified with model checkers,
and optimized using vsyncer.

We plan to gradually release more and more algorithms, including some of our own highly optimized algorithms.


## API Documentation
See [doc](doc/api/vsync/README.md) for details and usage examples.

## User Guide

To install and use libvsync

You need the following dependencies:
- git
- CMake 3.16 or newer
- GNU make or Ninja
- a semi-recent C-compiler: clang or gcc.

_Note: libvsync has been tested on Ubuntu 22.04 with Clang-14, and openEuler 22.03 (LTS=SP1) with gcc 10.3.1_


### for installation:

```bash
git clone <libvsync-clone-url>
cd libvsync
cmake -S. -Bbuild -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build -j $(nproc)
sudo cmake --install build
```

### to use without installation:
You copy the header files or configure your compiler
to point to the headers directly. You must keep the files under `vsync/`, e.g.,
`my_project/include/vsync/*.h`.

### to run tests:
```bash
cd libvsync
cmake -S. -Bbuild
cmake --build build
ctest --test-dir build
```

### to integrate in a CMake project

The project provides a CMake `INTERFACE` library called `vsync`, which you can link into your own
CMake targets.

To add libvsync to your project you can just use the regular CMake mechanisms such as
`add_subdirectory()`, or e.g. the CPM dependency manager.
Below an example which fetches libvsync from a remote git forge without any external dependencies:

```cmake
include(FetchContent)

FetchContent_Declare(
    libvsync
    GIT_REPOSITORY <libvsync-clone-https-url>
    GIT_TAG v3.0 # You may also specify a branch name or commit hash here.
)
FetchContent_MakeAvailable(libvsync)
```



## Developer Guide

If you would like to contribute to libvsync, check out [developer guide](doc/developer_guide.md).

## License
This library is distributed under the [MIT](LICENSE) license.

## Contact
For questions write us an email: `vsync AT huawei DOT com`

## Disclaimer
- A few algorithms require using safe memory reclamation scheme (SMR). In their examples you will find `gdump` used for that purpose.
The current version of libvsync does not contain `gdump`. Users can replace `gdump` with their own quiescent-state/grace-period based SMR scheme. We intend to release SMR schemes in future releases.

- Note that due to the fact that model checkers do not scale well with respect to number of threads/operations, our verification is not complete and cannot guarantee the absence of bugs. However, it does
increase confidence in our implementations especially on hardware with WMM (e.g ARM).

## Acknowledgement

This project is under the support of [OpenHarmony Concurrency & Coordination TSG (Technical Support Group), 并发与协同TSG][tsg].

[tsg]: https://www.openharmony.cn/techCommittee/aboutTSG
