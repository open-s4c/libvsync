#  [vsync](../README.md) / [spinlock](README.md) / cnalock.h
_Compact NUMA-aware Lock._ 

**Groups:** [Numa-aware](GROUP_numa_aware.md)

The CNA is an efficient variant of the MCS locks, which adds NUMA-awareness without a hierarchical approach.


### Example:



```c

#include <vsync/spinlock/cnalock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12U
#define EXPECTED_VAL N

cnalock_t g_lock      = CNALOCK_INIT();
cna_node_t g_nodes[N] = {0};

vuint32_t g_x = 0;
vuint32_t g_y = 0;

typedef struct eg_args {
    vsize_t tid;
    vuint32_t numa_node;
} eg_args_t;

void *
run(void *args)
{
    eg_args_t *t_args = (eg_args_t *)args;
    // Bind the thread to run only on the specified numa node, e.g.
    // with `sched_setaffinity`.

    // an example critical section.
    cnalock_acquire(&g_lock, &g_nodes[t_args->tid], t_args->numa_node);
    g_x++;
    g_y++;
    cnalock_release(&g_lock, &g_nodes[t_args->tid], t_args->numa_node);
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];
    eg_args_t args[N];

    for (vsize_t i = 0; i < N; i++) {
        args[i].tid = i;
        // In a real-world program you would probably use `libnuma` to detect
        // how many numa nodes there are, and have a more elaborate strategy
        // for binding threads to certain cores / nodes.
        args[i].numa_node = i % 2;
        pthread_create(&threads[i], NULL, run, (void *)&args[i]);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    ASSERT(g_x == EXPECTED_VAL);
    ASSERT(g_x == g_y);
    printf("Final value %u\n", g_x);
    return 0;
}
```




### References:
 Dave Dice and Alex Kogan - [Compact NUMA-aware locks](https://dl.acm.org/doi/10.1145/3302424.3303984) 

---
# Macros 

| Macro | Description |
|---|---|
| [CNALOCK_INIT](cnalock.h.md#macro-cnalock_init) | Initializer of `cnalock_t`.  |

##  Macro `CNALOCK_INIT`

```c
CNALOCK_INIT()
```

 
_Initializer of_ `cnalock_t`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [cnalock_init](cnalock.h.md#function-cnalock_init) | Initializes the CNA lock.  |
| [cnalock_acquire](cnalock.h.md#function-cnalock_acquire) | Acquires the CNA lock.  |
| [cnalock_release](cnalock.h.md#function-cnalock_release) | Releases the CNA lock.  |

##  Function `cnalock_init`

```c
static void cnalock_init(cnalock_t *lock)
``` 
_Initializes the CNA lock._ 




**Parameters:**

- `lock`: address of cnalock_t object.


> **Note:** alternatively use CNALOCK_INIT 


##  Function `cnalock_acquire`

```c
static void cnalock_acquire(cnalock_t *lock, cna_node_t *me, vuint32_t numa_node)
``` 
_Acquires the CNA lock._ 




**Parameters:**

- `lock`: address of cnalock_t object. 
- `me`: address of cna_node_t object associated with the calling thread. 
- `numa_node`: valid id of the NUMA node where the calling thread is hosted 




##  Function `cnalock_release`

```c
static void cnalock_release(cnalock_t *lock, cna_node_t *me, vuint32_t numa_node)
``` 
_Releases the CNA lock._ 




**Parameters:**

- `lock`: address of cnalock_t object. 
- `me`: address of cna_node_t object associated with the calling thread. 
- `numa_node`: valid id of the NUMA node where the calling thread is hosted. 





---
