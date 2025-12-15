#  [vsync](../README.md) / atomic
_Rich interface of atomic operations and fences._ 

VSync atomics (VAtomics) implements a rich interface of atomic operations and fences. It employs efficient custom assembly for supported architectures, and falls back to compiler builtins otherwise.

### Atomic types

[core.h](core.h.md) implements the following atomic types:



| Atomic type (A)   |Related type (T)    |
| --- | --- |
| vatomic8_t   |vuint8_t    |
| vatomic16_t   |vuint16_t    |
| vatomic32_t   |vuint32_t    |
| vatomic64_t   |vuint64_t    |
| vatomicptr_t   |void*   |



Functions are always prefixed with the atomic type, e.g., [vatomic32_read()](core_u32.h.md#function-vatomic32_read), [vatomic64_read()](core_u64.h.md#function-vatomic64_read), [vatomicptr_read()](core_ptr.h.md#function-vatomicptr_read). Arguments or return values typically are of related types, e.g., [`vatomic64_write(vatomic64_t *a, vuint64_t v)`](core_u64.h.md#function-vatomic64_write).

The types `vuint32_t` and `vuint64_t` map to equivalent types from <stdint.h> unless VSYNC_ENABLE_FREESTANDING is defined.

Note that a few functions are not defined for vatomicptr_t, e.g., add, sub, etc.

### Memory orders

By default, atomic functions are _seq_cst_ (sequentially consistent). To specify another memory order, add the corresponding suffix to the function name:



| Mode   |Suffix    |
| --- | --- |
| acquire   |`_acq`    |
| release   |`_rel`    |
| relaxed   |`_rlx`   |



> **Note:** Not all functions support every memory order. See the function documentation for more information.

### Components

VSync atomics are divided in several components.

For the declaration and definition of the core atomic operations and fences, include [core.h](core.h.md) (as in `#include <`[`vsync/atomic/core.h`](core.h.md)`>`).

For a set of advanced atomic operations optimized for politely waiting (spinning), include [await.h](await.h.md).

For readability and ease of use, include [dispatch.h](internal_2dispatch_8h). It introduces several macro dispatchers prefixed with vatomic_. These dispatchers map to the call to the respective vatomic8_, vatomic16_, vatomic32_ or vatomi64_ functions depending on the type/size of the arguments. For example, [vatomic_read(a)](dispatch.h.md#macro-vatomic_read) is the same as vatomic32_read(a) if the type of `a` is vatomic32_t.

More advanced stamped and marked atomic pointers are available in [atomicptr_stamped.h](atomicptr_stamped.h.md) and [atomicptr_markable.h](atomicptr_markable.h.md).

### Implementation variants

Optimized atomic implementations are available for ARMv7 (32 and 64 bits) and ARMv8 (32 and 64 bits). For all other architectures, including x86_64, the fallback is `__atomic` compiler builtins. To force the use of compiler builtins, define VATOMIC_BUILTINS.

For further configuration flags, please refer to [config.h](config.h.md).


### Example:



```c
#include <vsync/atomic.h>
#include <assert.h>
#include <stdio.h>

vatomic32_t var;
vatomicptr_t ptr;
int x;

void
foo(void)
{
    vatomic32_write(&var, 1000);
    vatomic32_add(&var, 10);
    vuint32_t val = vatomic32_read(&var);
    assert(val == 1010);
    assert(vatomic32_cmpxchg(&var, val, 0) == val);

    x = 123;
    vatomicptr_write(&ptr, &x);
    int *y = vatomicptr_read(&ptr);
    (*y)++;
    assert(*y == x);

    printf("passed\n");
}
int
main(void)
{
    foo();
}
```

 

---
## File Index


| File|Description|
| --- | --- |
| [vsync/atomic/atomicptr_markable.h](atomicptr_markable.h.md)|C implementation of AtomicMarkableReference in Java. |
| [vsync/atomic/atomicptr_stamped.h](atomicptr_stamped.h.md)|C implementation of AtomicstampedReference in Java. |
| [vsync/atomic/await.h](await.h.md)|Atomic await functions. |
| [vsync/atomic/await_ptr.h](await_ptr.h.md)|Atomic await functions for vatomicptr_t variables. |
| [vsync/atomic/await_u32.h](await_u32.h.md)|Atomic await functions for vatomic32_t variables. |
| [vsync/atomic/await_u64.h](await_u64.h.md)|Atomic await functions for vatomic64_t variables. |
| [vsync/atomic/config.h](config.h.md)|Global configuration of vatomics. |
| [vsync/atomic/core.h](core.h.md)|Atomic types, core atomic operations and fences. |
| [vsync/atomic/core_ptr.h](core_ptr.h.md)|Atomic functions for vatomicptr_t variables. |
| [vsync/atomic/core_sz.h](core_sz.h.md)|Atomic functions for vatomicsz_t variables. |
| [vsync/atomic/core_u16.h](core_u16.h.md)|Atomic functions for vatomic16_t variables. |
| [vsync/atomic/core_u32.h](core_u32.h.md)|Atomic functions for vatomic32_t variables. |
| [vsync/atomic/core_u64.h](core_u64.h.md)|Atomic functions for vatomic64_t variables. |
| [vsync/atomic/core_u8.h](core_u8.h.md)|Atomic functions for vatomic8_t variables. |
| [vsync/atomic/dispatch.h](dispatch.h.md)|Set of macros to dispatch atomic functions. |


---
