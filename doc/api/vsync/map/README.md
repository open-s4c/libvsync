#  [vsync](../README.md) / map
_This is a collection of algos that implement map interface._ 

---
## File Index


| File|Description|Linearizable|Lock-free|SMR-required|
| --- | --- | --- | --- | --- |
| [vsync/map/hashtable_standard.h](hashtable_standard.h.md)|This is a lock-free listset based hashtable. | &#x2714; | &#x2714; | &#x2714; |
| [vsync/map/listset_coarse.h](listset_coarse.h.md)|This implementation of listset uses one lock to serialize all operations. | &#x2714; | &#x274C; | &#x2714; |
| [vsync/map/listset_fine.h](listset_fine.h.md)|This implementation of listset uses fine grained locking. | &#x2714; | &#x274C; | &#x2714; |
| [vsync/map/listset_lazy.h](listset_lazy.h.md)|This is a partially parallel implementation with lock-free get. | &#x2714; | &#x274C; | &#x2714; |
| [vsync/map/listset_lf.h](listset_lf.h.md)|Lock-free implementation of listset. | &#x2714; | &#x2714; | &#x2714; |
| [vsync/map/listset_opt.h](listset_opt.h.md)|This implementation is an optimized verison of listset_fine. | &#x2714; | &#x274C; | &#x2714; |


---
