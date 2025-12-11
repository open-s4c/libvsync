# Verification Guide

`vatomic` ships a Boogie-based pipeline that checks each atomic primitive
against the [VSync Memory Model][]. The verification assets live under `verify/`
and power the `build_boogie_*` targets exposed by CMake.

[VSync Memory Model]: ../vmm.cat
[VMM]: ../vmm.cat

## Dependencies

The verification targets are available when configuring the project with
`-DVATOMIC_TESTS=ON` (the default for top-level builds). Locally you will need:

- CMake (>= 3.16) and a build backend (Ninja or Make)
- A Rust toolchain (`cargo`) to build the assembly parser/generator in
  `verify/src`
- Boogie CLI (`boogie`) and its SMT solver dependency (Z3)
- Cross compilers: `aarch64-linux-gnu-gcc` (ARMv8) and
  `riscv64-linux-gnu-gcc` (RISC-V)

### Details

- Boogie support for irreducible control flow (needed for several atomics)
  only landed in release [**3.5.5**][] (see our [contribution to
  upstream][]). Stick to that version or newer when running the pipeline
  outside the CI image.
- Z3 results varied wildly with old releases. Use at least the version baked
  into `.github/Dockerfile.boogie` (currently **z3-solver 4.13.0.0** from
  PyPI) to match CI behaviour.
- The cross-compilers shipped with Ubuntu 24.04 (GCC **13.2** series, as
  installed in `.github/Dockerfile.boogie`) are the minimum known-good
  versions. Older Ubuntu toolchains could not inline the builtin assembly
  that powers the verification flow.

[contribution to upstream]: https://github.com/boogie-org/boogie/pull/1032
[**3.5.5**]: https://github.com/boogie-org/boogie/releases/tag/v3.5.5

### Using the CICD container

CI uses the container image `ghcr.io/open-s4c/vatomic/boogie-ci:latest`,
which bundles all of the above. You can pull it locally if you prefer a
hermetic environment.

```sh
docker pull ghcr.io/open-s4c/vatomic/boogie-ci:latest
```

Then you can start the container mounting the vatomic directory as follows:

```sh
cd vatomic
docker run -it --rm  -v $(pwd):$(pwd) -w $(pwd) -u $(id -u):$(id -g) \
    ghcr.io/open-s4c/vatomic/boogie-ci:latest
```

You current `vatomic` directory is mounted over the exact same path in the
container (`-v` flag), and you start with the workdir set to that directory
(`-w` flag). Also the user id and user group match the ids in the host (`-u`
flag), so that if you create files they preserve the right ownership after
leaving the container.

Inside the container, you can follow the instructions in the next section.

---

## Running the verification suite

1. Configure the project and enable `VATOMIC_TESTS` targets:

   ```sh
   cmake -S . -B build -DVATOMIC_TESTS=ON
   ```

2. Build one (or all) Boogie targets. Available options are `lse`, `llsc`,
   `lxe`, and `no_polite_await`.

   ```sh
   cmake --build build --target build_boogie_lse
   ```

   Each target:

   - Preprocesses `vsync/atomic.h` with the chosen compiler flags to obtain
     `atomic.s` (and optionally sanitises it via `cleaner.sh`).
   - Runs the Rust generator (`cargo run` via `generate.sh`) to parse the
     assembly and emit Boogie (`.bpl`) files.
   - Instantiates the templates under `verify/boogie/templates`.
   - Executes Boogie to prove each atomic operation (driven by `verify.sh`).

3. Inspect results via CTest or directly in the generated logs:

   ```sh
   ctest --test-dir build/verify -R lse --output-on-failure
   # Logs: build/verify/lse/*.log
   ```

### Customising the toolchain

- Pass `-DARMV8_CMAKE_C_COMPILER=/path/to/aarch64-linux-gnu-gcc` or
  `-DRISCV_CMAKE_C_COMPILER=/path/to/riscv64-linux-gnu-gcc` to override the
  default cross compilers.
- Set `BPL`/`Z3_EXE` or adjust `PATH` so Boogie and Z3 are discoverable.
- Export `CARGO` to point at a specific `cargo` binary when multiple toolchains
  coexist on the system.

---

## Maintaining the function list

The lists of verified entry points lives in `verify/lists`. When adding new
atomic primitives, update that file so the verification targets cover the
new functions. Refer to the [Boogie templates](#boogie-templates) section
for the available templates and how they map to atomic behaviour.

---

## Boogie templates

Each `.bpl` template describes some aspect of correctness for the supported
atomics. For each atomic, one or more templates are instantiated to fully
specify the proof obligations. The templates are:

- `read_only` — describes atomics that never write, e.g., `read`, `await`
  (non-RMW)
- `read` — describes atomics that perform a read, e.g., `RMW`, `read`, `await`,
  `await-RMW`
- `write` — describes `write` atomics
- `await` — describes `await` and `await-RMW` atomics
- `rmw` — describes atomics performing a read-modify-write operation, e.g.,
  `RMW` and `await-RMW`

### Template parameters

Each template uses one or more of the following parameters, written as `#NAME`:

- `#registers` — comma separated identifiers specifying all registers used in
  the function, e.g., `a0, a1, a2, x5, x6`.
- `#address` — identifier specifying the register that originally holds the
  address, e.g., `a0`.
- `#input1` — identifier for the register holding the first function argument,
  e.g., for `write(a, v)` the value of `v`, or for `cmpxchg(a, e, v)` the value
  of `e`. Could be `a1`.
- `#input2` — second function argument. Could be `a2`.
- `#output` — register holding the output value at the end of the function,
  e.g., `a0`.
- `#implementation` — body of the function, including assumes for all procedure
  parameters, code, and two basic loop invariants.
- `#state` — comma separated list of identifiers for all additional state
  introduced by the architecture, e.g., `local_monitor, monitor_exclusive`.

### Template matrix

List of templates and their functions:

| template     | write number | return value | store order | load order |
|--------------|--------------|--------------|-------------|------------|
| `read_only`  | 0            | output       | –           | –          |
| `read`       | –            | ret          | –           | yes        |
| `write`      | ≤1           | –            | yes         | –          |
| `await`      | –            | await cond   | –           | –          |
| `must_store` | 1 (+value)   | –            | yes         | –          |
| `rmw`        | (+op)        | –            | yes         | –          |

List of templates used by each atomic:

| template     | read | write | RMW | await | await RMW |
|--------------|------|-------|-----|-------|-----------|
| `read_only`  | x    |       |     | x     |           |
| `read`       | x    |       | x   | x     | x         |
| `write`      |      | x     | x   |       | x         |
| `await`      |      |       |     | x     | x         |
| `must_store` |      | x     |     |       |           |
| `rmw`        |      |       | x   |       | x         |

---
