#  [vsync](../README.md) / spinlock
_Spinlocks for kernel and userspace._ 

---
## File Index


| File|Description|
| --- | --- |
| [vsync/spinlock/caslock.h](caslock.h.md)|Simple spinlock based on compare-and-swap (CAS). |
| [vsync/spinlock/mcslock.h](mcslock.h.md)|Mellor-Crummey Scott Lock - the well-known scalable lock. |
| [vsync/spinlock/rec_mcslock.h](rec_mcslock.h.md)|Recursive MCS lock implementation using recursive.h. |
| [vsync/spinlock/rec_spinlock.h](rec_spinlock.h.md)|Recursive spinlock implementation using recursive.h. |
| [vsync/spinlock/rec_ticketlock.h](rec_ticketlock.h.md)|Recursive ticketlock implementation using recursive.h. |
| [vsync/spinlock/rwlock.h](rwlock.h.md)|Write-preferring rwlock based on semaphore and an atomic vbool_t. |
| [vsync/spinlock/semaphore.h](semaphore.h.md)|Counting Semaphore. |
| [vsync/spinlock/seqcount.h](seqcount.h.md)|Lightweight single-writer multi-reader optimistic lock. |
| [vsync/spinlock/seqlock.h](seqlock.h.md)|Multi-writer multi-reader optimistic lock. |
| [vsync/spinlock/ticketlock.h](ticketlock.h.md)|Classic ticketlock with two 32-bit variables (owner and next). |
| [vsync/spinlock/ttaslock.h](ttaslock.h.md)|Test, Test and Set lock. |


---
