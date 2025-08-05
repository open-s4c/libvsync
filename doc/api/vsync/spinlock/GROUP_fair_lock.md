#  Fair locks
_Group of starvation-free locks (fair locks)._ 

---
## File Index


| File|Description|Fair locks|Numa-aware|Reentrant|
| --- | --- | --- | --- | --- |
| [vsync/spinlock/arraylock.h](arraylock.h.md)|Simple array-based queue lock. | &#x2714; | &#x274C; | &#x274C; |
| [vsync/spinlock/clhlock.h](clhlock.h.md)|List-based lock attributed to Craig, Landin and Hagersten. | &#x2714; | &#x274C; | &#x274C; |
| [vsync/spinlock/cnalock.h](cnalock.h.md)|Compact NUMA-aware Lock. | &#x2714; | &#x2714; | &#x274C; |
| [vsync/spinlock/hclhlock.h](hclhlock.h.md)|Hierarchical CLH Queue Lock. | &#x2714; | &#x2714; | &#x274C; |
| [vsync/spinlock/hemlock.h](hemlock.h.md)|Hemlock by Dice and Kogan. | &#x2714; | &#x274C; | &#x274C; |
| [vsync/spinlock/hmcslock.h](hmcslock.h.md)|Hierarchical MCS lock for systems with NUMA Hierarchies. | &#x2714; | &#x2714; | &#x274C; |
| [vsync/spinlock/mcslock.h](mcslock.h.md)|Mellor-Crummey Scott Lock - the well-known scalable lock. | &#x2714; | &#x274C; | &#x274C; |
| [vsync/spinlock/rec_mcslock.h](rec_mcslock.h.md)|Recursive MCS lock implementation using recursive.h. | &#x2714; | &#x274C; | &#x274C; |
| [vsync/spinlock/rec_ticketlock.h](rec_ticketlock.h.md)|Recursive ticketlock implementation using recursive.h. | &#x2714; | &#x274C; | &#x2714; |
| [vsync/spinlock/ticketlock.h](ticketlock.h.md)|Classic ticketlock with two 32-bit variables (owner and next). | &#x2714; | &#x274C; | &#x274C; |
| [vsync/spinlock/twalock.h](twalock.h.md)|Ticketlock with waiting array (TWA). | &#x2714; | &#x274C; | &#x274C; |


---
