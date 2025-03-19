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
| [vsync/map/simpleht.h](map/simpleht.h.md)|Simple lock-free hashtable. | &#x2714; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/map/skiplist_lf.h](map/skiplist_lf.h.md)|Lock-free concurrent skiplist. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/map/treeset_bst_coarse.h](map/treeset_bst_coarse.h.md)|This implementation of treeset uses unbalanced binary search tree (BST) and coarse-grained locking. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/map/treeset_bst_fine.h](map/treeset_bst_fine.h.md)|This implementation of treeset uses unbalanced binary search tree (BST) and fine-grained locking. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/map/treeset_rb_coarse.h](map/treeset_rb_coarse.h.md)|This implementation of treeset uses balanced red-black tree (RB) and coarse-grained locking. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/map/treeset_rb_fine.h](map/treeset_rb_fine.h.md)|This implementation of treeset uses balanced red-black tree (RB) and fine-grained locking. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/bounded_locked.h](queue/bounded_locked.h.md)|Multi-producer, multi-consumer bounded queue protected by a spinlock. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/bounded_mpmc.h](queue/bounded_mpmc.h.md)|Lockless, multi-producer, multi-consumer bounded queue. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/bounded_spsc.h](queue/bounded_spsc.h.md)|Single-producer, single-consumer, wait-free bounded queue. | &#x2714; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/queue/cachedq.h](queue/cachedq.h.md)|Lockless, multi-producer, multi-consumer queue. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/unbounded_queue_lf.h](queue/unbounded_queue_lf.h.md)|Lock-free unbounded queue. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/unbounded_queue_lf_recycle.h](queue/unbounded_queue_lf_recycle.h.md)|Lock-free recycle unbounded queue. | &#x2714; | &#x2714; | &#x274C; | &#x2714; |
| [vsync/queue/unbounded_queue_total.h](queue/unbounded_queue_total.h.md)|Unbounded blocking total queue. | &#x2714; | &#x274C; | &#x274C; | &#x2714; |
| [vsync/queue/vqueue_prio_heap_based.h](queue/vqueue_prio_heap_based.h.md)|Unbounded heap-based priority queue. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/vqueue_prio_skiplist_based.h](queue/vqueue_prio_skiplist_based.h.md)|Skiplist-based priority queue implementation. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/vqueue_prio_stack_array_based.h](queue/vqueue_prio_stack_array_based.h.md)|Array-based bounded priority queue. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/stack/elimination_stack.h](stack/elimination_stack.h.md)|Unbounded lock-free stack with elimination backoff. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/stack/quack.h](stack/quack.h.md)|Lockfree concurrent stack/queue (Treiber stack) | &#x2714; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/stack/xbo_stack.h](stack/xbo_stack.h.md)|Unbounded lock-free stack with exponential backoff. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |


---
