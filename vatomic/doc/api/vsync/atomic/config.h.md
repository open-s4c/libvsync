#  [vsync](../README.md) / [atomic](README.md) / config.h
_Global configuration of vatomics._ 

The configuration defined in this file is typically applied to the whole project using vatomic. 

---
# Macros 

| Macro | Description |
|---|---|
| [VATOMIC_ENABLE_FENCE_RLX_NOP](config.h.md#macro-vatomic_enable_fence_rlx_nop) | maps `vatomic_fence_rlx` to a NOP instruction, otherwise to nothing.  |
| [VATOMIC_DISABLE_POLITE_AWAIT](config.h.md#macro-vatomic_disable_polite_await) | disables polite await in x86_64 and aarch64.  |
| [VATOMIC_BUILTINS](config.h.md#macro-vatomic_builtins) | uses `__atomic` compiler builtins to implement atomic operations.  |
| [VATOMIC_ENABLE_ATOMIC_SC](config.h.md#macro-vatomic_enable_atomic_sc) | turns barrier mode of all atomic operations and fences to seq_cst.  |
| [VATOMIC_ENABLE_ATOMIC_RLX](config.h.md#macro-vatomic_enable_atomic_rlx) | turns barrier mode of all atomic operations and fences to relaxed.  |
| [VATOMIC_DISABLE_ARM64_LSE](config.h.md#macro-vatomic_disable_arm64_lse) | Do not use ARM64 LSE instructions for atomic operations.  |
| [VATOMIC_ENABLE_ARM64_LXE](config.h.md#macro-vatomic_enable_arm64_lxe) | use ARM64 LSE instructions in slow path of LLSC for atomic operations.  |

##  Macro `VATOMIC_ENABLE_FENCE_RLX_NOP`

 
_maps_ `vatomic_fence_rlx` _to a NOP instruction, otherwise to nothing._ 



##  Macro `VATOMIC_DISABLE_POLITE_AWAIT`

 
_disables polite await in x86_64 and aarch64._ 


By default, `vatomic_await_` functions use polite await strategies with `PAUSE` or `WFE` instructions in x86_64 and arm64, respectively. Define VATOMIC_DISABLE_POLITE_AWAIT to busy loop without such instructions. 


##  Macro `VATOMIC_BUILTINS`

 
_uses_ `__atomic` _compiler builtins to implement atomic operations._ 


Unless VATOMIC_BUILTINS is defined, arm32 and aarch64 architectures employ custom assembly as atomics implementation. Other architectures, including x86_64, always employ compiler builtins. 


##  Macro `VATOMIC_ENABLE_ATOMIC_SC`

 
_turns barrier mode of all atomic operations and fences to seq_cst._ 



##  Macro `VATOMIC_ENABLE_ATOMIC_RLX`

 
_turns barrier mode of all atomic operations and fences to relaxed._ 



##  Macro `VATOMIC_DISABLE_ARM64_LSE`

 
_Do not use ARM64 LSE instructions for atomic operations._ 


If the compiler is configured to emit LSE instructions with some flag such as -march=armv8-a+lse, vatomic uses LSE instructions. Defining this flag disables the use of these instructions. 


##  Macro `VATOMIC_ENABLE_ARM64_LXE`

 
_use ARM64 LSE instructions in slow path of LLSC for atomic operations._ 


To use this option, the compiler must be configured to emit LSE instructions with some flag such as -march=armv8-a+lse. 



---
