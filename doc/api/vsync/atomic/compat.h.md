#  [vsync](../README.md) / [atomic](README.md) / compat.h
_Set of functions backward compatible with libvsync 1.0._ 

These functions are deprecated. Please, use the new vatomic interface. 

---
# Functions 

| Function | Description |
|---|---|
| [atomic_read](compat.h.md#function-atomic_read) | read/write/init functions  |
| [atomic_read_acq](compat.h.md#function-atomic_read_acq) | Deprecated: See vatomic32_read_acq.  |
| [atomic_read_rlx](compat.h.md#function-atomic_read_rlx) | Deprecated: See vatomic32_read_rlx.  |
| [atomic32_read](compat.h.md#function-atomic32_read) | Deprecated: See vatomic32_read.  |
| [atomic32_read_acq](compat.h.md#function-atomic32_read_acq) | Deprecated: See vatomic32_read_acq.  |
| [atomic32_read_rlx](compat.h.md#function-atomic32_read_rlx) | Deprecated: See vatomic32_read_rlx.  |
| [atomic64_read](compat.h.md#function-atomic64_read) | Deprecated: See vatomic64_read.  |
| [atomic64_read_acq](compat.h.md#function-atomic64_read_acq) | Deprecated: See vatomic64_read_acq.  |
| [atomic64_read_rlx](compat.h.md#function-atomic64_read_rlx) | Deprecated: See vatomic64_read_rlx.  |
| [atomicptr_read](compat.h.md#function-atomicptr_read) | Deprecated: See vatomicptr_read.  |
| [atomicptr_read_acq](compat.h.md#function-atomicptr_read_acq) | Deprecated: See vatomicptr_read_acq.  |
| [atomicptr_read_rlx](compat.h.md#function-atomicptr_read_rlx) | Deprecated: See vatomicptr_read_rlx.  |
| [atomic_write](compat.h.md#function-atomic_write) | Deprecated: See vatomic32_write.  |
| [atomic_write_rel](compat.h.md#function-atomic_write_rel) | Deprecated: See vatomic32_write_rel.  |
| [atomic_write_rlx](compat.h.md#function-atomic_write_rlx) | Deprecated: See vatomic32_write_rlx.  |
| [atomic32_write](compat.h.md#function-atomic32_write) | Deprecated: See vatomic32_write.  |
| [atomic32_write_rel](compat.h.md#function-atomic32_write_rel) | Deprecated: See vatomic32_write_rel.  |
| [atomic32_write_rlx](compat.h.md#function-atomic32_write_rlx) | Deprecated: See vatomic32_write_rlx.  |
| [atomic64_write](compat.h.md#function-atomic64_write) | Deprecated: See vatomic64_write.  |
| [atomic64_write_rel](compat.h.md#function-atomic64_write_rel) | Deprecated: See vatomic64_write_rel.  |
| [atomic64_write_rlx](compat.h.md#function-atomic64_write_rlx) | Deprecated: See vatomic64_write_rlx.  |
| [atomicptr_write](compat.h.md#function-atomicptr_write) | Deprecated: See vatomicptr_write.  |
| [atomicptr_write_rel](compat.h.md#function-atomicptr_write_rel) | Deprecated: See vatomicptr_write_rel.  |
| [atomicptr_write_rlx](compat.h.md#function-atomicptr_write_rlx) | Deprecated: See vatomicptr_write_rlx.  |
| [atomic_init](compat.h.md#function-atomic_init) | Deprecated: See vatomic32_init.  |
| [atomic32_init](compat.h.md#function-atomic32_init) | Deprecated: See vatomic32_init.  |
| [atomic64_init](compat.h.md#function-atomic64_init) | Deprecated: See vatomic64_init.  |
| [atomicptr_init](compat.h.md#function-atomicptr_init) | Deprecated: See vatomicptr_init.  |
| [atomic_xchg](compat.h.md#function-atomic_xchg) | cmpxchg/xchg functions  |
| [atomic_cmpxchg](compat.h.md#function-atomic_cmpxchg) | Deprecated: See vatomic32_cmpxchg.  |
| [atomic_xchg_acq](compat.h.md#function-atomic_xchg_acq) | Deprecated: See vatomic32_xchg_acq.  |
| [atomic_cmpxchg_acq](compat.h.md#function-atomic_cmpxchg_acq) | Deprecated: See vatomic32_cmpxchg_acq.  |
| [atomic_xchg_rel](compat.h.md#function-atomic_xchg_rel) | Deprecated: See vatomic32_xchg_rel.  |
| [atomic_cmpxchg_rel](compat.h.md#function-atomic_cmpxchg_rel) | Deprecated: See vatomic32_cmpxchg_rel.  |
| [atomic_xchg_rlx](compat.h.md#function-atomic_xchg_rlx) | Deprecated: See vatomic32_xchg_rlx.  |
| [atomic_cmpxchg_rlx](compat.h.md#function-atomic_cmpxchg_rlx) | Deprecated: See vatomic32_cmpxchg_rlx.  |
| [atomic32_xchg](compat.h.md#function-atomic32_xchg) | Deprecated: See vatomic32_xchg.  |
| [atomic32_cmpxchg](compat.h.md#function-atomic32_cmpxchg) | Deprecated: See vatomic32_cmpxchg.  |
| [atomic32_xchg_acq](compat.h.md#function-atomic32_xchg_acq) | Deprecated: See vatomic32_xchg_acq.  |
| [atomic32_cmpxchg_acq](compat.h.md#function-atomic32_cmpxchg_acq) | Deprecated: See vatomic32_cmpxchg_acq.  |
| [atomic32_xchg_rel](compat.h.md#function-atomic32_xchg_rel) | Deprecated: See vatomic32_xchg_rel.  |
| [atomic32_cmpxchg_rel](compat.h.md#function-atomic32_cmpxchg_rel) | Deprecated: See vatomic32_cmpxchg_rel.  |
| [atomic32_xchg_rlx](compat.h.md#function-atomic32_xchg_rlx) | Deprecated: See vatomic32_xchg_rlx.  |
| [atomic32_cmpxchg_rlx](compat.h.md#function-atomic32_cmpxchg_rlx) | Deprecated: See vatomic32_cmpxchg_rlx.  |
| [atomic64_xchg](compat.h.md#function-atomic64_xchg) | Deprecated: See vatomic64_xchg.  |
| [atomic64_cmpxchg](compat.h.md#function-atomic64_cmpxchg) | Deprecated: See vatomic64_cmpxchg.  |
| [atomic64_xchg_acq](compat.h.md#function-atomic64_xchg_acq) | Deprecated: See vatomic64_xchg_acq.  |
| [atomic64_cmpxchg_acq](compat.h.md#function-atomic64_cmpxchg_acq) | Deprecated: See vatomic64_cmpxchg_acq.  |
| [atomic64_xchg_rel](compat.h.md#function-atomic64_xchg_rel) | Deprecated: See vatomic64_xchg_rel.  |
| [atomic64_cmpxchg_rel](compat.h.md#function-atomic64_cmpxchg_rel) | Deprecated: See vatomic64_cmpxchg_rel.  |
| [atomic64_xchg_rlx](compat.h.md#function-atomic64_xchg_rlx) | Deprecated: See vatomic64_xchg_rlx.  |
| [atomic64_cmpxchg_rlx](compat.h.md#function-atomic64_cmpxchg_rlx) | Deprecated: See vatomic64_cmpxchg_rlx.  |
| [atomicptr_xchg](compat.h.md#function-atomicptr_xchg) | Deprecated: See vatomicptr_xchg.  |
| [atomicptr_cmpxchg](compat.h.md#function-atomicptr_cmpxchg) | Deprecated: See vatomicptr_cmpxchg.  |
| [atomicptr_xchg_acq](compat.h.md#function-atomicptr_xchg_acq) | Deprecated: See vatomicptr_xchg_acq.  |
| [atomicptr_cmpxchg_acq](compat.h.md#function-atomicptr_cmpxchg_acq) | Deprecated: See vatomicptr_cmpxchg_acq.  |
| [atomicptr_xchg_rel](compat.h.md#function-atomicptr_xchg_rel) | Deprecated: See vatomicptr_xchg_rel.  |
| [atomicptr_cmpxchg_rel](compat.h.md#function-atomicptr_cmpxchg_rel) | Deprecated: See vatomicptr_cmpxchg_rel.  |
| [atomicptr_xchg_rlx](compat.h.md#function-atomicptr_xchg_rlx) | Deprecated: See vatomicptr_xchg_rlx.  |
| [atomicptr_cmpxchg_rlx](compat.h.md#function-atomicptr_cmpxchg_rlx) | Deprecated: See vatomicptr_cmpxchg_rlx.  |
| [atomic_add](compat.h.md#function-atomic_add) | add, sub, or, and, xor  |
| [atomic_get_add](compat.h.md#function-atomic_get_add) | Deprecated: See vatomic32_get_add.  |
| [atomic_add_acq](compat.h.md#function-atomic_add_acq) | Deprecated: See vatomic32_add_get_acq.  |
| [atomic_get_add_acq](compat.h.md#function-atomic_get_add_acq) | Deprecated: See vatomic32_get_add_acq.  |
| [atomic_add_rel](compat.h.md#function-atomic_add_rel) | Deprecated: See vatomic32_add_get_rel.  |
| [atomic_get_add_rel](compat.h.md#function-atomic_get_add_rel) | Deprecated: See vatomic32_get_add_rel.  |
| [atomic_add_rlx](compat.h.md#function-atomic_add_rlx) | Deprecated: See vatomic32_add_get_rlx.  |
| [atomic_get_add_rlx](compat.h.md#function-atomic_get_add_rlx) | Deprecated: See vatomic32_get_add_rlx.  |
| [atomic_sub](compat.h.md#function-atomic_sub) | Deprecated: See vatomic32_sub_get.  |
| [atomic_get_sub](compat.h.md#function-atomic_get_sub) | Deprecated: See vatomic32_get_sub.  |
| [atomic_sub_acq](compat.h.md#function-atomic_sub_acq) | Deprecated: See vatomic32_sub_get_acq.  |
| [atomic_get_sub_acq](compat.h.md#function-atomic_get_sub_acq) | Deprecated: See vatomic32_get_sub_acq.  |
| [atomic_sub_rel](compat.h.md#function-atomic_sub_rel) | Deprecated: See vatomic32_sub_get_rel.  |
| [atomic_get_sub_rel](compat.h.md#function-atomic_get_sub_rel) | Deprecated: See vatomic32_get_sub_rel.  |
| [atomic_sub_rlx](compat.h.md#function-atomic_sub_rlx) | Deprecated: See vatomic32_sub_get_rlx.  |
| [atomic_get_sub_rlx](compat.h.md#function-atomic_get_sub_rlx) | Deprecated: See vatomic32_get_sub_rlx.  |
| [atomic_xor](compat.h.md#function-atomic_xor) | Deprecated: See vatomic32_xor_get.  |
| [atomic_get_xor](compat.h.md#function-atomic_get_xor) | Deprecated: See vatomic32_get_xor.  |
| [atomic_xor_acq](compat.h.md#function-atomic_xor_acq) | Deprecated: See vatomic32_xor_get_acq.  |
| [atomic_get_xor_acq](compat.h.md#function-atomic_get_xor_acq) | Deprecated: See vatomic32_get_xor_acq.  |
| [atomic_xor_rel](compat.h.md#function-atomic_xor_rel) | Deprecated: See vatomic32_xor_get_rel.  |
| [atomic_get_xor_rel](compat.h.md#function-atomic_get_xor_rel) | Deprecated: See vatomic32_get_xor_rel.  |
| [atomic_xor_rlx](compat.h.md#function-atomic_xor_rlx) | Deprecated: See vatomic32_xor_get_rlx.  |
| [atomic_get_xor_rlx](compat.h.md#function-atomic_get_xor_rlx) | Deprecated: See vatomic32_get_xor_rlx.  |
| [atomic_and](compat.h.md#function-atomic_and) | Deprecated: See vatomic32_and_get.  |
| [atomic_get_and](compat.h.md#function-atomic_get_and) | Deprecated: See vatomic32_get_and.  |
| [atomic_and_acq](compat.h.md#function-atomic_and_acq) | Deprecated: See vatomic32_and_get_acq.  |
| [atomic_get_and_acq](compat.h.md#function-atomic_get_and_acq) | Deprecated: See vatomic32_get_and_acq.  |
| [atomic_and_rel](compat.h.md#function-atomic_and_rel) | Deprecated: See vatomic32_and_get_rel.  |
| [atomic_get_and_rel](compat.h.md#function-atomic_get_and_rel) | Deprecated: See vatomic32_get_and_rel.  |
| [atomic_and_rlx](compat.h.md#function-atomic_and_rlx) | Deprecated: See vatomic32_and_get_rlx.  |
| [atomic_get_and_rlx](compat.h.md#function-atomic_get_and_rlx) | Deprecated: See vatomic32_get_and_rlx.  |
| [atomic_or](compat.h.md#function-atomic_or) | Deprecated: See vatomic32_or_get.  |
| [atomic_get_or](compat.h.md#function-atomic_get_or) | Deprecated: See vatomic32_get_or.  |
| [atomic_or_acq](compat.h.md#function-atomic_or_acq) | Deprecated: See vatomic32_or_get_acq.  |
| [atomic_get_or_acq](compat.h.md#function-atomic_get_or_acq) | Deprecated: See vatomic32_get_or_acq.  |
| [atomic_or_rel](compat.h.md#function-atomic_or_rel) | Deprecated: See vatomic32_or_get_rel.  |
| [atomic_get_or_rel](compat.h.md#function-atomic_get_or_rel) | Deprecated: See vatomic32_get_or_rel.  |
| [atomic_or_rlx](compat.h.md#function-atomic_or_rlx) | Deprecated: See vatomic32_or_get_rlx.  |
| [atomic_get_or_rlx](compat.h.md#function-atomic_get_or_rlx) | Deprecated: See vatomic32_get_or_rlx.  |
| [atomic32_add](compat.h.md#function-atomic32_add) | Deprecated: See vatomic32_add_get.  |
| [atomic32_get_add](compat.h.md#function-atomic32_get_add) | Deprecated: See vatomic32_get_add.  |
| [atomic32_add_acq](compat.h.md#function-atomic32_add_acq) | Deprecated: See vatomic32_add_get_acq.  |
| [atomic32_get_add_acq](compat.h.md#function-atomic32_get_add_acq) | Deprecated: See vatomic32_get_add_acq.  |
| [atomic32_add_rel](compat.h.md#function-atomic32_add_rel) | Deprecated: See vatomic32_add_get_rel.  |
| [atomic32_get_add_rel](compat.h.md#function-atomic32_get_add_rel) | Deprecated: See vatomic32_get_add_rel.  |
| [atomic32_add_rlx](compat.h.md#function-atomic32_add_rlx) | Deprecated: See vatomic32_add_get_rlx.  |
| [atomic32_get_add_rlx](compat.h.md#function-atomic32_get_add_rlx) | Deprecated: See vatomic32_get_add_rlx.  |
| [atomic32_sub](compat.h.md#function-atomic32_sub) | Deprecated: See vatomic32_sub_get.  |
| [atomic32_get_sub](compat.h.md#function-atomic32_get_sub) | Deprecated: See vatomic32_get_sub.  |
| [atomic32_sub_acq](compat.h.md#function-atomic32_sub_acq) | Deprecated: See vatomic32_sub_get_acq.  |
| [atomic32_get_sub_acq](compat.h.md#function-atomic32_get_sub_acq) | Deprecated: See vatomic32_get_sub_acq.  |
| [atomic32_sub_rel](compat.h.md#function-atomic32_sub_rel) | Deprecated: See vatomic32_sub_get_rel.  |
| [atomic32_get_sub_rel](compat.h.md#function-atomic32_get_sub_rel) | Deprecated: See vatomic32_get_sub_rel.  |
| [atomic32_sub_rlx](compat.h.md#function-atomic32_sub_rlx) | Deprecated: See vatomic32_sub_get_rlx.  |
| [atomic32_get_sub_rlx](compat.h.md#function-atomic32_get_sub_rlx) | Deprecated: See vatomic32_get_sub_rlx.  |
| [atomic32_xor](compat.h.md#function-atomic32_xor) | Deprecated: See vatomic32_xor_get.  |
| [atomic32_get_xor](compat.h.md#function-atomic32_get_xor) | Deprecated: See vatomic32_get_xor.  |
| [atomic32_xor_acq](compat.h.md#function-atomic32_xor_acq) | Deprecated: See vatomic32_xor_get_acq.  |
| [atomic32_get_xor_acq](compat.h.md#function-atomic32_get_xor_acq) | Deprecated: See vatomic32_get_xor_acq.  |
| [atomic32_xor_rel](compat.h.md#function-atomic32_xor_rel) | Deprecated: See vatomic32_xor_get_rel.  |
| [atomic32_get_xor_rel](compat.h.md#function-atomic32_get_xor_rel) | Deprecated: See vatomic32_get_xor_rel.  |
| [atomic32_xor_rlx](compat.h.md#function-atomic32_xor_rlx) | Deprecated: See vatomic32_xor_get_rlx.  |
| [atomic32_get_xor_rlx](compat.h.md#function-atomic32_get_xor_rlx) | Deprecated: See vatomic32_get_xor_rlx.  |
| [atomic32_and](compat.h.md#function-atomic32_and) | Deprecated: See vatomic32_and_get.  |
| [atomic32_get_and](compat.h.md#function-atomic32_get_and) | Deprecated: See vatomic32_get_and.  |
| [atomic32_and_acq](compat.h.md#function-atomic32_and_acq) | Deprecated: See vatomic32_and_get_acq.  |
| [atomic32_get_and_acq](compat.h.md#function-atomic32_get_and_acq) | Deprecated: See vatomic32_get_and_acq.  |
| [atomic32_and_rel](compat.h.md#function-atomic32_and_rel) | Deprecated: See vatomic32_and_get_rel.  |
| [atomic32_get_and_rel](compat.h.md#function-atomic32_get_and_rel) | Deprecated: See vatomic32_get_and_rel.  |
| [atomic32_and_rlx](compat.h.md#function-atomic32_and_rlx) | Deprecated: See vatomic32_and_get_rlx.  |
| [atomic32_get_and_rlx](compat.h.md#function-atomic32_get_and_rlx) | Deprecated: See vatomic32_get_and_rlx.  |
| [atomic32_or](compat.h.md#function-atomic32_or) | Deprecated: See vatomic32_or_get.  |
| [atomic32_get_or](compat.h.md#function-atomic32_get_or) | Deprecated: See vatomic32_get_or.  |
| [atomic32_or_acq](compat.h.md#function-atomic32_or_acq) | Deprecated: See vatomic32_or_get_acq.  |
| [atomic32_get_or_acq](compat.h.md#function-atomic32_get_or_acq) | Deprecated: See vatomic32_get_or_acq.  |
| [atomic32_or_rel](compat.h.md#function-atomic32_or_rel) | Deprecated: See vatomic32_or_get_rel.  |
| [atomic32_get_or_rel](compat.h.md#function-atomic32_get_or_rel) | Deprecated: See vatomic32_get_or_rel.  |
| [atomic32_or_rlx](compat.h.md#function-atomic32_or_rlx) | Deprecated: See vatomic32_or_get_rlx.  |
| [atomic32_get_or_rlx](compat.h.md#function-atomic32_get_or_rlx) | Deprecated: See vatomic32_get_or_rlx.  |
| [atomic64_add](compat.h.md#function-atomic64_add) | Deprecated: See vatomic64_add_get.  |
| [atomic64_get_add](compat.h.md#function-atomic64_get_add) | Deprecated: See vatomic64_get_add.  |
| [atomic64_add_acq](compat.h.md#function-atomic64_add_acq) | Deprecated: See vatomic64_add_get_acq.  |
| [atomic64_get_add_acq](compat.h.md#function-atomic64_get_add_acq) | Deprecated: See vatomic64_get_add_acq.  |
| [atomic64_add_rel](compat.h.md#function-atomic64_add_rel) | Deprecated: See vatomic64_add_get_rel.  |
| [atomic64_get_add_rel](compat.h.md#function-atomic64_get_add_rel) | Deprecated: See vatomic64_get_add_rel.  |
| [atomic64_add_rlx](compat.h.md#function-atomic64_add_rlx) | Deprecated: See vatomic64_add_get_rlx.  |
| [atomic64_get_add_rlx](compat.h.md#function-atomic64_get_add_rlx) | Deprecated: See vatomic64_get_add_rlx.  |
| [atomic64_sub](compat.h.md#function-atomic64_sub) | Deprecated: See vatomic64_sub_get.  |
| [atomic64_get_sub](compat.h.md#function-atomic64_get_sub) | Deprecated: See vatomic64_get_sub.  |
| [atomic64_sub_acq](compat.h.md#function-atomic64_sub_acq) | Deprecated: See vatomic64_sub_get_acq.  |
| [atomic64_get_sub_acq](compat.h.md#function-atomic64_get_sub_acq) | Deprecated: See vatomic64_get_sub_acq.  |
| [atomic64_sub_rel](compat.h.md#function-atomic64_sub_rel) | Deprecated: See vatomic64_sub_get_rel.  |
| [atomic64_get_sub_rel](compat.h.md#function-atomic64_get_sub_rel) | Deprecated: See vatomic64_get_sub_rel.  |
| [atomic64_sub_rlx](compat.h.md#function-atomic64_sub_rlx) | Deprecated: See vatomic64_sub_get_rlx.  |
| [atomic64_get_sub_rlx](compat.h.md#function-atomic64_get_sub_rlx) | Deprecated: See vatomic64_get_sub_rlx.  |
| [atomic64_xor](compat.h.md#function-atomic64_xor) | Deprecated: See vatomic64_xor_get.  |
| [atomic64_get_xor](compat.h.md#function-atomic64_get_xor) | Deprecated: See vatomic64_get_xor.  |
| [atomic64_xor_acq](compat.h.md#function-atomic64_xor_acq) | Deprecated: See vatomic64_xor_get_acq.  |
| [atomic64_get_xor_acq](compat.h.md#function-atomic64_get_xor_acq) | Deprecated: See vatomic64_get_xor_acq.  |
| [atomic64_xor_rel](compat.h.md#function-atomic64_xor_rel) | Deprecated: See vatomic64_xor_get_rel.  |
| [atomic64_get_xor_rel](compat.h.md#function-atomic64_get_xor_rel) | Deprecated: See vatomic64_get_xor_rel.  |
| [atomic64_xor_rlx](compat.h.md#function-atomic64_xor_rlx) | Deprecated: See vatomic64_xor_get_rlx.  |
| [atomic64_get_xor_rlx](compat.h.md#function-atomic64_get_xor_rlx) | Deprecated: See vatomic64_get_xor_rlx.  |
| [atomic64_and](compat.h.md#function-atomic64_and) | Deprecated: See vatomic64_and_get.  |
| [atomic64_get_and](compat.h.md#function-atomic64_get_and) | Deprecated: See vatomic64_get_and.  |
| [atomic64_and_acq](compat.h.md#function-atomic64_and_acq) | Deprecated: See vatomic64_and_get_acq.  |
| [atomic64_get_and_acq](compat.h.md#function-atomic64_get_and_acq) | Deprecated: See vatomic64_get_and_acq.  |
| [atomic64_and_rel](compat.h.md#function-atomic64_and_rel) | Deprecated: See vatomic64_and_get_rel.  |
| [atomic64_get_and_rel](compat.h.md#function-atomic64_get_and_rel) | Deprecated: See vatomic64_get_and_rel.  |
| [atomic64_and_rlx](compat.h.md#function-atomic64_and_rlx) | Deprecated: See vatomic64_and_get_rlx.  |
| [atomic64_get_and_rlx](compat.h.md#function-atomic64_get_and_rlx) | Deprecated: See vatomic64_get_and_rlx.  |
| [atomic64_or](compat.h.md#function-atomic64_or) | Deprecated: See vatomic64_or_get.  |
| [atomic64_get_or](compat.h.md#function-atomic64_get_or) | Deprecated: See vatomic64_get_or.  |
| [atomic64_or_acq](compat.h.md#function-atomic64_or_acq) | Deprecated: See vatomic64_or_get_acq.  |
| [atomic64_get_or_acq](compat.h.md#function-atomic64_get_or_acq) | Deprecated: See vatomic64_get_or_acq.  |
| [atomic64_or_rel](compat.h.md#function-atomic64_or_rel) | Deprecated: See vatomic64_or_get_rel.  |
| [atomic64_get_or_rel](compat.h.md#function-atomic64_get_or_rel) | Deprecated: See vatomic64_get_or_rel.  |
| [atomic64_or_rlx](compat.h.md#function-atomic64_or_rlx) | Deprecated: See vatomic64_or_get_rlx.  |
| [atomic64_get_or_rlx](compat.h.md#function-atomic64_get_or_rlx) | Deprecated: See vatomic64_get_or_rlx.  |
| [atomic_inc](compat.h.md#function-atomic_inc) | increment/decrement  |
| [atomic_get_inc](compat.h.md#function-atomic_get_inc) | Deprecated: See vatomic32_get_inc.  |
| [atomic_inc_acq](compat.h.md#function-atomic_inc_acq) | Deprecated: See vatomic32_inc_get_acq.  |
| [atomic_get_inc_acq](compat.h.md#function-atomic_get_inc_acq) | Deprecated: See vatomic32_get_inc_acq.  |
| [atomic_inc_rel](compat.h.md#function-atomic_inc_rel) | Deprecated: See vatomic32_inc_get_rel.  |
| [atomic_get_inc_rel](compat.h.md#function-atomic_get_inc_rel) | Deprecated: See vatomic32_get_inc_rel.  |
| [atomic_inc_rlx](compat.h.md#function-atomic_inc_rlx) | Deprecated: See vatomic32_inc_get_rlx.  |
| [atomic_get_inc_rlx](compat.h.md#function-atomic_get_inc_rlx) | Deprecated: See vatomic32_get_inc_rlx.  |
| [atomic_dec](compat.h.md#function-atomic_dec) | Deprecated: See vatomic32_dec_get.  |
| [atomic_get_dec](compat.h.md#function-atomic_get_dec) | Deprecated: See vatomic32_get_dec.  |
| [atomic_dec_acq](compat.h.md#function-atomic_dec_acq) | Deprecated: See vatomic32_dec_get_acq.  |
| [atomic_get_dec_acq](compat.h.md#function-atomic_get_dec_acq) | Deprecated: See vatomic32_get_dec_acq.  |
| [atomic_dec_rel](compat.h.md#function-atomic_dec_rel) | Deprecated: See vatomic32_dec_get_rel.  |
| [atomic_get_dec_rel](compat.h.md#function-atomic_get_dec_rel) | Deprecated: See vatomic32_get_dec_rel.  |
| [atomic_dec_rlx](compat.h.md#function-atomic_dec_rlx) | Deprecated: See vatomic32_dec_get_rlx.  |
| [atomic_get_dec_rlx](compat.h.md#function-atomic_get_dec_rlx) | Deprecated: See vatomic32_get_dec_rlx.  |
| [atomic32_inc](compat.h.md#function-atomic32_inc) | Deprecated: See vatomic32_inc_get.  |
| [atomic32_get_inc](compat.h.md#function-atomic32_get_inc) | Deprecated: See vatomic32_get_inc.  |
| [atomic32_inc_acq](compat.h.md#function-atomic32_inc_acq) | Deprecated: See vatomic32_inc_get_acq.  |
| [atomic32_get_inc_acq](compat.h.md#function-atomic32_get_inc_acq) | Deprecated: See vatomic32_get_inc_acq.  |
| [atomic32_inc_rel](compat.h.md#function-atomic32_inc_rel) | Deprecated: See vatomic32_inc_get_rel.  |
| [atomic32_get_inc_rel](compat.h.md#function-atomic32_get_inc_rel) | Deprecated: See vatomic32_get_inc_rel.  |
| [atomic32_inc_rlx](compat.h.md#function-atomic32_inc_rlx) | Deprecated: See vatomic32_inc_get_rlx.  |
| [atomic32_get_inc_rlx](compat.h.md#function-atomic32_get_inc_rlx) | Deprecated: See vatomic32_get_inc_rlx.  |
| [atomic32_dec](compat.h.md#function-atomic32_dec) | Deprecated: See vatomic32_dec_get.  |
| [atomic32_get_dec](compat.h.md#function-atomic32_get_dec) | Deprecated: See vatomic32_get_dec.  |
| [atomic32_dec_acq](compat.h.md#function-atomic32_dec_acq) | Deprecated: See vatomic32_dec_get_acq.  |
| [atomic32_get_dec_acq](compat.h.md#function-atomic32_get_dec_acq) | Deprecated: See vatomic32_get_dec_acq.  |
| [atomic32_dec_rel](compat.h.md#function-atomic32_dec_rel) | Deprecated: See vatomic32_dec_get_rel.  |
| [atomic32_get_dec_rel](compat.h.md#function-atomic32_get_dec_rel) | Deprecated: See vatomic32_get_dec_rel.  |
| [atomic32_dec_rlx](compat.h.md#function-atomic32_dec_rlx) | Deprecated: See vatomic32_dec_get_rlx.  |
| [atomic32_get_dec_rlx](compat.h.md#function-atomic32_get_dec_rlx) | Deprecated: See vatomic32_get_dec_rlx.  |
| [atomic64_inc](compat.h.md#function-atomic64_inc) | Deprecated: See vatomic64_inc_get.  |
| [atomic64_get_inc](compat.h.md#function-atomic64_get_inc) | Deprecated: See vatomic64_get_inc.  |
| [atomic64_inc_acq](compat.h.md#function-atomic64_inc_acq) | Deprecated: See vatomic64_inc_get_acq.  |
| [atomic64_get_inc_acq](compat.h.md#function-atomic64_get_inc_acq) | Deprecated: See vatomic64_get_inc_acq.  |
| [atomic64_inc_rel](compat.h.md#function-atomic64_inc_rel) | Deprecated: See vatomic64_inc_get_rel.  |
| [atomic64_get_inc_rel](compat.h.md#function-atomic64_get_inc_rel) | Deprecated: See vatomic64_get_inc_rel.  |
| [atomic64_inc_rlx](compat.h.md#function-atomic64_inc_rlx) | Deprecated: See vatomic64_inc_get_rlx.  |
| [atomic64_get_inc_rlx](compat.h.md#function-atomic64_get_inc_rlx) | Deprecated: See vatomic64_get_inc_rlx.  |
| [atomic64_dec](compat.h.md#function-atomic64_dec) | Deprecated: See vatomic64_dec_get.  |
| [atomic64_get_dec](compat.h.md#function-atomic64_get_dec) | Deprecated: See vatomic64_get_dec.  |
| [atomic64_dec_acq](compat.h.md#function-atomic64_dec_acq) | Deprecated: See vatomic64_dec_get_acq.  |
| [atomic64_get_dec_acq](compat.h.md#function-atomic64_get_dec_acq) | Deprecated: See vatomic64_get_dec_acq.  |
| [atomic64_dec_rel](compat.h.md#function-atomic64_dec_rel) | Deprecated: See vatomic64_dec_get_rel.  |
| [atomic64_get_dec_rel](compat.h.md#function-atomic64_get_dec_rel) | Deprecated: See vatomic64_get_dec_rel.  |
| [atomic64_dec_rlx](compat.h.md#function-atomic64_dec_rlx) | Deprecated: See vatomic64_dec_get_rlx.  |
| [atomic64_get_dec_rlx](compat.h.md#function-atomic64_get_dec_rlx) | Deprecated: See vatomic64_get_dec_rlx.  |

##  Function `atomic_read`

```c
static vuint32_t V_DEPRECATED atomic_read(atomic_t *a)
``` 
_read/write/init functions_ 


Deprecated: See vatomic32_read 


##  Function `atomic_read_acq`

```c
static vuint32_t V_DEPRECATED atomic_read_acq(atomic_t *a)
``` 
_Deprecated: See vatomic32_read_acq._ 



##  Function `atomic_read_rlx`

```c
static vuint32_t V_DEPRECATED atomic_read_rlx(atomic_t *a)
``` 
_Deprecated: See vatomic32_read_rlx._ 



##  Function `atomic32_read`

```c
static vuint32_t V_DEPRECATED atomic32_read(atomic32_t *a)
``` 
_Deprecated: See vatomic32_read._ 



##  Function `atomic32_read_acq`

```c
static vuint32_t V_DEPRECATED atomic32_read_acq(atomic32_t *a)
``` 
_Deprecated: See vatomic32_read_acq._ 



##  Function `atomic32_read_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_read_rlx(atomic32_t *a)
``` 
_Deprecated: See vatomic32_read_rlx._ 



##  Function `atomic64_read`

```c
static vuint64_t V_DEPRECATED atomic64_read(atomic64_t *a)
``` 
_Deprecated: See vatomic64_read._ 



##  Function `atomic64_read_acq`

```c
static vuint64_t V_DEPRECATED atomic64_read_acq(atomic64_t *a)
``` 
_Deprecated: See vatomic64_read_acq._ 



##  Function `atomic64_read_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_read_rlx(atomic64_t *a)
``` 
_Deprecated: See vatomic64_read_rlx._ 



##  Function `atomicptr_read`

```c
static void* V_DEPRECATED atomicptr_read(atomicptr_t *a)
``` 
_Deprecated: See vatomicptr_read._ 



##  Function `atomicptr_read_acq`

```c
static void* V_DEPRECATED atomicptr_read_acq(atomicptr_t *a)
``` 
_Deprecated: See vatomicptr_read_acq._ 



##  Function `atomicptr_read_rlx`

```c
static void* V_DEPRECATED atomicptr_read_rlx(atomicptr_t *a)
``` 
_Deprecated: See vatomicptr_read_rlx._ 



##  Function `atomic_write`

```c
static void V_DEPRECATED atomic_write(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_write._ 



##  Function `atomic_write_rel`

```c
static void V_DEPRECATED atomic_write_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_write_rel._ 



##  Function `atomic_write_rlx`

```c
static void V_DEPRECATED atomic_write_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_write_rlx._ 



##  Function `atomic32_write`

```c
static void V_DEPRECATED atomic32_write(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_write._ 



##  Function `atomic32_write_rel`

```c
static void V_DEPRECATED atomic32_write_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_write_rel._ 



##  Function `atomic32_write_rlx`

```c
static void V_DEPRECATED atomic32_write_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_write_rlx._ 



##  Function `atomic64_write`

```c
static void V_DEPRECATED atomic64_write(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_write._ 



##  Function `atomic64_write_rel`

```c
static void V_DEPRECATED atomic64_write_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_write_rel._ 



##  Function `atomic64_write_rlx`

```c
static void V_DEPRECATED atomic64_write_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_write_rlx._ 



##  Function `atomicptr_write`

```c
static void V_DEPRECATED atomicptr_write(atomicptr_t *a, void *v)
``` 
_Deprecated: See vatomicptr_write._ 



##  Function `atomicptr_write_rel`

```c
static void V_DEPRECATED atomicptr_write_rel(atomicptr_t *a, void *v)
``` 
_Deprecated: See vatomicptr_write_rel._ 



##  Function `atomicptr_write_rlx`

```c
static void V_DEPRECATED atomicptr_write_rlx(atomicptr_t *a, void *v)
``` 
_Deprecated: See vatomicptr_write_rlx._ 



##  Function `atomic_init`

```c
static void V_DEPRECATED atomic_init(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_init._ 



##  Function `atomic32_init`

```c
static void V_DEPRECATED atomic32_init(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_init._ 



##  Function `atomic64_init`

```c
static void V_DEPRECATED atomic64_init(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_init._ 



##  Function `atomicptr_init`

```c
static void V_DEPRECATED atomicptr_init(atomicptr_t *a, void *v)
``` 
_Deprecated: See vatomicptr_init._ 



##  Function `atomic_xchg`

```c
static vuint32_t V_DEPRECATED atomic_xchg(atomic_t *a, vuint32_t v)
``` 
_cmpxchg/xchg functions_ 


Deprecated: See vatomic32_xchg 


##  Function `atomic_cmpxchg`

```c
static vuint32_t V_DEPRECATED atomic_cmpxchg(atomic_t *a, vuint32_t c, vuint32_t v)
``` 
_Deprecated: See vatomic32_cmpxchg._ 



##  Function `atomic_xchg_acq`

```c
static vuint32_t V_DEPRECATED atomic_xchg_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xchg_acq._ 



##  Function `atomic_cmpxchg_acq`

```c
static vuint32_t V_DEPRECATED atomic_cmpxchg_acq(atomic_t *a, vuint32_t c, vuint32_t v)
``` 
_Deprecated: See vatomic32_cmpxchg_acq._ 



##  Function `atomic_xchg_rel`

```c
static vuint32_t V_DEPRECATED atomic_xchg_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xchg_rel._ 



##  Function `atomic_cmpxchg_rel`

```c
static vuint32_t V_DEPRECATED atomic_cmpxchg_rel(atomic_t *a, vuint32_t c, vuint32_t v)
``` 
_Deprecated: See vatomic32_cmpxchg_rel._ 



##  Function `atomic_xchg_rlx`

```c
static vuint32_t V_DEPRECATED atomic_xchg_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xchg_rlx._ 



##  Function `atomic_cmpxchg_rlx`

```c
static vuint32_t V_DEPRECATED atomic_cmpxchg_rlx(atomic_t *a, vuint32_t c, vuint32_t v)
``` 
_Deprecated: See vatomic32_cmpxchg_rlx._ 



##  Function `atomic32_xchg`

```c
static vuint32_t V_DEPRECATED atomic32_xchg(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xchg._ 



##  Function `atomic32_cmpxchg`

```c
static vuint32_t V_DEPRECATED atomic32_cmpxchg(atomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Deprecated: See vatomic32_cmpxchg._ 



##  Function `atomic32_xchg_acq`

```c
static vuint32_t V_DEPRECATED atomic32_xchg_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xchg_acq._ 



##  Function `atomic32_cmpxchg_acq`

```c
static vuint32_t V_DEPRECATED atomic32_cmpxchg_acq(atomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Deprecated: See vatomic32_cmpxchg_acq._ 



##  Function `atomic32_xchg_rel`

```c
static vuint32_t V_DEPRECATED atomic32_xchg_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xchg_rel._ 



##  Function `atomic32_cmpxchg_rel`

```c
static vuint32_t V_DEPRECATED atomic32_cmpxchg_rel(atomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Deprecated: See vatomic32_cmpxchg_rel._ 



##  Function `atomic32_xchg_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_xchg_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xchg_rlx._ 



##  Function `atomic32_cmpxchg_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_cmpxchg_rlx(atomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Deprecated: See vatomic32_cmpxchg_rlx._ 



##  Function `atomic64_xchg`

```c
static vuint64_t V_DEPRECATED atomic64_xchg(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_xchg._ 



##  Function `atomic64_cmpxchg`

```c
static vuint64_t V_DEPRECATED atomic64_cmpxchg(atomic64_t *a, vuint64_t c, vuint64_t v)
``` 
_Deprecated: See vatomic64_cmpxchg._ 



##  Function `atomic64_xchg_acq`

```c
static vuint64_t V_DEPRECATED atomic64_xchg_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_xchg_acq._ 



##  Function `atomic64_cmpxchg_acq`

```c
static vuint64_t V_DEPRECATED atomic64_cmpxchg_acq(atomic64_t *a, vuint64_t c, vuint64_t v)
``` 
_Deprecated: See vatomic64_cmpxchg_acq._ 



##  Function `atomic64_xchg_rel`

```c
static vuint64_t V_DEPRECATED atomic64_xchg_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_xchg_rel._ 



##  Function `atomic64_cmpxchg_rel`

```c
static vuint64_t V_DEPRECATED atomic64_cmpxchg_rel(atomic64_t *a, vuint64_t c, vuint64_t v)
``` 
_Deprecated: See vatomic64_cmpxchg_rel._ 



##  Function `atomic64_xchg_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_xchg_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_xchg_rlx._ 



##  Function `atomic64_cmpxchg_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_cmpxchg_rlx(atomic64_t *a, vuint64_t c, vuint64_t v)
``` 
_Deprecated: See vatomic64_cmpxchg_rlx._ 



##  Function `atomicptr_xchg`

```c
static void* V_DEPRECATED atomicptr_xchg(atomicptr_t *a, void *v)
``` 
_Deprecated: See vatomicptr_xchg._ 



##  Function `atomicptr_cmpxchg`

```c
static void* V_DEPRECATED atomicptr_cmpxchg(atomicptr_t *a, void *c, void *v)
``` 
_Deprecated: See vatomicptr_cmpxchg._ 



##  Function `atomicptr_xchg_acq`

```c
static void* V_DEPRECATED atomicptr_xchg_acq(atomicptr_t *a, void *v)
``` 
_Deprecated: See vatomicptr_xchg_acq._ 



##  Function `atomicptr_cmpxchg_acq`

```c
static void* V_DEPRECATED atomicptr_cmpxchg_acq(atomicptr_t *a, void *c, void *v)
``` 
_Deprecated: See vatomicptr_cmpxchg_acq._ 



##  Function `atomicptr_xchg_rel`

```c
static void* V_DEPRECATED atomicptr_xchg_rel(atomicptr_t *a, void *v)
``` 
_Deprecated: See vatomicptr_xchg_rel._ 



##  Function `atomicptr_cmpxchg_rel`

```c
static void* V_DEPRECATED atomicptr_cmpxchg_rel(atomicptr_t *a, void *c, void *v)
``` 
_Deprecated: See vatomicptr_cmpxchg_rel._ 



##  Function `atomicptr_xchg_rlx`

```c
static void* V_DEPRECATED atomicptr_xchg_rlx(atomicptr_t *a, void *v)
``` 
_Deprecated: See vatomicptr_xchg_rlx._ 



##  Function `atomicptr_cmpxchg_rlx`

```c
static void* V_DEPRECATED atomicptr_cmpxchg_rlx(atomicptr_t *a, void *c, void *v)
``` 
_Deprecated: See vatomicptr_cmpxchg_rlx._ 



##  Function `atomic_add`

```c
static vuint32_t V_DEPRECATED atomic_add(atomic_t *a, vuint32_t v)
``` 
_add, sub, or, and, xor_ 


Deprecated: See vatomic32_add_get 


##  Function `atomic_get_add`

```c
static vuint32_t V_DEPRECATED atomic_get_add(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_add._ 



##  Function `atomic_add_acq`

```c
static vuint32_t V_DEPRECATED atomic_add_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_add_get_acq._ 



##  Function `atomic_get_add_acq`

```c
static vuint32_t V_DEPRECATED atomic_get_add_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_add_acq._ 



##  Function `atomic_add_rel`

```c
static vuint32_t V_DEPRECATED atomic_add_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_add_get_rel._ 



##  Function `atomic_get_add_rel`

```c
static vuint32_t V_DEPRECATED atomic_get_add_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_add_rel._ 



##  Function `atomic_add_rlx`

```c
static vuint32_t V_DEPRECATED atomic_add_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_add_get_rlx._ 



##  Function `atomic_get_add_rlx`

```c
static vuint32_t V_DEPRECATED atomic_get_add_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_add_rlx._ 



##  Function `atomic_sub`

```c
static vuint32_t V_DEPRECATED atomic_sub(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_sub_get._ 



##  Function `atomic_get_sub`

```c
static vuint32_t V_DEPRECATED atomic_get_sub(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_sub._ 



##  Function `atomic_sub_acq`

```c
static vuint32_t V_DEPRECATED atomic_sub_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_sub_get_acq._ 



##  Function `atomic_get_sub_acq`

```c
static vuint32_t V_DEPRECATED atomic_get_sub_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_sub_acq._ 



##  Function `atomic_sub_rel`

```c
static vuint32_t V_DEPRECATED atomic_sub_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_sub_get_rel._ 



##  Function `atomic_get_sub_rel`

```c
static vuint32_t V_DEPRECATED atomic_get_sub_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_sub_rel._ 



##  Function `atomic_sub_rlx`

```c
static vuint32_t V_DEPRECATED atomic_sub_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_sub_get_rlx._ 



##  Function `atomic_get_sub_rlx`

```c
static vuint32_t V_DEPRECATED atomic_get_sub_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_sub_rlx._ 



##  Function `atomic_xor`

```c
static vuint32_t V_DEPRECATED atomic_xor(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xor_get._ 



##  Function `atomic_get_xor`

```c
static vuint32_t V_DEPRECATED atomic_get_xor(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_xor._ 



##  Function `atomic_xor_acq`

```c
static vuint32_t V_DEPRECATED atomic_xor_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xor_get_acq._ 



##  Function `atomic_get_xor_acq`

```c
static vuint32_t V_DEPRECATED atomic_get_xor_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_xor_acq._ 



##  Function `atomic_xor_rel`

```c
static vuint32_t V_DEPRECATED atomic_xor_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xor_get_rel._ 



##  Function `atomic_get_xor_rel`

```c
static vuint32_t V_DEPRECATED atomic_get_xor_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_xor_rel._ 



##  Function `atomic_xor_rlx`

```c
static vuint32_t V_DEPRECATED atomic_xor_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xor_get_rlx._ 



##  Function `atomic_get_xor_rlx`

```c
static vuint32_t V_DEPRECATED atomic_get_xor_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_xor_rlx._ 



##  Function `atomic_and`

```c
static vuint32_t V_DEPRECATED atomic_and(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_and_get._ 



##  Function `atomic_get_and`

```c
static vuint32_t V_DEPRECATED atomic_get_and(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_and._ 



##  Function `atomic_and_acq`

```c
static vuint32_t V_DEPRECATED atomic_and_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_and_get_acq._ 



##  Function `atomic_get_and_acq`

```c
static vuint32_t V_DEPRECATED atomic_get_and_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_and_acq._ 



##  Function `atomic_and_rel`

```c
static vuint32_t V_DEPRECATED atomic_and_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_and_get_rel._ 



##  Function `atomic_get_and_rel`

```c
static vuint32_t V_DEPRECATED atomic_get_and_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_and_rel._ 



##  Function `atomic_and_rlx`

```c
static vuint32_t V_DEPRECATED atomic_and_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_and_get_rlx._ 



##  Function `atomic_get_and_rlx`

```c
static vuint32_t V_DEPRECATED atomic_get_and_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_and_rlx._ 



##  Function `atomic_or`

```c
static vuint32_t V_DEPRECATED atomic_or(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_or_get._ 



##  Function `atomic_get_or`

```c
static vuint32_t V_DEPRECATED atomic_get_or(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_or._ 



##  Function `atomic_or_acq`

```c
static vuint32_t V_DEPRECATED atomic_or_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_or_get_acq._ 



##  Function `atomic_get_or_acq`

```c
static vuint32_t V_DEPRECATED atomic_get_or_acq(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_or_acq._ 



##  Function `atomic_or_rel`

```c
static vuint32_t V_DEPRECATED atomic_or_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_or_get_rel._ 



##  Function `atomic_get_or_rel`

```c
static vuint32_t V_DEPRECATED atomic_get_or_rel(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_or_rel._ 



##  Function `atomic_or_rlx`

```c
static vuint32_t V_DEPRECATED atomic_or_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_or_get_rlx._ 



##  Function `atomic_get_or_rlx`

```c
static vuint32_t V_DEPRECATED atomic_get_or_rlx(atomic_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_or_rlx._ 



##  Function `atomic32_add`

```c
static vuint32_t V_DEPRECATED atomic32_add(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_add_get._ 



##  Function `atomic32_get_add`

```c
static vuint32_t V_DEPRECATED atomic32_get_add(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_add._ 



##  Function `atomic32_add_acq`

```c
static vuint32_t V_DEPRECATED atomic32_add_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_add_get_acq._ 



##  Function `atomic32_get_add_acq`

```c
static vuint32_t V_DEPRECATED atomic32_get_add_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_add_acq._ 



##  Function `atomic32_add_rel`

```c
static vuint32_t V_DEPRECATED atomic32_add_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_add_get_rel._ 



##  Function `atomic32_get_add_rel`

```c
static vuint32_t V_DEPRECATED atomic32_get_add_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_add_rel._ 



##  Function `atomic32_add_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_add_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_add_get_rlx._ 



##  Function `atomic32_get_add_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_get_add_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_add_rlx._ 



##  Function `atomic32_sub`

```c
static vuint32_t V_DEPRECATED atomic32_sub(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_sub_get._ 



##  Function `atomic32_get_sub`

```c
static vuint32_t V_DEPRECATED atomic32_get_sub(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_sub._ 



##  Function `atomic32_sub_acq`

```c
static vuint32_t V_DEPRECATED atomic32_sub_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_sub_get_acq._ 



##  Function `atomic32_get_sub_acq`

```c
static vuint32_t V_DEPRECATED atomic32_get_sub_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_sub_acq._ 



##  Function `atomic32_sub_rel`

```c
static vuint32_t V_DEPRECATED atomic32_sub_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_sub_get_rel._ 



##  Function `atomic32_get_sub_rel`

```c
static vuint32_t V_DEPRECATED atomic32_get_sub_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_sub_rel._ 



##  Function `atomic32_sub_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_sub_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_sub_get_rlx._ 



##  Function `atomic32_get_sub_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_get_sub_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_sub_rlx._ 



##  Function `atomic32_xor`

```c
static vuint32_t V_DEPRECATED atomic32_xor(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xor_get._ 



##  Function `atomic32_get_xor`

```c
static vuint32_t V_DEPRECATED atomic32_get_xor(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_xor._ 



##  Function `atomic32_xor_acq`

```c
static vuint32_t V_DEPRECATED atomic32_xor_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xor_get_acq._ 



##  Function `atomic32_get_xor_acq`

```c
static vuint32_t V_DEPRECATED atomic32_get_xor_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_xor_acq._ 



##  Function `atomic32_xor_rel`

```c
static vuint32_t V_DEPRECATED atomic32_xor_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xor_get_rel._ 



##  Function `atomic32_get_xor_rel`

```c
static vuint32_t V_DEPRECATED atomic32_get_xor_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_xor_rel._ 



##  Function `atomic32_xor_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_xor_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_xor_get_rlx._ 



##  Function `atomic32_get_xor_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_get_xor_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_xor_rlx._ 



##  Function `atomic32_and`

```c
static vuint32_t V_DEPRECATED atomic32_and(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_and_get._ 



##  Function `atomic32_get_and`

```c
static vuint32_t V_DEPRECATED atomic32_get_and(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_and._ 



##  Function `atomic32_and_acq`

```c
static vuint32_t V_DEPRECATED atomic32_and_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_and_get_acq._ 



##  Function `atomic32_get_and_acq`

```c
static vuint32_t V_DEPRECATED atomic32_get_and_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_and_acq._ 



##  Function `atomic32_and_rel`

```c
static vuint32_t V_DEPRECATED atomic32_and_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_and_get_rel._ 



##  Function `atomic32_get_and_rel`

```c
static vuint32_t V_DEPRECATED atomic32_get_and_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_and_rel._ 



##  Function `atomic32_and_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_and_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_and_get_rlx._ 



##  Function `atomic32_get_and_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_get_and_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_and_rlx._ 



##  Function `atomic32_or`

```c
static vuint32_t V_DEPRECATED atomic32_or(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_or_get._ 



##  Function `atomic32_get_or`

```c
static vuint32_t V_DEPRECATED atomic32_get_or(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_or._ 



##  Function `atomic32_or_acq`

```c
static vuint32_t V_DEPRECATED atomic32_or_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_or_get_acq._ 



##  Function `atomic32_get_or_acq`

```c
static vuint32_t V_DEPRECATED atomic32_get_or_acq(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_or_acq._ 



##  Function `atomic32_or_rel`

```c
static vuint32_t V_DEPRECATED atomic32_or_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_or_get_rel._ 



##  Function `atomic32_get_or_rel`

```c
static vuint32_t V_DEPRECATED atomic32_get_or_rel(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_or_rel._ 



##  Function `atomic32_or_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_or_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_or_get_rlx._ 



##  Function `atomic32_get_or_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_get_or_rlx(atomic32_t *a, vuint32_t v)
``` 
_Deprecated: See vatomic32_get_or_rlx._ 



##  Function `atomic64_add`

```c
static vuint64_t V_DEPRECATED atomic64_add(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_add_get._ 



##  Function `atomic64_get_add`

```c
static vuint64_t V_DEPRECATED atomic64_get_add(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_add._ 



##  Function `atomic64_add_acq`

```c
static vuint64_t V_DEPRECATED atomic64_add_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_add_get_acq._ 



##  Function `atomic64_get_add_acq`

```c
static vuint64_t V_DEPRECATED atomic64_get_add_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_add_acq._ 



##  Function `atomic64_add_rel`

```c
static vuint64_t V_DEPRECATED atomic64_add_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_add_get_rel._ 



##  Function `atomic64_get_add_rel`

```c
static vuint64_t V_DEPRECATED atomic64_get_add_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_add_rel._ 



##  Function `atomic64_add_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_add_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_add_get_rlx._ 



##  Function `atomic64_get_add_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_get_add_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_add_rlx._ 



##  Function `atomic64_sub`

```c
static vuint64_t V_DEPRECATED atomic64_sub(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_sub_get._ 



##  Function `atomic64_get_sub`

```c
static vuint64_t V_DEPRECATED atomic64_get_sub(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_sub._ 



##  Function `atomic64_sub_acq`

```c
static vuint64_t V_DEPRECATED atomic64_sub_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_sub_get_acq._ 



##  Function `atomic64_get_sub_acq`

```c
static vuint64_t V_DEPRECATED atomic64_get_sub_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_sub_acq._ 



##  Function `atomic64_sub_rel`

```c
static vuint64_t V_DEPRECATED atomic64_sub_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_sub_get_rel._ 



##  Function `atomic64_get_sub_rel`

```c
static vuint64_t V_DEPRECATED atomic64_get_sub_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_sub_rel._ 



##  Function `atomic64_sub_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_sub_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_sub_get_rlx._ 



##  Function `atomic64_get_sub_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_get_sub_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_sub_rlx._ 



##  Function `atomic64_xor`

```c
static vuint64_t V_DEPRECATED atomic64_xor(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_xor_get._ 



##  Function `atomic64_get_xor`

```c
static vuint64_t V_DEPRECATED atomic64_get_xor(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_xor._ 



##  Function `atomic64_xor_acq`

```c
static vuint64_t V_DEPRECATED atomic64_xor_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_xor_get_acq._ 



##  Function `atomic64_get_xor_acq`

```c
static vuint64_t V_DEPRECATED atomic64_get_xor_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_xor_acq._ 



##  Function `atomic64_xor_rel`

```c
static vuint64_t V_DEPRECATED atomic64_xor_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_xor_get_rel._ 



##  Function `atomic64_get_xor_rel`

```c
static vuint64_t V_DEPRECATED atomic64_get_xor_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_xor_rel._ 



##  Function `atomic64_xor_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_xor_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_xor_get_rlx._ 



##  Function `atomic64_get_xor_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_get_xor_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_xor_rlx._ 



##  Function `atomic64_and`

```c
static vuint64_t V_DEPRECATED atomic64_and(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_and_get._ 



##  Function `atomic64_get_and`

```c
static vuint64_t V_DEPRECATED atomic64_get_and(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_and._ 



##  Function `atomic64_and_acq`

```c
static vuint64_t V_DEPRECATED atomic64_and_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_and_get_acq._ 



##  Function `atomic64_get_and_acq`

```c
static vuint64_t V_DEPRECATED atomic64_get_and_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_and_acq._ 



##  Function `atomic64_and_rel`

```c
static vuint64_t V_DEPRECATED atomic64_and_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_and_get_rel._ 



##  Function `atomic64_get_and_rel`

```c
static vuint64_t V_DEPRECATED atomic64_get_and_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_and_rel._ 



##  Function `atomic64_and_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_and_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_and_get_rlx._ 



##  Function `atomic64_get_and_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_get_and_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_and_rlx._ 



##  Function `atomic64_or`

```c
static vuint64_t V_DEPRECATED atomic64_or(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_or_get._ 



##  Function `atomic64_get_or`

```c
static vuint64_t V_DEPRECATED atomic64_get_or(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_or._ 



##  Function `atomic64_or_acq`

```c
static vuint64_t V_DEPRECATED atomic64_or_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_or_get_acq._ 



##  Function `atomic64_get_or_acq`

```c
static vuint64_t V_DEPRECATED atomic64_get_or_acq(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_or_acq._ 



##  Function `atomic64_or_rel`

```c
static vuint64_t V_DEPRECATED atomic64_or_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_or_get_rel._ 



##  Function `atomic64_get_or_rel`

```c
static vuint64_t V_DEPRECATED atomic64_get_or_rel(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_or_rel._ 



##  Function `atomic64_or_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_or_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_or_get_rlx._ 



##  Function `atomic64_get_or_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_get_or_rlx(atomic64_t *a, vuint64_t v)
``` 
_Deprecated: See vatomic64_get_or_rlx._ 



##  Function `atomic_inc`

```c
static vuint32_t V_DEPRECATED atomic_inc(atomic_t *a)
``` 
_increment/decrement_ 


Deprecated: See vatomic32_inc_get 


##  Function `atomic_get_inc`

```c
static vuint32_t V_DEPRECATED atomic_get_inc(atomic_t *a)
``` 
_Deprecated: See vatomic32_get_inc._ 



##  Function `atomic_inc_acq`

```c
static vuint32_t V_DEPRECATED atomic_inc_acq(atomic_t *a)
``` 
_Deprecated: See vatomic32_inc_get_acq._ 



##  Function `atomic_get_inc_acq`

```c
static vuint32_t V_DEPRECATED atomic_get_inc_acq(atomic_t *a)
``` 
_Deprecated: See vatomic32_get_inc_acq._ 



##  Function `atomic_inc_rel`

```c
static vuint32_t V_DEPRECATED atomic_inc_rel(atomic_t *a)
``` 
_Deprecated: See vatomic32_inc_get_rel._ 



##  Function `atomic_get_inc_rel`

```c
static vuint32_t V_DEPRECATED atomic_get_inc_rel(atomic_t *a)
``` 
_Deprecated: See vatomic32_get_inc_rel._ 



##  Function `atomic_inc_rlx`

```c
static vuint32_t V_DEPRECATED atomic_inc_rlx(atomic_t *a)
``` 
_Deprecated: See vatomic32_inc_get_rlx._ 



##  Function `atomic_get_inc_rlx`

```c
static vuint32_t V_DEPRECATED atomic_get_inc_rlx(atomic_t *a)
``` 
_Deprecated: See vatomic32_get_inc_rlx._ 



##  Function `atomic_dec`

```c
static vuint32_t V_DEPRECATED atomic_dec(atomic_t *a)
``` 
_Deprecated: See vatomic32_dec_get._ 



##  Function `atomic_get_dec`

```c
static vuint32_t V_DEPRECATED atomic_get_dec(atomic_t *a)
``` 
_Deprecated: See vatomic32_get_dec._ 



##  Function `atomic_dec_acq`

```c
static vuint32_t V_DEPRECATED atomic_dec_acq(atomic_t *a)
``` 
_Deprecated: See vatomic32_dec_get_acq._ 



##  Function `atomic_get_dec_acq`

```c
static vuint32_t V_DEPRECATED atomic_get_dec_acq(atomic_t *a)
``` 
_Deprecated: See vatomic32_get_dec_acq._ 



##  Function `atomic_dec_rel`

```c
static vuint32_t V_DEPRECATED atomic_dec_rel(atomic_t *a)
``` 
_Deprecated: See vatomic32_dec_get_rel._ 



##  Function `atomic_get_dec_rel`

```c
static vuint32_t V_DEPRECATED atomic_get_dec_rel(atomic_t *a)
``` 
_Deprecated: See vatomic32_get_dec_rel._ 



##  Function `atomic_dec_rlx`

```c
static vuint32_t V_DEPRECATED atomic_dec_rlx(atomic_t *a)
``` 
_Deprecated: See vatomic32_dec_get_rlx._ 



##  Function `atomic_get_dec_rlx`

```c
static vuint32_t V_DEPRECATED atomic_get_dec_rlx(atomic_t *a)
``` 
_Deprecated: See vatomic32_get_dec_rlx._ 



##  Function `atomic32_inc`

```c
static vuint32_t V_DEPRECATED atomic32_inc(atomic32_t *a)
``` 
_Deprecated: See vatomic32_inc_get._ 



##  Function `atomic32_get_inc`

```c
static vuint32_t V_DEPRECATED atomic32_get_inc(atomic32_t *a)
``` 
_Deprecated: See vatomic32_get_inc._ 



##  Function `atomic32_inc_acq`

```c
static vuint32_t V_DEPRECATED atomic32_inc_acq(atomic32_t *a)
``` 
_Deprecated: See vatomic32_inc_get_acq._ 



##  Function `atomic32_get_inc_acq`

```c
static vuint32_t V_DEPRECATED atomic32_get_inc_acq(atomic32_t *a)
``` 
_Deprecated: See vatomic32_get_inc_acq._ 



##  Function `atomic32_inc_rel`

```c
static vuint32_t V_DEPRECATED atomic32_inc_rel(atomic32_t *a)
``` 
_Deprecated: See vatomic32_inc_get_rel._ 



##  Function `atomic32_get_inc_rel`

```c
static vuint32_t V_DEPRECATED atomic32_get_inc_rel(atomic32_t *a)
``` 
_Deprecated: See vatomic32_get_inc_rel._ 



##  Function `atomic32_inc_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_inc_rlx(atomic32_t *a)
``` 
_Deprecated: See vatomic32_inc_get_rlx._ 



##  Function `atomic32_get_inc_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_get_inc_rlx(atomic32_t *a)
``` 
_Deprecated: See vatomic32_get_inc_rlx._ 



##  Function `atomic32_dec`

```c
static vuint32_t V_DEPRECATED atomic32_dec(atomic32_t *a)
``` 
_Deprecated: See vatomic32_dec_get._ 



##  Function `atomic32_get_dec`

```c
static vuint32_t V_DEPRECATED atomic32_get_dec(atomic32_t *a)
``` 
_Deprecated: See vatomic32_get_dec._ 



##  Function `atomic32_dec_acq`

```c
static vuint32_t V_DEPRECATED atomic32_dec_acq(atomic32_t *a)
``` 
_Deprecated: See vatomic32_dec_get_acq._ 



##  Function `atomic32_get_dec_acq`

```c
static vuint32_t V_DEPRECATED atomic32_get_dec_acq(atomic32_t *a)
``` 
_Deprecated: See vatomic32_get_dec_acq._ 



##  Function `atomic32_dec_rel`

```c
static vuint32_t V_DEPRECATED atomic32_dec_rel(atomic32_t *a)
``` 
_Deprecated: See vatomic32_dec_get_rel._ 



##  Function `atomic32_get_dec_rel`

```c
static vuint32_t V_DEPRECATED atomic32_get_dec_rel(atomic32_t *a)
``` 
_Deprecated: See vatomic32_get_dec_rel._ 



##  Function `atomic32_dec_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_dec_rlx(atomic32_t *a)
``` 
_Deprecated: See vatomic32_dec_get_rlx._ 



##  Function `atomic32_get_dec_rlx`

```c
static vuint32_t V_DEPRECATED atomic32_get_dec_rlx(atomic32_t *a)
``` 
_Deprecated: See vatomic32_get_dec_rlx._ 



##  Function `atomic64_inc`

```c
static vuint64_t V_DEPRECATED atomic64_inc(atomic64_t *a)
``` 
_Deprecated: See vatomic64_inc_get._ 



##  Function `atomic64_get_inc`

```c
static vuint64_t V_DEPRECATED atomic64_get_inc(atomic64_t *a)
``` 
_Deprecated: See vatomic64_get_inc._ 



##  Function `atomic64_inc_acq`

```c
static vuint64_t V_DEPRECATED atomic64_inc_acq(atomic64_t *a)
``` 
_Deprecated: See vatomic64_inc_get_acq._ 



##  Function `atomic64_get_inc_acq`

```c
static vuint64_t V_DEPRECATED atomic64_get_inc_acq(atomic64_t *a)
``` 
_Deprecated: See vatomic64_get_inc_acq._ 



##  Function `atomic64_inc_rel`

```c
static vuint64_t V_DEPRECATED atomic64_inc_rel(atomic64_t *a)
``` 
_Deprecated: See vatomic64_inc_get_rel._ 



##  Function `atomic64_get_inc_rel`

```c
static vuint64_t V_DEPRECATED atomic64_get_inc_rel(atomic64_t *a)
``` 
_Deprecated: See vatomic64_get_inc_rel._ 



##  Function `atomic64_inc_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_inc_rlx(atomic64_t *a)
``` 
_Deprecated: See vatomic64_inc_get_rlx._ 



##  Function `atomic64_get_inc_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_get_inc_rlx(atomic64_t *a)
``` 
_Deprecated: See vatomic64_get_inc_rlx._ 



##  Function `atomic64_dec`

```c
static vuint64_t V_DEPRECATED atomic64_dec(atomic64_t *a)
``` 
_Deprecated: See vatomic64_dec_get._ 



##  Function `atomic64_get_dec`

```c
static vuint64_t V_DEPRECATED atomic64_get_dec(atomic64_t *a)
``` 
_Deprecated: See vatomic64_get_dec._ 



##  Function `atomic64_dec_acq`

```c
static vuint64_t V_DEPRECATED atomic64_dec_acq(atomic64_t *a)
``` 
_Deprecated: See vatomic64_dec_get_acq._ 



##  Function `atomic64_get_dec_acq`

```c
static vuint64_t V_DEPRECATED atomic64_get_dec_acq(atomic64_t *a)
``` 
_Deprecated: See vatomic64_get_dec_acq._ 



##  Function `atomic64_dec_rel`

```c
static vuint64_t V_DEPRECATED atomic64_dec_rel(atomic64_t *a)
``` 
_Deprecated: See vatomic64_dec_get_rel._ 



##  Function `atomic64_get_dec_rel`

```c
static vuint64_t V_DEPRECATED atomic64_get_dec_rel(atomic64_t *a)
``` 
_Deprecated: See vatomic64_get_dec_rel._ 



##  Function `atomic64_dec_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_dec_rlx(atomic64_t *a)
``` 
_Deprecated: See vatomic64_dec_get_rlx._ 



##  Function `atomic64_get_dec_rlx`

```c
static vuint64_t V_DEPRECATED atomic64_get_dec_rlx(atomic64_t *a)
``` 
_Deprecated: See vatomic64_get_dec_rlx._ 




---
