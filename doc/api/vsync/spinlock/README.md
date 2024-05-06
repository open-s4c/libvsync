#  [vsync](../README.md) / spinlock
_Spinlocks for kernel and userspace._ 

---
## File Index


| File|Description|Numa-aware|
| --- | --- | --- |
| [vsync/spinlock/arraylock.h](arraylock.h.md)|Simple array-based queue lock. | &#x274C; |
| [vsync/spinlock/caslock.h](caslock.h.md)|Simple spinlock based on compare-and-swap (CAS). | &#x274C; |
| [vsync/spinlock/clhlock.h](clhlock.h.md)|List-based lock attributed to Craig, Landin and Hagersten. | &#x274C; |
| [vsync/spinlock/cnalock.h](cnalock.h.md)|Compact NUMA-aware Lock. | &#x2714; |
| [vsync/spinlock/hclhlock.h](hclhlock.h.md)|Hierarchical CLH Queue Lock. | &#x2714; |
| [vsync/spinlock/hemlock.h](hemlock.h.md)|Hemlock by Dice and Kogan. | &#x274C; |
| [vsync/spinlock/hmcslock.h](hmcslock.h.md)|Hierarchical MCS lock for systems with NUMA Hierarchies. | &#x2714; |
| [vsync/spinlock/mcslock.h](mcslock.h.md)|Mellor-Crummey Scott Lock - the well-known scalable lock. | &#x274C; |
| [vsync/spinlock/rec_mcslock.h](rec_mcslock.h.md)|Recursive MCS lock implementation using recursive.h. | &#x274C; |
| [vsync/spinlock/rec_seqlock.h](rec_seqlock.h.md)|Recursive seqlock implementation using recursive.h. | &#x274C; |
| [vsync/spinlock/rec_spinlock.h](rec_spinlock.h.md)|Recursive spinlock implementation using recursive.h. | &#x274C; |
| [vsync/spinlock/rec_ticketlock.h](rec_ticketlock.h.md)|Recursive ticketlock implementation using recursive.h. | &#x274C; |
| [vsync/spinlock/rwlock.h](rwlock.h.md)|Write-preferring rwlock based on semaphore and an atomic vbool_t. | &#x274C; |
| [vsync/spinlock/semaphore.h](semaphore.h.md)|Counting Semaphore. | &#x274C; |
| [vsync/spinlock/seqcount.h](seqcount.h.md)|Lightweight single-writer multi-reader optimistic lock. | &#x274C; |
| [vsync/spinlock/seqlock.h](seqlock.h.md)|Multi-writer multi-reader optimistic lock. | &#x274C; |
| [vsync/spinlock/ticketlock.h](ticketlock.h.md)|Classic ticketlock with two 32-bit variables (owner and next). | &#x274C; |
| [vsync/spinlock/ttaslock.h](ttaslock.h.md)|Test, Test and Set lock. | &#x274C; |
| [vsync/spinlock/twalock.h](twalock.h.md)|Ticketlock with waiting array (TWA). | &#x274C; |


---
