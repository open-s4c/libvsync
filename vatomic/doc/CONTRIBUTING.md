# Contributing to vatomic

This document collects the expectations for bug reports, patches, and
documentation changes so reviews can stay focused on the substance of your
contribution.

## Before you start

- Review the [README](../README.md) to understand the project scope, supported
  architectures, and the current verification status.
- Open a GitHub issue or discussion before investing significant effort,
  especially when proposing a new API or architecture backend. Please describe
  the motivating use case, target architecture, compiler/toolchain versions, and
  any memory-model considerations.
- For bug reports include (if possible) a minimal reproducer, the architecture
  target, and the compiler (including version and flags) you are using.

## Development workflow

1. Fork the repository and create a topic branch; keep pull requests scoped to a
   single logical change.
2. Keep commits focused and descriptive. Reference the GitHub issue (if any) in
   the commit message body.
3. Update or add documentation/tests alongside code changes so the behavior
   stays discoverable.
4. Run the relevant test suites (see below) before submitting your pull request.

## Building and testing

`vatomic` is a header-only library, but tests are compiled programs. The
build process is orchestrated by CMake >= 3.16.
Ensure the following two flags are set when configuring the project:

- `VATOMIC_TESTS=ON` - enables build of unit tests,
- `VATOMIC_DEV=ON` -  enables template file expansion.

Here is an initial sequence of commands to compile and run all tests:

```sh
cmake -S . -B build -DVATOMIC_DEV=ON -DVATOMIC_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

### Template files

The `vatomic` library only consists of the files in `include/`. These are
not written by hand, but rather expanded from a series of template files,
which can be found in `template/include`.

When setting `VATOMIC_DEV`, the build system will trigger the expansion of
the templates whenever they are modified, overwriting the files in include. In
this process, two tools are used:

- [`tmplr`][] - reads the `.h.in` files and expands them with the additional
  rules in `template/include/vsync/atomic/vatomic.rules`.
- `clang-format` - reformats the overwritten header files in `include`
  to maintain a coherent and readable output.

If not installed, `tmplr` is automatically downloaded (with curl) and built
with your standard C compiler. `clang-format` is expected to be preinstalled.

Most test cases in `test/` are also expanded templates. The following commands
expand the template files:

```sh
cmake --build --target vatomic-generate
cmake --build --target vatomic-test-generate
```

### Verification

Enabling `VATOMIC_TESTS` exposes the verification targets under `verify/`
if the required tools (Boogie, Z3, etc) are installed. When you touch code
that is formally verified, please run the relevant harness (for example
`build_boogie_lse` followed by `ctest --test-dir build/verify -R lse
--output-on-failure`). Refer to [`doc/VERIFICATION.md`](VERIFICATION.md)
for the full list of targets and dependencies.

### API documentation

A Markdown version of the API documentation is kept in `doc/api`. The process
to extract the documentation of the code is performed in two steps:

- [Doxygen][] parses the header files in `include` and generates a set of
  XML files.
- [mdox][] parses the XML files and writes the new Markdown files into
  `doc/api`.

The user has to manually install these tools to be able update the API
documentation. The following commands create the up-to-date documentation:

```sh
cmake --build build --target markdown
```

## Coding guidelines

- Follow the existing file layout under `include/vsync/`. Keep architecture
  specific code isolated under the matching directory.
- Run `clang-format` (configuration in `.clang-format`) on all C/C++ changes
  and `cmake-format` on CMake files. The build systems has two targets to
  help applying the format over the whole codebase: `clang-format-apply` and
  `cmake-format-apply`. The CI enforces these styles with `clang-format` 14.
- Favor small, documented helpers over macro-heavy solutions when possible.
  When macros are unavoidable, prefer well-scoped names.
- Maintain a consistent memory-ordering story: document the expected ordering
  whenever you add or update APIs, and add litmus/Boogie coverage when
  applicable.
- Public APIs must compile cleanly as both C and C++ (see
  `include/vsync/atomic.h` and `include/vsync/atomic.hpp` for patterns
  to follow).

## Documentation

- Update the Doxygen comments in `include/` and the manuals under `doc/man/`
  when you add or modify APIs.
- If your change affects build, testing, or verification flows, update
  `README.md`, `doc/VERIFICATION.md`, or add a new document under `doc/` so the
  information is easy to find.

## Pull request checklist

- [ ] Tests and relevant verification targets pass locally.
- [ ] `clang-format` / `cmake-format` ran on touched files.
- [ ] Documentation reflects the change.
- [ ] Each commit message explains **why** the change is needed.
- [ ] New public APIs include usage examples or tests.

Thanks again for contributing! Maintaining a clear testing story and
documentation trail keeps the project approachable for the broader concurrency
community.
