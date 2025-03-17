#  SMR-required
_Group of algorithms that require safe memory reclamation._ 

Users are expected to couple the usage of these algorithms with an SMR scheme from vsync/smr e.g. vsync/smr/gdump.h . Detached/retired nodes of these algorithms must be retired to the SMR and not freed/recycled directly. Operations of these algorithms should be called within the critical section of the SMR within enter/exit, unless stated otherwise. 

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
| [vsync/map/skiplist_lf.h](map/skiplist_lf.h.md)|Lock-free concurrent skiplist. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/queue/unbounded_queue_lf.h](queue/unbounded_queue_lf.h.md)|Lock-free unbounded queue. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/vqueue_prio_skiplist_based.h](queue/vqueue_prio_skiplist_based.h.md)|Skiplist-based priority queue implementation. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/vqueue_prio_stack_array_based.h](queue/vqueue_prio_stack_array_based.h.md)|Array-based bounded priority queue. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/stack/elimination_stack.h](stack/elimination_stack.h.md)|Unbounded lock-free stack with elimination backoff. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/stack/xbo_stack.h](stack/xbo_stack.h.md)|Unbounded lock-free stack with exponential backoff. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |


---
