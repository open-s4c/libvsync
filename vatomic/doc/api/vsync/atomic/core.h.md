#  [vsync](../README.md) / [atomic](README.md) / core.h
_Atomic types, core atomic operations and fences._ 

This file is the main entry point of VSync atomics. You can include it with:



```c
#include <vsync/atomic/core.h>
```



The declaration and documentation of most atomic operations is split into files based on the atomic type operated by the function as follows:



| File   |Type    |
| --- | --- |
| [core_u8.h](core_u8.h.md)   |vatomic8_t and vuint8_t    |
| [core_u16.h](core_u16.h.md)   |vatomic16_t and vuint16_t    |
| [core_u32.h](core_u32.h.md)   |vatomic32_t and vuint32_t    |
| [core_u64.h](core_u64.h.md)   |vatomic64_t and vuint64_t    |
| [core_sz.h](core_sz.h.md)   |vatomicsz_t and vsize_t    |
| [core_ptr.h](core_ptr.h.md)   |vatomicptr_t and void *   |



Major configuration options are described in [config.h](config.h.md). 

---
# Macros 

| Macro | Description |
|---|---|
| [VATOMIC_INIT](core.h.md#macro-vatomic_init) | Initializes an atomic variable with value v.  |
| [vatomicptr](core.h.md#macro-vatomicptr) | Declares an atomic pointer type.  |
| [vatomic_cpu_pause](core.h.md#macro-vatomic_cpu_pause) | Calls CPU pause instruction if available, e.g., `PAUSE` in x86.  |
| [VATOMIC_DISABLE_MACRO_UNDEF](core.h.md#macro-vatomic_disable_macro_undef) | Disables undefines of non-exported macros.  |

##  Macro `VATOMIC_INIT`

```c
VATOMIC_INIT(v)
```

 
_Initializes an atomic variable with value v._ 



##  Macro `vatomicptr`

```c
vatomicptr(T)
```

 
_Declares an atomic pointer type._ 


When declaring an atomic pointer it may be useful to annotate it with the original type. Use [vatomicptr(T)](core.h.md#macro-vatomicptr) macro for that.


### Example:



```c
typedef struct mcs_node_s {
   vatomicptr(struct mcs_node_s*) next;
} mcs_node_t;
```

 


##  Macro `vatomic_cpu_pause`

```c
vatomic_cpu_pause()
```

 
_Calls CPU pause instruction if available, e.g.,_ `PAUSE` _in x86._ 


Tight spinloops often overuse the memory subsytem. This macro calls an architecture-dependent instruction to slowdown spinloops (`PAUSE` in x86 and `YIELD` in aarch64). Define `VSYNC_DISABLE_POLITE_AWAIT` to disable the effect of this macro.


### Example:



```c
while (!vatomic_read(&flag)) vatomic_cpu_pause();
```

 


##  Macro `VATOMIC_DISABLE_MACRO_UNDEF`

 
_Disables undefines of non-exported macros._ 


By default, all non-exported macros are undefined at the end of [atomic.h](../atomic.h.md). When VATOMIC_DISABLE_MACRO_UNDEF, no macro is undefined. This is useful for testing. 


---
# Functions 

| Function | Description |
|---|---|
| [vatomic_fence](core.h.md#function-vatomic_fence) | Executes an atomic fence with seq_cst memory order.  |
| [vatomic_fence_acq](core.h.md#function-vatomic_fence_acq) | Executes an atomic fence with acquire memory order.  |
| [vatomic_fence_rel](core.h.md#function-vatomic_fence_rel) | Executes an atomic fence with release memory order.  |
| [vatomic_fence_rlx](core.h.md#function-vatomic_fence_rlx) | Executes an atomic fence with relaxed memory order.  |

##  Function `vatomic_fence`

```c
static void vatomic_fence(void)
``` 
_Executes an atomic fence with seq_cst memory order._ 



##  Function `vatomic_fence_acq`

```c
static void vatomic_fence_acq(void)
``` 
_Executes an atomic fence with acquire memory order._ 



##  Function `vatomic_fence_rel`

```c
static void vatomic_fence_rel(void)
``` 
_Executes an atomic fence with release memory order._ 



##  Function `vatomic_fence_rlx`

```c
static void vatomic_fence_rlx(void)
``` 
_Executes an atomic fence with relaxed memory order._ 


> **Note:** Technically, there no fence_rlx, it compiles to a NOP. 



---
