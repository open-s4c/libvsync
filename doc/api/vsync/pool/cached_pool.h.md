#  [vsync](../README.md) / [pool](README.md) / cached_pool.h
_Multi-threaded, fixed size, and fixed capacity memory pool._ 

### Performance:


- No atomic operations in the fast path of alloc/free
- Increasing number of entries may have a better performance
### Example:




```c
#include <vsync/vtypes.h>
#include <vsync/common/assert.h>
#include <vsync/pool/cached_pool.h>
#include <pthread.h>
#include <stdlib.h>

#define CACHEDP_NUM_ENTRIES 4096U
#define CACHEDP_MAX_THREAD  32U

cached_pool_t *g_pool;
void *g_buff;

typedef struct entry_s {
    vuint32_t x;
    vuint32_t y;
} entry_t;

#define CACHEDP_ENTRY_SIZE sizeof(entry_t)

void
init(void)
{
    vsize_t size = cached_pool_memsize(CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES,
                                       CACHEDP_ENTRY_SIZE);
    g_buff       = malloc(size);
    g_pool = cached_pool_init(g_buff, CACHEDP_MAX_THREAD, CACHEDP_NUM_ENTRIES,
                              CACHEDP_ENTRY_SIZE);
}

void
deinit(void)
{
    free(g_pool);
}

void *
run(void *arg)
{
    vuint64_t tid  = (vuint64_t)(arg);
    entry_t *entry = cached_pool_alloc(g_pool, tid);
    entry->x       = 123;
    entry->y       = 456;
    cached_pool_free(g_pool, tid, entry);
    return NULL;
}

int
main(void)
{
    init();
    pthread_t threads[CACHEDP_MAX_THREAD];
    for (vuint64_t i = 0; i < CACHEDP_MAX_THREAD; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }
    for (vuint64_t i = 0; i < CACHEDP_MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }
    deinit();
}
```

 

---
# Macros 

| Macro | Description |
|---|---|
| [cached_pool_memsize](cached_pool.h.md#macro-cached_pool_memsize) | Calculate the needed memory space for creating a pool.  |

##  Macro `cached_pool_memsize`

```c
cached_pool_memsize(thread_num,entry_num,entry_size)
```

 
_Calculate the needed memory space for creating a pool._ 




**Parameters:**

- `thread_num`: maxinum thread number 
- `entry_num`: minimal number of entires 
- `entry_size`: size of each entry


**Returns:** needed memory space (in bytes) 



---
# Functions 

| Function | Description |
|---|---|
| [cached_pool_init](cached_pool.h.md#function-cached_pool_init) | Initialize the pool.  |
| [cached_pool_alloc](cached_pool.h.md#function-cached_pool_alloc) | Allocate an entry.  |
| [cached_pool_free](cached_pool.h.md#function-cached_pool_free) | Free an entry.  |

##  Function `cached_pool_init`

```c
static cached_pool_t* cached_pool_init(void *buf, vuint32_t thread_num, vuint32_t entry_num, vsize_t entry_size)
``` 
_Initialize the pool._ 


Make sure the buffer has enough size (calculated by cached_pool_memsize)



**Parameters:**

- `buf`: pointer to the buffer 
- `thread_num`: maxinum thread number 
- `entry_num`: minimal number of entires 
- `entry_size`: size of each entry


**Returns:** a pointer of the pool structure 



##  Function `cached_pool_alloc`

```c
static void* cached_pool_alloc(cached_pool_t *a, vuint32_t id)
``` 
_Allocate an entry._ 


Cannot be called from different threads with the same id



**Parameters:**

- `a`: pointer to the pool data structure 
- `id`: thread ID


**Returns:** address of the allocated entry (NULL if there is no space) 



##  Function `cached_pool_free`

```c
static void cached_pool_free(cached_pool_t *a, vuint32_t id, void *p)
``` 
_Free an entry._ 


Cannot be called from different threads with the same id



**Parameters:**

- `a`: pointer to the pool data structure 
- `id`: thread ID 
- `p`: address of the allocated entry 





---
