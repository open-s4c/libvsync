#  Linearizable
_Group of algorithms linearizable algorithms._ 

---
## File Index


| File|Description|Linearizable|Lock-free|SMR-required|Unbounded-Queue|
| --- | --- | --- | --- | --- | --- |
| [vsync/map/hashtable_standard.h](map/hashtable_standard.h.md)|This is a lock-free listset based hashtable. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/map/listset_coarse.h](map/listset_coarse.h.md)|This implementation of listset uses one lock to serialize all operations. | &#x2714; | &#x274C; | &#x2714; | &#x274C; |
| [vsync/map/listset_fine.h](map/listset_fine.h.md)|This implementation of listset uses fine grained locking. | &#x2714; | &#x274C; | &#x2714; | &#x274C; |
| [vsync/map/listset_lazy.h](map/listset_lazy.h.md)|This is a partially parallel implementation with lock-free get. | &#x2714; | &#x274C; | &#x2714; | &#x274C; |
| [vsync/map/listset_lf.h](map/listset_lf.h.md)|Lock-free implementation of listset. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/map/listset_opt.h](map/listset_opt.h.md)|This implementation is an optimized verison of listset_fine. | &#x2714; | &#x274C; | &#x2714; | &#x274C; |
| [vsync/queue/bounded_locked.h](queue/bounded_locked.h.md)|Multi-producer, multi-consumer bounded queue protected by a spinlock. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/bounded_mpmc.h](queue/bounded_mpmc.h.md)|Lockless, multi-producer, multi-consumer bounded queue. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/bounded_spsc.h](queue/bounded_spsc.h.md)|Single-producer, single-consumer, wait-free bounded queue. | &#x2714; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/queue/unbounded_queue_lf.h](queue/unbounded_queue_lf.h.md)|Lock-free unbounded queue. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/unbounded_queue_lf_recycle.h](queue/unbounded_queue_lf_recycle.h.md)|Lock-free recycle unbounded queue. | &#x2714; | &#x2714; | &#x274C; | &#x2714; |
| [vsync/queue/unbounded_queue_total.h](queue/unbounded_queue_total.h.md)|Unbounded blocking total queue. | &#x2714; | &#x274C; | &#x274C; | &#x2714; |
| [vsync/stack/quack.h](stack/quack.h.md)|Lockfree concurrent stack/queue (Treiber stack) | &#x2714; | &#x2714; | &#x274C; | &#x274C; |


---
