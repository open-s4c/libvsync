#  [vsync](../README.md) / [spinlock](README.md) / hmcslock.h
_Hierarchical MCS lock for systems with NUMA Hierarchies._ 

**Groups:** [Numa-aware](GROUP_numa_aware.md)


### Example:



```c

#include <vsync/spinlock/hmcslock.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

// To build such a hierarchical tree:               //
//       R                                          //
//     /   \                                        //
//    A     B                                       //
//   / \   / \                                      //
//  C   D  E  F                                     //
//                                                  //
// The following specs are needed:                  //
// level 1, has a root (1 node)                     //
// level 2, has two nodes connected to R the parent //
// level 3, has two nodes per parent (A/B)          //
//                                                  //
// number of levels = 3                             //
// number of locks  = 7                             //

#define NUM_LEVELS 3

#define LEVEL_1 1
#define LEVEL_2 2
#define LEVEL_3 2

#define LEVEL_1_THRESHOLD 1
#define LEVEL_2_THRESHOLD 1
#define LEVEL_3_THRESHOLD 1

hmcslock_level_spec_t g_level_specs[NUM_LEVELS] = {
    {LEVEL_1, LEVEL_1_THRESHOLD},
    {LEVEL_2, LEVEL_2_THRESHOLD},
    {LEVEL_3, LEVEL_3_THRESHOLD},
};

#define NUM_LOCKS                                                              \
    ((LEVEL_3 * LEVEL_2 * LEVEL_1) + (LEVEL_1 * LEVEL_2) + (LEVEL_1))

#define N            10
#define IT           10
#define EXPECTED_VAL (N) * (IT)

vuint32_t g_x = 0;
vuint32_t g_y = 0;

hmcslock_t g_locks[NUM_LOCKS];
hmcs_node_t g_qnode[N] = {0};

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;

    /* the thread should be pinned to the core */

    /* find the leaf lock associated with this code */
    hmcslock_t *lock = hmcslock_which_lock(g_locks, NUM_LOCKS, g_level_specs,
                                           NUM_LEVELS, LEVEL_3, tid);

    for (vsize_t i = 0; i < IT; i++) {
        hmcslock_acquire(lock, &g_qnode[tid], NUM_LEVELS);
        g_x++;
        g_y++;
        assert(g_x == g_y);
        hmcslock_release(lock, &g_qnode[tid], NUM_LEVELS);
    }

    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    hmcslock_init(g_locks, NUM_LOCKS, g_level_specs, NUM_LEVELS);

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

[High Performance Locks for Multi-level NUMA Systems](https://dl.acm.org/doi/pdf/10.1145/2858788.2688503)


### References:

[Verifying and Optimizing the HMCS Lock for Arm Servers](https://link.springer.com/chapter/10.1007/978-3-030-91014-3_17) 

---
# Functions 

| Function | Description |
|---|---|
| [hmcslock_which_lock](hmcslock.h.md#function-hmcslock_which_lock) | Returns the address of the lock associated with the core id.  |
| [hmcslock_init](hmcslock.h.md#function-hmcslock_init) | Initializes the locks array.  |
| [hmcslock_acquire](hmcslock.h.md#function-hmcslock_acquire) | Acquires the given lock.  |
| [hmcslock_release](hmcslock.h.md#function-hmcslock_release) | Releases the given lock.  |

##  Function `hmcslock_which_lock`

```c
static hmcslock_t* hmcslock_which_lock(hmcslock_t *locks, vsize_t locks_len, hmcslock_level_spec_t *level_specs, vsize_t num_levels, vuint32_t cores_per_node, vuint32_t core_id)
``` 
_Returns the address of the lock associated with the core id._ 




**Parameters:**

- `locks`: array of hmcslock_t objects. 
- `locks_len`: `locks` array length. 
- `level_specs`: array of hmcslock_level_spec_t objects. 
- `num_levels`: number of the levels in hierarchy, same as the length of `level_specs`. 
- `cores_per_node`: number of cores per leaf lock node. 
- `core_id`: core id of the calling thread.


**Returns:** hmcslock_t* address of hmcslock_t object, which the calling thread should use to acquire the lock.

**Returns:** NULL when `HMCS_MAX_THREADS` is too small. 



##  Function `hmcslock_init`

```c
static void hmcslock_init(hmcslock_t *locks, vsize_t locks_len, hmcslock_level_spec_t *level_specs, vsize_t num_levels)
``` 
_Initializes the locks array._ 




**Parameters:**

- `locks`: array of hmcslock_t objects. 
- `locks_len`: `locks` array length. 
- `level_specs`: array of hmcslock_level_spec_t objects. 
- `num_levels`: number of the levels in heirarchy, same as the length of `level_specs`.


> **Note:** e.g., say you have three levels, the machine, two NUMA nodes, and two caches per NUMA. Then `locks_len = 1(machine) + 2(NUMA) + 4 (2*2 caches) = 7`. `num_levels = 3` (including the machine level) define levels_spec as follows: 

```c
level_specs[num_levels] = {
  {1, LEVEL_THRESHOLD}, // 1 machine
  {2, LEVEL_THRESHOLD}, // 2 NUMAs per machine
  {2, LEVEL_THRESHOLD}, // 2 caches per NUMA
}
```

 


##  Function `hmcslock_acquire`

```c
static void hmcslock_acquire(hmcslock_t *lock, hmcs_node_t *qnode, vsize_t num_levels)
``` 
_Acquires the given lock._ 




**Parameters:**

- `lock`: address of hmcslock_t object. 
- `qnode`: address of hmcs_node_t object. 
- `num_levels`: number of levels including machine level.


> **Note:** `lock` should be what `hmcslock_which_lock` returned. 


##  Function `hmcslock_release`

```c
static void hmcslock_release(hmcslock_t *lock, hmcs_node_t *qnode, vsize_t num_levels)
``` 
_Releases the given lock._ 




**Parameters:**

- `lock`: address of hmcslock_t object. 
- `qnode`: address of hmcs_node_t object. 
- `num_levels`: number of levels including machine level.


> **Note:** `lock` should be what `hmcslock_which_lock` returned. 



---
