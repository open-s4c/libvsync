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
#include <vsync/queue/bbq_spsc.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#define BUFFER_ENTRY_NUM 4096U
#define NUM              10U
#define ENQUEUE_BATCH    5UL
#define DEQUEUE_BATCH    4UL

#define NON_BLOCKING false
#define BLOCKING     true

bbq_spsc_t *g_bbq_spsc = NULL;

void *
writer(void *arg)
{
    vuintptr_t buf[ENQUEUE_BATCH] = {0};
    vuint64_t ptr                 = 0;
    vuint64_t rest                = NUM;

    while (rest) {
        vuint32_t count = rest < ENQUEUE_BATCH ? rest : ENQUEUE_BATCH;
        for (vuint32_t i = 0; i < count; i++) {
            buf[i] = ptr++;
        }
        rest -= bbq_spsc_enqueue(g_bbq_spsc, buf, count, BLOCKING);
    }

    (void)arg;
    return NULL;
}

void *
reader(void *arg)
{
    vuintptr_t buf[DEQUEUE_BATCH] = {0};
    vuint64_t ptr                 = 0;
    vuint64_t rest                = NUM;

    while (rest) {
        vuint32_t count =
            bbq_spsc_dequeue(g_bbq_spsc, buf, DEQUEUE_BATCH, NON_BLOCKING);

        for (vuint32_t i = 0; i < count; i++) {
            ASSERT(buf[i] == ptr++);
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
    pthread_t t_writer;
    pthread_t t_reader;

    // allocate
    vsize_t sz = bbq_spsc_memsize(BUFFER_ENTRY_NUM);
    g_bbq_spsc = (bbq_spsc_t *)malloc(sz);
    ASSERT(g_bbq_spsc);

    // init
    vbool_t success = bbq_spsc_init(g_bbq_spsc, sz);
    ASSERT(success);

    pthread_create(&t_writer, NULL, writer, NULL);
    pthread_create(&t_reader, NULL, reader, NULL);
    pthread_join(t_reader, NULL);
    pthread_join(t_writer, NULL);

    // deallocate
    free(g_bbq_spsc);
    return 0;
}
```

 

### Multi-process



```c
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <vsync/vtypes.h>
#include <vsync/queue/bbq_spsc.h>

#define QUEUE_SIZE      4095U
#define EPOCH_SIZE      100U
#define NUM_ENTRIES     1U
#define SLEEP_MICRO_SEC 10
#define NON_BLOCKING    false
#define BLOCKING        true

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

bbq_spsc_t *g_bbq;

void *
create_shared_memory(vsize_t size)
{
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0);
}

void
check_space(void)
{
    vuint64_t data  = 0;
    vuint64_t count = 0;

    while (bbq_spsc_dequeue(g_bbq, &data, NUM_ENTRIES, NON_BLOCKING)) {}

    while (bbq_spsc_enqueue(g_bbq, &data, NUM_ENTRIES, NON_BLOCKING)) {
        count++;
    }
    while (bbq_spsc_dequeue(g_bbq, &data, NUM_ENTRIES, NON_BLOCKING)) {}

    printf(
        "Current size of the queue is %ld after %d crashes, shouldn't be "
        "zero.\n",
        count, EPOCH_SIZE);
    assert(count != 0);
}

void *
writer(void *arg)
{
    V_UNUSED(arg);
    vuint64_t data = 0;
    while (true) {
        data = (data + 1) % (QUEUE_SIZE - 1);
        while (!bbq_spsc_enqueue(g_bbq, &data, NUM_ENTRIES, NON_BLOCKING)) {}
        usleep(SLEEP_MICRO_SEC);
    }
}

void *
reader(void *arg)
{
    V_UNUSED(arg);
    vuint64_t data;
    while (true) {
        while (!bbq_spsc_dequeue(g_bbq, &data, 1, NON_BLOCKING)) {}
        usleep(SLEEP_MICRO_SEC);
        assert(data < QUEUE_SIZE - 1);
    }
}

int
main(void)
{
    srand(time(NULL));
    vuint64_t sz = bbq_spsc_memsize(QUEUE_SIZE);
    g_bbq        = create_shared_memory(sz);
    assert(g_bbq);
    vbool_t success = bbq_spsc_init(g_bbq, sz);
    ASSERT(success && "BBQ init failed");
    for (vuint32_t i = 0; i < EPOCH_SIZE; i++) {
        pid_t pid = fork();
        assert(pid >= 0);
        if (pid != 0) {
            usleep(rand() % 10000);
            while (kill(pid, SIGTERM) < 0) {}
        } else {
            pthread_t t_writer, t_reader;
            pthread_create(&t_writer, NULL, writer, NULL);
            pthread_create(&t_reader, NULL, reader, NULL);
            pthread_join(t_writer, NULL);
            pthread_join(t_reader, NULL);
            return 0;
        }
    }
    check_space();
    return 0;
}
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
