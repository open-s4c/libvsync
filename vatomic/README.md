# VSync atomics - formally-verified atomic operations library

vatomic is a header library of *atomics operations*, supporting mainstream
architectures: ARMv7, ARMv8 (AArch32 and AArch64), RISC-V, and x86_64. The
memory ordering guarantees provided by the atomic interface are formally
described in the [VSync Memory Model (VMM)](vmm.cat) file.

Users can use the `vmm.cat` file to verify the correctness of their algorithms
with a model checker such as [Dartagnan][] or [vsyncer][].

The atomics implementations are being gradually verified to comply with VMM. At
the moment, we have completed the verification of ARMv8 64-bits with and
without LSE instructions.

This project is a spinoff of the VSync project and a key component in
[libvsync][].

Refer to our ASPLOS'21 [publication][paper] describing part of the
research effort put into this library.

## Acknowledgements

This project was initially developed under the support of
[OpenHarmony Concurrency & Coordination TSG, 并发与协同TSG][tsg].

[libvsync]: https://github.com/open-s4c/libvsync
[vsyncer]: https://github.com/open-s4c/vsyncer
[Dartagnan]: https://github.com/hernanponcedeleon/dat3m
[paper]: https://dl.acm.org/doi/abs/10.1145/3445814.3446748
[tsg]: https://www.openharmony.cn/techCommittee/aboutTSG
