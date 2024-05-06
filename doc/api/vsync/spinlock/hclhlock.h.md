#  [vsync](../README.md) / [spinlock](README.md) / hclhlock.h
_Hierarchical CLH Queue Lock._ 

**Groups:** [Numa-aware](GROUP_numa_aware.md)


### Example:



```c

/* you can use the default value or redefine it to indicate how many     \
         clusters you need */
#define HCLH_MAX_CLUSTERS 2
#include <vsync/spinlock/hclhlock.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

#define N            10
#define IT           10
#define EXPECTED_VAL (N) * (IT)

vuint32_t g_x = 0;
vuint32_t g_y = 0;

hclh_lock_t g_lock;
hclh_qnode_t g_qnode[N] = {0};
hclh_tnode_t g_tnode[N] = {0};

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;

    /* this should be assigned to either numa group or cache group where the
     * thread is running */
    vuint32_t cluster = tid % HCLH_MAX_CLUSTERS;

    hclhlock_init_tnode(&g_tnode[tid], &g_qnode[tid], cluster);

    for (vsize_t i = 0; i < IT; i++) {
        hclhlock_acquire(&g_lock, &g_tnode[tid]);
        g_x++;
        g_y++;
        assert(g_x == g_y);
        hclhlock_release(&g_tnode[tid]);
    }

    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    hclhlock_init(&g_lock);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final value %u %u\n", g_x, g_y);
    assert(g_x == EXPECTED_VAL);
    assert(g_x == g_y);
    return 0;
}
```




### References:

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 7.8.2](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Macros 

| Macro | Description |
|---|---|
| [HCLH_MAX_CLUSTERS](hclhlock.h.md#macro-hclh_max_clusters) | configures the number of available clusters in HCLH lock.  |

##  Macro `HCLH_MAX_CLUSTERS`

 
_configures the number of available clusters in HCLH lock._ 


Each cluster has a local queue. Each thread must be associated with a cluster value in `[0: HCLH_MAX_CLUSTERS -1]`

A cluster can be thought of as a NUMA node or cache group. Threads running on cores that belong to the same cache-group or NUMA node are expected to be associated with the same cluster.

> **Note:** by default 32 clusters are defined. Users can redefine it using: `-DHCLH_MAX_CLUSTERS=N`, where `N` is the number of clusters the user wishes to set. 


---
# Functions 

| Function | Description |
|---|---|
| [hclhlock_init](hclhlock.h.md#function-hclhlock_init) | Initializes the lock.  |
| [hclhlock_init_tnode](hclhlock.h.md#function-hclhlock_init_tnode) | Initializes the given `tnode`.  |
| [hclhlock_acquire](hclhlock.h.md#function-hclhlock_acquire) | Acquires the lock.  |
| [hclhlock_release](hclhlock.h.md#function-hclhlock_release) | Releases the lock.  |

##  Function `hclhlock_init`

```c
static void hclhlock_init(hclh_lock_t *hclh_lock)
``` 
_Initializes the lock._ 




**Parameters:**

- `hclh_lock`: address of hclh_lock object to be initialized. 




##  Function `hclhlock_init_tnode`

```c
static void hclhlock_init_tnode(hclh_tnode_t *tnode, hclh_qnode_t *qnode, vuint32_t cluster)
``` 
_Initializes the given_ `tnode`_._ 




**Parameters:**

- `tnode`: address of the hclh_tnode_t object associated with the calling thread. 
- `qnode`: address of the hclh_qnode_t object associated with the calling thread. 
- `cluster`: the cluster to which the calling thread belongs.


> **Note:** Must be called once before the first call of `hclhlock_acquire`.

> **Note:** `qnode` must live as long as the system is running and not be freed. The reason is that on release qnodes get passed to successor threads.

> **Note:** `tnode` should live as long as the associated thread lives. 


##  Function `hclhlock_acquire`

```c
static void hclhlock_acquire(hclh_lock_t *lock, hclh_tnode_t *tnode)
``` 
_Acquires the lock._ 




**Parameters:**

- `lock`: address of the lock to be used. 
- `tnode`: address of the hclh_tnode_t object associated with the calling thread.


> **Note:** `hclhlock_init_tnode` must be called on the given `tnode`, before `hclhlock_acquire` is called for the first time. 


##  Function `hclhlock_release`

```c
static void hclhlock_release(hclh_tnode_t *tnode)
``` 
_Releases the lock._ 




**Parameters:**

- `tnode`: address of the hclh_tnode_t object associated with the calling thread. 





---
