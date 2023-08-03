#  Lock-free
_Group of algorithms with lock-free progress condition._ 

---
## File Index


| File|Description|Linearizable|Lock-free|SMR-required|Unbounded-Queue|
| --- | --- | --- | --- | --- | --- |
| [vsync/map/hashtable_standard.h](map/hashtable_standard.h.md)|This is a lock-free listset based hashtable. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/map/listset_lf.h](map/listset_lf.h.md)|Lock-free implementation of listset. | &#x2714; | &#x2714; | &#x2714; | &#x274C; |
| [vsync/queue/bounded_spsc.h](queue/bounded_spsc.h.md)|Single-producer, single-consumer, wait-free bounded queue. | &#x2714; | &#x2714; | &#x274C; | &#x274C; |
| [vsync/queue/unbounded_queue_lf.h](queue/unbounded_queue_lf.h.md)|Lock-free unbounded queue. | &#x2714; | &#x2714; | &#x2714; | &#x2714; |
| [vsync/queue/unbounded_queue_lf_recycle.h](queue/unbounded_queue_lf_recycle.h.md)|Lock-free recycle unbounded queue. | &#x2714; | &#x2714; | &#x274C; | &#x2714; |
| [vsync/stack/quack.h](stack/quack.h.md)|Lockfree concurrent stack/queue (Treiber stack) | &#x2714; | &#x2714; | &#x274C; | &#x274C; |


---
