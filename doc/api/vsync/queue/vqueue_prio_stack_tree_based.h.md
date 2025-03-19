#  [vsync](../README.md) / [queue](README.md) / vqueue_prio_stack_tree_based.h
---
# Macros 

| Macro | Description |
|---|---|
| [VQUEUE_PRIO_PRIORITY_RANGE](vqueue_prio_stack_tree_based.h.md#macro-vqueue_prio_priority_range) | `VQUEUE_PRIO_PRIORITY_RANGE` configures the queue to accept priorities from [0:VQUEUE_PRIO_PRIORITY_RANGE-1]  |

##  Macro `VQUEUE_PRIO_PRIORITY_RANGE`

 
`VQUEUE_PRIO_PRIORITY_RANGE` _configures the queue to accept priorities from [0:VQUEUE_PRIO_PRIORITY_RANGE-1]_ 


> **Note:** `VQUEUE_PRIO_PRIORITY_RANGE` must be power of two. 


---
# Functions 

| Function | Description |
|---|---|
| [vqueue_prio_init](vqueue_prio_stack_tree_based.h.md#function-vqueue_prio_init) | Initializes the queue.  |
| [vqueue_prio_destroy](vqueue_prio_stack_tree_based.h.md#function-vqueue_prio_destroy) | Calls the given callback on all remaining nodes.  |
| [vqueue_prio_add](vqueue_prio_stack_tree_based.h.md#function-vqueue_prio_add) | Enqueues the given `node`.  |
| [vqueue_prio_remove_min](vqueue_prio_stack_tree_based.h.md#function-vqueue_prio_remove_min) | Dequeues a node with highest priority available.  |

##  Function `vqueue_prio_init`

```c
static void vqueue_prio_init(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t retire_cb, void *retire_cb_arg, backoff_rand_fun_t rand_fun)
``` 
_Initializes the queue._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `retire_cb`: callback function used for retiring detached nodes to the SMR. 
- `retire_cb_arg`: second parameter of `retire_cb`. 
- `rand_fun`: a function pointer to a function that generates a random number. 




##  Function `vqueue_prio_destroy`

```c
static void vqueue_prio_destroy(vqueue_prio_t *pqueue, vqueue_prio_handle_node_t destroy_cb, void *args)
``` 
_Calls the given callback on all remaining nodes._ 


Nodes can be freed in `destroy_cb`

> **Note:** this is not thread safe and must be called if and only if all threads are done accessing the queue



**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `destroy_cb`: address of a callback function to call on each remaining node. 
- `args`: second argument of `destroy_cb`. 




##  Function `vqueue_prio_add`

```c
static void vqueue_prio_add(vqueue_prio_t *pqueue, vqueue_prio_node_t *pnode, vsize_t priority)
``` 
_Enqueues the given_ `node`_._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 
- `node`: address of vqueue_prio_node_t object. 
- `priority`: the priority associated with `node`. 




##  Function `vqueue_prio_remove_min`

```c
static vqueue_prio_node_t* vqueue_prio_remove_min(vqueue_prio_t *pqueue)
``` 
_Dequeues a node with highest priority available._ 




**Parameters:**

- `pqueue`: address of vqueue_prio_t object. 


**Returns:** vqueue_prio_node_t* address of dequeued object. 




---
