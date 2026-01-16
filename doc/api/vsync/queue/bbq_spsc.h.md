#  [vsync](../README.md) / [queue](README.md) / bbq_spsc.h
_Block-based Bounded Queue single-producer/single-consumer._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md)

A highly performant bounded queue that splits the buffer in multiple blocks.

### Remarks:

In this implementations, values have a fixed size equal to pointer size. This implementation does not support `DROP_OLD` mode as described in the original paper.


### References:
 [BBQ: A Block-based Bounded Queue for Exchanging Data and Profiling](https://www.usenix.org/conference/atc22/presentation/wang-jiawei)


### Example:

### Multi-threaded



```c
```

 

### Multi-process



```c
```

 

---
# Macros 

| Macro | Description |
|---|---|
| [BBQ_BLOCK_NUM_LOG](bbq_spsc.h.md#macro-bbq_block_num_log) | Define this macro with `-DBBQ_BLOCK_NUM_LOG=N` to define the total number of blocks equals to `2^N`  |
| [BBQ_ENTRY_SIZE_LOG](bbq_spsc.h.md#macro-bbq_entry_size_log) | Define this macro with `-BBQ_ENTRY_SIZE_LOG=N` to define an entry size equals to `2^N`  |

##  Macro `BBQ_BLOCK_NUM_LOG`

 
_Define this macro with_ `-DBBQ_BLOCK_NUM_LOG=N` _to define the total number of blocks equals to_ `2^N` __


> **Note:** default value is `3U` -> (`8` blocks) 


##  Macro `BBQ_ENTRY_SIZE_LOG`

 
_Define this macro with_ `-BBQ_ENTRY_SIZE_LOG=N` _to define an entry size equals to_ `2^N` __


> **Note:** default value is `log2(sizeof(vuintptr_t))` 


---
# Functions 

| Function | Description |
|---|---|
| [bbq_spsc_enqueue](bbq_spsc.h.md#function-bbq_spsc_enqueue) | Enqueues one or more entries.  |
| [bbq_spsc_dequeue](bbq_spsc.h.md#function-bbq_spsc_dequeue) | Dequeues one or more entries.  |
| [bbq_spsc_memsize](bbq_spsc.h.md#function-bbq_spsc_memsize) | Calculates the size of bbq_spsc_t object based on the given capacity.  |
| [bbq_spsc_init](bbq_spsc.h.md#function-bbq_spsc_init) | Initializes a bbq data structure.  |

##  Function `bbq_spsc_enqueue`

```c
static vuint32_t bbq_spsc_enqueue(bbq_spsc_t *q, vuintptr_t *buf, vuint32_t count, vbool_t wait)
``` 
_Enqueues one or more entries._ 


Multiple entries can be enqueued if `src` points to an array. Use `count` to indicate how many entries should be enqueueed, starting from `src`.



**Parameters:**

- `q`: address of bbq_spsc_t object. 
- `buf`: address of the first entry. 
- `count`: number of entries to enqueue. 
- `wait`: true/false when set to true it waits (blocks) till space becomes available. Otherwise, it quits retrying.


**Returns:** number of enqueued entries. 



##  Function `bbq_spsc_dequeue`

```c
static vuint32_t bbq_spsc_dequeue(bbq_spsc_t *q, vuintptr_t *buf, vuint32_t count, vbool_t wait)
``` 
_Dequeues one or more entries._ 


Multiple entries can be dequeued if `buf` points to an array. Use `count` to indicate how many entries should be dequeued.



**Parameters:**

- `q`: address of bbq_spsc_t object. 
- `buf`: address of the first entry of the preallocated memory. 
- `count`: number of entries to dequeue. 
- `wait`: true/false. When set to true the API waits/blocks for entries to be available


**Returns:** number of dequeued entries. 



##  Function `bbq_spsc_memsize`

```c
static vsize_t bbq_spsc_memsize(vsize_t capacity)
``` 
_Calculates the size of bbq_spsc_t object based on the given capacity._ 




**Parameters:**

- `capacity`: maximum number of entries that can fit in the queue. 


**Returns:** size of bbq_spsc_t object with the given capacity. 



##  Function `bbq_spsc_init`

```c
static vbool_t bbq_spsc_init(bbq_spsc_t *rb, vsize_t size)
``` 
_Initializes a bbq data structure._ 




**Parameters:**

- `rb`: address of bbq_spsc_t object. 
- `size`: size of the given bbq_spsc_t object `rb`.


**Returns:** true initialization succeeded. 

**Returns:** false initialization failed. 




---
