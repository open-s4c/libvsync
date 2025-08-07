#  [vsync](../README.md) / [queue](README.md) / cachedq.h
_Lockless, multi-producer, multi-consumer queue._ 

**Groups:** [Linearizable](../GROUP_linearizable.md)

A variation of the DPDK ring buffer that uses cached variables to improve the performance. It accepts vuint64_t as data type.


### Example:



```c
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <vsync/common/assert.h>
#include <vsync/queue/cachedq.h>

#define BUFFER_ENTRY_NUM 6UL
#define WRITER_THREAD    2UL
#define READER_THREAD    2UL
#define ENQUEUE_BATCH    4UL
#define DEQUEUE_BATCH    3UL
#define DATA             12345UL

cachedq_t *q;

void *
writer(void *id)
{
    vuint64_t buf[ENQUEUE_BATCH] = {0};
    for (vsize_t i = 0; i < ENQUEUE_BATCH; i++) {
        buf[i] = DATA;
    }
    vuint64_t count = cachedq_enqueue(q, buf, ENQUEUE_BATCH);
    printf(
        "Writer #%zu tries to write a batch with %ld items, %ld items are "
        "success.\n",
        (vsize_t)id, ENQUEUE_BATCH, count);
    return NULL;
}

void *
reader(void *id)
{
    vuint64_t buf[DEQUEUE_BATCH] = {0};
    vuint64_t count              = cachedq_dequeue(q, buf, DEQUEUE_BATCH);
    for (vsize_t i = 0; i < count; i++) {
        ASSERT(buf[i] == DATA);
    }
    printf(
        "Reader #%zu tries to read a batch with %ld items, %ld items are "
        "success.\n",
        (vsize_t)id, DEQUEUE_BATCH, count);
    return NULL;
}

int
main(void)
{
    vsize_t buf_size = cachedq_memsize(BUFFER_ENTRY_NUM);
    void *buf        = malloc(buf_size);

    q = cachedq_init(buf, buf_size);
    if (q == NULL) {
        perror("fail to create the cachedq");
    }

    pthread_t thread_w[WRITER_THREAD] = {0};
    for (vsize_t i = 0; i < WRITER_THREAD; i++) {
        pthread_create(&thread_w[i], NULL, writer, (void *)i);
    }

    pthread_t thread_r[READER_THREAD] = {0};
    for (vsize_t i = 0; i < READER_THREAD; i++) {
        pthread_create(&thread_r[i], NULL, reader, (void *)i);
    }

    for (vsize_t i = 0; i < WRITER_THREAD; i++) {
        pthread_join(thread_w[i], NULL);
    }

    for (vsize_t i = 0; i < READER_THREAD; i++) {
        pthread_join(thread_r[i], NULL);
    }

    free(buf);
    return 0;
}
```

 

---
# Functions 

| Function | Description |
|---|---|
| [cachedq_memsize](cachedq.h.md#function-cachedq_memsize) | Returns the minimum allocation size for a`cachedq` with the given capacity.  |
| [cachedq_init](cachedq.h.md#function-cachedq_init) | Initializes the `cachedq` with a maximum capacity.  |
| [cachedq_enqueue](cachedq.h.md#function-cachedq_enqueue) | Enqueues one or more entries.  |
| [cachedq_dequeue](cachedq.h.md#function-cachedq_dequeue) | Dequeues one or more entries.  |
| [cachedq_count](cachedq.h.md#function-cachedq_count) | Returns the current number of entries in the cachedq.  |

##  Function `cachedq_memsize`

```c
static vsize_t cachedq_memsize(vsize_t capacity)
``` 
_Returns the minimum allocation size for a_`cachedq` _with the given capacity._ 




**Parameters:**

- `capacity`: maximum number of entries that can fit in the `cachedq`.


**Returns:** minimum number bytes the `cachedq` buffer should have. 



##  Function `cachedq_init`

```c
static cachedq_t* cachedq_init(void *buf, vsize_t capacity)
``` 
_Initializes the_ `cachedq` _with a maximum capacity._ 




**Parameters:**

- `buf`: a buffer that can accommodate at least `capacity` elements. 
- `capacity`: maximum number of entries that can fit in the cachedq.


**Returns:** `buf` pointer casted to cachedq_t*.

Use [`cachedq_memsize()`](cachedq.h.md#function-cachedq_memsize) to determine the size of buf. 


##  Function `cachedq_enqueue`

```c
static vsize_t cachedq_enqueue(cachedq_t *q, vuint64_t *buf, vsize_t count)
``` 
_Enqueues one or more entries._ 


Multiple entries can be enqueued if `buf` points to an array. Use `count` to indicate how many entries should be enqueueed, starting from `buf`.



**Parameters:**

- `q`: address of `cachedq_t` object. 
- `buf`: pointer to first entry. 
- `count`: number of entries to enqueue.


**Returns:** number of enqueued entries. 



##  Function `cachedq_dequeue`

```c
static vsize_t cachedq_dequeue(cachedq_t *q, vuint64_t *buf, vsize_t count)
``` 
_Dequeues one or more entries._ 


Multiple entries can be dequeued if `buf` points to an array. Use `count` to indicate how many entries should be dequeued.



**Parameters:**

- `q`: address of `cachedq_t` object. 
- `buf`: pointer to preallocated memory for the first entry. 
- `count`: number of entries to dequeue. 


**Returns:** number of dequeued entries. 



##  Function `cachedq_count`

```c
static vsize_t cachedq_count(cachedq_t *q)
``` 
_Returns the current number of entries in the cachedq._ 




**Parameters:**

- `q`: address of `cachedq_t` object. 


**Returns:** number of entries in the cachedq. 




---
