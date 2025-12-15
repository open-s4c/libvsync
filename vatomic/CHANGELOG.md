# Changelog

All notable changes to this project will be documented in this file.
This project partially comply with [Semantic Versioning][].

[Semantic Versioning]: https://semver.org/spec/v2.0.0.html

## [2.4.0]

- Introduced vsync::atomic for C++ (vsync/atomic.hpp)
- Added new verification pipeline in `verify/`
- Introduced RISCV verification
- Improved documentation on verification and contributing to the project
- Reorganization of template files
- Minor refactoring of internal headers (not user facing)

## [2.3.1]

- Added verification of AArch64 atomic implementation
- Enabled RISC-V tests (linking with libatomic)
- Fixed installation of CMake find_package scripts

## [2.3.0]

- **Forked** from libvsync v4.1.0 as standalone project
- Compilation on macOS-arm and NetBSD

## [2.2.1]

Released as part of libvsync v4.0.2

- type conversion warnings in `vatomic32_get_*` rmw APIs.

## [2.2.0]

Released as part of libvsync v4.0.1

- added `const` to `vatomic*_t *a` parameter in read-only vatomic APIs
- remove undef of used macros from `vtypes.h`.
- deprecated `atomic*_wait_mask_neq` and `atomic*_wait_mask_eq` APIs

## [2.1.1]

Released as part of libvsync v3.5.0

- Added vmm.cat
- Documentation updates

## [2.1.0] - 2024-01-26

Initial public release as part of libvsync v3.2.0.

## [2.0.0]

- Moved to `vatomic_` prefix to avoid name clashing.
- Introduced new atomics such as `max`.

## [1.0.0]

Initial version using `atomic_` prefix.
