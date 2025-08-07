#  [vsync](../README.md) / [spinlock](README.md) / arraylock.h
_Simple array-based queue lock._ 

**Groups:** [Fair locks](GROUP_fair_lock.md)

The array lock has an array of flags, each slot is associated with a thread. If we have `N` threads then the array has `N` slots. Each slot represents a boolean flag indicating the associated thread's permission to acquire the lock. The thread waits on its flag to become `true` to acquire the lock. The thread releases the lock, by giving away its permission to the next thread, i.e. sets its own flag to `false` and the flag of the one next in line to `true`.

Initially the first flag is set to `true` and the rest to `false`, and the tail holds the index of the first slot.


### Example:



```c
#include <vsync/spinlock/arraylock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

#define NEXT_POWER_2 16U

arraylock_flag_t g_flags[NEXT_POWER_2];
arraylock_t g_lock;

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    vuint32_t slot = 0;

    arraylock_acquire(&g_lock, &slot);
    g_x++;
    g_y++;
    arraylock_release(&g_lock, slot);

    (void)args;
    return NULL;
}

int
main(void)
{
    arraylock_init(&g_lock, g_flags, NEXT_POWER_2);

    pthread_t threads[N];

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
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

Maurice Herlihy, Nir Shavit - [The Art of Multiprocessor Programming 7.5.1](https://dl.acm.org/doi/pdf/10.5555/2385452) 

---
# Functions 

| Function | Description |
|---|---|
| [arraylock_init](arraylock.h.md#function-arraylock_init) | Initializes the given lock object.  |
| [arraylock_acquire](arraylock.h.md#function-arraylock_acquire) | Acquires the given lock.  |
| [arraylock_release](arraylock.h.md#function-arraylock_release) | Releases the given lock.  |

##  Function `arraylock_init`

```c
static void arraylock_init(arraylock_t *lock, arraylock_flag_t *flags, const vuint32_t len)
``` 
_Initializes the given lock object._ 




**Parameters:**

- `lock`: address of arraylock_t object. 
- `flags`: address of arraylock_flag_t object. 
- `len`: next power of two greater or equal to the maximum number of threads. Length of `flags`array.


> **Note:** `len` MUST BE A POWER OF TWO. This is required because we use a circular array and take advantage of unsigned integers "overflow". 


##  Function `arraylock_acquire`

```c
static void arraylock_acquire(arraylock_t *lock, vuint32_t *slot)
``` 
_Acquires the given lock._ 




**Parameters:**

- `lock`: address of arraylock_t object. 
- `slot`: output parameter. Stores the index of the slot associates with the calling thread.


> **Note:** the same value returned in `slot` must be passed intact to `arraylock_release`. 


##  Function `arraylock_release`

```c
static void arraylock_release(arraylock_t *lock, vuint32_t slot)
``` 
_Releases the given lock._ 




**Parameters:**

- `lock`: address of arraylock_t object. 
- `slot`: index of the slot associates with the calling thread. 





---
