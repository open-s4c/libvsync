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
| [vsync/map/simpleht.h](simpleht.h.md)|Simple lock-free hashtable. | &#x2714; | &#x2714; | &#x274C; |
| [vsync/map/skiplist_lf.h](skiplist_lf.h.md)|Lock-free concurrent skiplist. | &#x2714; | &#x2714; | &#x2714; |
| [vsync/map/treeset_bst_coarse.h](treeset_bst_coarse.h.md)|This implementation of treeset uses unbalanced binary search tree (BST) and coarse-grained locking. | &#x2714; | &#x274C; | &#x274C; |
| [vsync/map/treeset_bst_fine.h](treeset_bst_fine.h.md)|This implementation of treeset uses unbalanced binary search tree (BST) and fine-grained locking. | &#x2714; | &#x274C; | &#x274C; |
| [vsync/map/treeset_rb_coarse.h](treeset_rb_coarse.h.md)|This implementation of treeset uses balanced red-black tree (RB) and coarse-grained locking. | &#x2714; | &#x274C; | &#x274C; |
| [vsync/map/treeset_rb_fine.h](treeset_rb_fine.h.md)|This implementation of treeset uses balanced red-black tree (RB) and fine-grained locking. | &#x2714; | &#x274C; | &#x274C; |


---
