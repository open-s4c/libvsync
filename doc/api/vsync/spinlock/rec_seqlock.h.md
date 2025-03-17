#  [vsync](../README.md) / [spinlock](README.md) / rec_seqlock.h
_Recursive seqlock implementation using recursive.h._ 

In a rec_seqlock writers are only blocked by writers, not by readers. Readers optimistically read the shared variables and subsequently check their consistency. If any of the shared variables has been updated while being read, the readers must retry

This is a reentrant implementation of  (see [seqlock.h](seqlock.h.md).)

### Example:



```c

#include <vsync/spinlock/rec_seqlock.h>
#include <pthread.h>
#include <stdio.h>

#define N            10
#define EXPECTED_VAL (N / 2)

rec_seqlock_t g_lock = REC_SEQLOCK_INIT();
vuint32_t g_x        = 0;
vuint32_t g_y        = 0;

void
writer(vuint32_t tid)
{
    rec_seqlock_acquire(&g_lock, tid);
    // writer acquisition of the lock can be nested
    rec_seqlock_acquire(&g_lock, tid);
    g_x++;
    g_y++;
    rec_seqlock_release(&g_lock);
    rec_seqlock_release(&g_lock);
}

void
reader(void)
{
    vuint32_t a  = 0;
    vuint32_t b  = 0;
    seqvalue_t s = 0;

    do {
        s = rec_seqlock_rbegin(&g_lock);
        a = g_x;
        b = g_y;
    } while (!rec_seqlock_rend(&g_lock, s));

    /* what we read must be consistent */
    ASSERT(a == b);
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;
    if (tid % 2 == 0) {
        reader();
    } else {
        writer((vuint32_t)tid);
    }
    return NULL;
}

int
main(void)
{
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

 

---
# Macros 

| Macro | Description |
|---|---|
| [REC_SEQLOCK_INIT](rec_seqlock.h.md#macro-rec_seqlock_init) | Initializer of `rec_seqlock`.  |

##  Macro `REC_SEQLOCK_INIT`

```c
REC_SEQLOCK_INIT()
```

 
_Initializer of_ `rec_seqlock`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [rec_seqlock_init](rec_seqlock.h.md#function-rec_seqlock_init) | Initializes the given recursive seqlock.  |
| [rec_seqlock_acquire](rec_seqlock.h.md#function-rec_seqlock_acquire) | Acquires the given recursive seqlock.  |
| [rec_seqlock_release](rec_seqlock.h.md#function-rec_seqlock_release) | Releases the given recursive seqlock.  |
| [rec_seqlock_rbegin](rec_seqlock.h.md#function-rec_seqlock_rbegin) | Marks beginning of reader critical section.  |
| [rec_seqlock_rend](rec_seqlock.h.md#function-rec_seqlock_rend) | Ends reader critical section.  |

##  Function `rec_seqlock_init`

```c
static void rec_seqlock_init(struct rec_seqlock_s *l)
``` 
_Initializes the given recursive seqlock._ 




**Parameters:**

- `l`: address of rec_seqlock_t object. 




##  Function `rec_seqlock_acquire`

```c
static void rec_seqlock_acquire(struct rec_seqlock_s *l, vuint32_t id)
``` 
_Acquires the given recursive seqlock._ 




**Parameters:**

- `l`: address of rec_seqlock_t object. 
- `id`: thread ID or core ID.


> **Note:** called by writer threads. 


##  Function `rec_seqlock_release`

```c
static void rec_seqlock_release(struct rec_seqlock_s *l)
``` 
_Releases the given recursive seqlock._ 




**Parameters:**

- `l`: address of rec_seqlock_t object.


> **Note:** called by writer threads. 


##  Function `rec_seqlock_rbegin`

```c
static seqvalue_t rec_seqlock_rbegin(rec_seqlock_t *l)
``` 
_Marks beginning of reader critical section._ 


Readers must call this function, before attempting to read. * This function returns a value that must be later passed to `rec_seqlock_rend`.


**Postcondition:** readers must call `rec_seqlock_rend` once they are done accessing the critical section.


**Parameters:**

- `l`: address of rec_seqlock_t object. 


**Returns:** `seqvalue_t` an unsigned integer value.

> **Note:** called by reader threads. 


##  Function `rec_seqlock_rend`

```c
static vbool_t rec_seqlock_rend(rec_seqlock_t *l, seqvalue_t sv)
``` 
_Ends reader critical section._ 


Users should rely on the return value to decide if repeating is necessary.


**Precondition:** readers must call `rec_seqlock_rbegin` before reading critical section data.


**Parameters:**

- `l`: address of rec_seqlock_t object. 
- `sv`: the value `rec_seqlock_rbegin` returned, before the read attempt 


**Returns:** true read data is consistent. 

**Returns:** false read data is inconsistent, reader must reattempt the read.

> **Note:** called by reader threads. 



---
