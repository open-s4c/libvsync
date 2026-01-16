#  [vsync](../README.md) / [queue](README.md) / bbq_mpmc.h
_Block-based Bounded Queue multi-producer/multi-consumer._ 

A highly performant bounded queue that splits the buffer in multiple blocks.

### Remarks:

In this implementations, values have the fixed size (pointer size). This implementation does not support DROP_OLD mode as described in the original paper.


### References:
 [BBQ: A Block-based Bounded Queue for Exchanging Data and Profiling](https://www.usenix.org/conference/atc22/presentation/wang-jiawei)


### Example:



```c
```

 

---
# Macros 

| Macro | Description |
|---|---|
| [BBQ_BLOCK_NUM_LOG](bbq_mpmc.h.md#macro-bbq_block_num_log) | Define this macro with `-DBBQ_BLOCK_NUM_LOG=N` to define the total number of blocks equals to `2^N`  |
| [BBQ_ENTRY_SIZE_LOG](bbq_mpmc.h.md#macro-bbq_entry_size_log) | Define this macro with `-BBQ_ENTRY_SIZE_LOG=N` to define an entry size equals to `2^N`  |

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
| [bbq_mpmc_enqueue](bbq_mpmc.h.md#function-bbq_mpmc_enqueue) | Enqueues one or more entries.  |
| [bbq_mpmc_dequeue](bbq_mpmc.h.md#function-bbq_mpmc_dequeue) | Dequeues one or more entries.  |
| [bbq_mpmc_memsize](bbq_mpmc.h.md#function-bbq_mpmc_memsize) | Calculates the size of the bbq queue.  |
| [bbq_mpmc_init](bbq_mpmc.h.md#function-bbq_mpmc_init) | Initializes a bbq data structure.  |

##  Function `bbq_mpmc_enqueue`

```c
static vuint32_t bbq_mpmc_enqueue(bbq_mpmc_t *q, vuintptr_t *buf, vuint32_t count, vbool_t wait)
``` 
_Enqueues one or more entries._ 


Multiple entries can be enqueued if `src` points to an array. Use `count` to indicate how many entries should be enqueueed, starting from `src`.



**Parameters:**

- `q`: pointer to bbq_mpmc data structure 
- `src`: pointer to first entry 
- `count`: number of entries to enqueue 
- `wait`: should wait for space to be available


**Returns:** number of enqueued entries 



##  Function `bbq_mpmc_dequeue`

```c
static vuint32_t bbq_mpmc_dequeue(bbq_mpmc_t *q, vuintptr_t *buf, vuint32_t count, vbool_t wait)
``` 
_Dequeues one or more entries._ 


Multiple entries can be dequeued if `src` points to an array. Use `count` to indicate how many entries should be dequeued.



**Parameters:**

- `q`: pointer to bbq_mpmc data structure 
- `src`: pointer to preallocated memory for the first entry 
- `count`: number of entries to dequeue 
- `wait`: should wait for entries to be available


**Returns:** number of dequeued entries 



##  Function `bbq_mpmc_memsize`

```c
static vsize_t bbq_mpmc_memsize(vsize_t capacity)
``` 
_Calculates the size of the bbq queue._ 




**Parameters:**

- `capacity`: maximum number of entries that can fit in the queue 


**Returns:** size to be allocated in bytes 



##  Function `bbq_mpmc_init`

```c
static vbool_t bbq_mpmc_init(bbq_mpmc_t *rb, vsize_t size)
``` 
_Initializes a bbq data structure._ 




**Parameters:**

- `buf`: pointer to bbq data structure 
- `size`: number of bytes allocated for bbq data structure 


**Returns:** true initialization succeeded. 

**Returns:** false initialization failed. 




---
