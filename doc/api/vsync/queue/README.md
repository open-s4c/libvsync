#  [vsync](../README.md) / queue
_Queues, priority queues and ringbuffers._ 

---
## File Index


| File|Description|Linearizable|Lock-free|SMR-required|Unbounded-Queue|
| --- | --- | --- | --- | --- | --- |
| [vsync/queue/bounded_locked.h](bounded_locked.h.md)|Multi-producer, multi-consumer bounded queue protected by a spinlock. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/bounded_mpmc.h](bounded_mpmc.h.md)|Lockless, multi-producer, multi-consumer bounded queue. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/bounded_spsc.h](bounded_spsc.h.md)|Single-producer, single-consumer, wait-free bounded queue. | &#x2714; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/queue/cachedq.h](cachedq.h.md)|Lockless, multi-producer, multi-consumer queue. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/chaselev.h](chaselev.h.md)|Chase-Lev Work-Stealing bounded deque. | &#x274C; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/queue/unbounded_queue_lf.h](unbounded_queue_lf.h.md)|Lock-free unbounded queue. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/unbounded_queue_lf_recycle.h](unbounded_queue_lf_recycle.h.md)|Lock-free recycle unbounded queue. | &#x2714; | &#x2714; | &#x274C; | &#x2714; |
| [vsync/queue/unbounded_queue_total.h](unbounded_queue_total.h.md)|Unbounded blocking total queue. | &#x2714; | &#x274C; | &#x274C; | &#x2714; |
| [vsync/queue/vqueue_prio_heap_based.h](vqueue_prio_heap_based.h.md)|Unbounded heap-based priority queue. | &#x2714; | &#x274C; | &#x274C; | &#x274C; |
| [vsync/queue/vqueue_prio_skiplist_based.h](vqueue_prio_skiplist_based.h.md)|Skiplist-based priority queue implementation. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/vqueue_prio_stack_array_based.h](vqueue_prio_stack_array_based.h.md)|Array-based bounded priority queue. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/vqueue_prio_stack_tree_based.h](vqueue_prio_stack_tree_based.h.md)|| &#x274C; | &#x274C; | &#x274C; | &#x274C; |


---
