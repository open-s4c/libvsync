#  [vsync](../README.md) / [queue](README.md) / bbq_mpmc.h
_Block-based Bounded Queue multi-producer/multi-consumer._ 

A highly performant bounded queue that splits the buffer in multiple blocks.

### Remarks:

In this implementations, values have the fixed size (pointer size). This implementation does not support DROP_OLD mode as described in the original paper.


### References:
 [BBQ: A Block-based Bounded Queue for Exchanging Data and Profiling](https://www.usenix.org/conference/atc22/presentation/wang-jiawei)


### Example:



```c
#include <vsync/queue/bbq_mpmc.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#define BUFFER_ENTRY_NUM 4096U
#define NUM              10U
#define ENQUEUE_BATCH    5UL
#define DEQUEUE_BATCH    4UL
#define NUM_WRITER       4U
#define NUM_READER       5U

#define NON_BLOCKING false
#define BLOCKING     true

bbq_mpmc_t *g_bbq_mpmc = NULL;

void *
writer(void *arg)
{
    vuintptr_t buf[ENQUEUE_BATCH] = {0};
    vuint64_t ptr                 = 0;
    vuint64_t rest                = NUM * NUM_READER;

    while (rest) {
        vuint32_t count = rest < ENQUEUE_BATCH ? rest : ENQUEUE_BATCH;
        for (vuint32_t i = 0; i < count; i++) {
            buf[i] = ++ptr;
        }
        rest -= bbq_mpmc_enqueue(g_bbq_mpmc, buf, count, BLOCKING);
    }

    (void)arg;
    return NULL;
}

void *
reader(void *arg)
{
    vuintptr_t buf[DEQUEUE_BATCH] = {0};
    vuint64_t rest                = NUM * NUM_WRITER;

    while (rest) {
        vuint32_t batch_size = rest < DEQUEUE_BATCH ? rest : DEQUEUE_BATCH;
        vuint32_t count =
            bbq_mpmc_dequeue(g_bbq_mpmc, buf, batch_size, NON_BLOCKING);

        for (vuint32_t i = 0; i < count; i++) {
            ASSERT(buf[i] > 0);
            printf("dequeue item %lu\n", buf[i]);
        }
        rest -= count;
    }
    (void)arg;
    return NULL;
}

int
main(void)
{
    pthread_t t_writer[NUM_WRITER];
    pthread_t t_reader[NUM_READER];

    // allocate
    vsize_t sz = bbq_mpmc_memsize(BUFFER_ENTRY_NUM);
    g_bbq_mpmc = (bbq_mpmc_t *)malloc(sz);
    ASSERT(g_bbq_mpmc);

    // init
    vbool_t success = bbq_mpmc_init(g_bbq_mpmc, sz);
    ASSERT(success);

    for (vsize_t i = 0; i < NUM_WRITER; i++) {
        pthread_create(&t_writer[i], NULL, writer, NULL);
    }
    for (vsize_t i = 0; i < NUM_READER; i++) {
        pthread_create(&t_reader[i], NULL, reader, NULL);
    }
    for (vsize_t i = 0; i < NUM_WRITER; i++) {
        pthread_join(t_writer[i], NULL);
    }
    for (vsize_t i = 0; i < NUM_READER; i++) {
        pthread_join(t_reader[i], NULL);
    }

    // deallocate
    free(g_bbq_mpmc);
    return 0;
}
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
