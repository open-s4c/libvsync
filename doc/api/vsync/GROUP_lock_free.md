#  Lock-free
_Group of algorithms with lock-free progress condition._ 

---
## File Index


| File|Description|Linearizable|Lock-free|SMR-required|Unbounded-Queue|
| --- | --- | --- | --- | --- | --- |
| [vsync/map/hashtable_standard.h](map/hashtable_standard.h.md)|This is a lock-free listset based hashtable. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/map/listset_lf.h](map/listset_lf.h.md)|Lock-free implementation of listset. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/map/simpleht.h](map/simpleht.h.md)|Simple lock-free hashtable. | &#x2714; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/map/skiplist_lf.h](map/skiplist_lf.h.md)|Lock-free concurrent skiplist. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/queue/bounded_spsc.h](queue/bounded_spsc.h.md)|Single-producer, single-consumer, wait-free bounded queue. | &#x2714; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/queue/chaselev.h](queue/chaselev.h.md)|Chase-Lev Work-Stealing bounded deque. | &#x274C; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/queue/unbounded_queue_lf.h](queue/unbounded_queue_lf.h.md)|Lock-free unbounded queue. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/unbounded_queue_lf_recycle.h](queue/unbounded_queue_lf_recycle.h.md)|Lock-free recycle unbounded queue. | &#x2714; | &#x2714; | &#x274C; | &#x2714; |
| [vsync/queue/vqueue_prio_skiplist_based.h](queue/vqueue_prio_skiplist_based.h.md)|Skiplist-based priority queue implementation. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/vqueue_prio_stack_array_based.h](queue/vqueue_prio_stack_array_based.h.md)|Array-based bounded priority queue. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/stack/elimination_stack.h](stack/elimination_stack.h.md)|Unbounded lock-free stack with elimination backoff. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/stack/quack.h](stack/quack.h.md)|Lockfree concurrent stack/queue (Treiber stack) | &#x2714; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/stack/xbo_stack.h](stack/xbo_stack.h.md)|Unbounded lock-free stack with exponential backoff. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |


---
