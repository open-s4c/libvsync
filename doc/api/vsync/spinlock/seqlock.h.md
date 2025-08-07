#  [vsync](../README.md) / [spinlock](README.md) / seqlock.h
_Multi-writer multi-reader optimistic lock._ 

In seqlock writers are only blocked by writers but not by readers. Readers optimistically read shared variables and subsequently check their consistency. If any of the shared variables has been updated while being read, the readers must retry.


### Example:



```c

#include <vsync/spinlock/seqlock.h>
#include <pthread.h>
#include <stdio.h>

#define N            1000
#define EXPECTED_VAL (N / 2)

seqlock_t g_lock = SEQ_LOCK_INIT();
vuint32_t g_x    = 0;
vuint32_t g_y    = 0;

void
writer(void)
{
    seqlock_acquire(&g_lock);
    g_x++;
    g_y++;
    seqlock_release(&g_lock);
}

void
reader(void)
{
    vuint32_t a  = 0;
    vuint32_t b  = 0;
    seqvalue_t s = 0;

    do {
        s = seqlock_rbegin(&g_lock);
        a = g_x;
        b = g_y;
    } while (!seqlock_rend(&g_lock, s));

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
        writer();
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



> **Note:** If you have a single writer [seqcount.h](seqcount.h.md) might be enough. 

---
# Macros 

| Macro | Description |
|---|---|
| [SEQ_LOCK_INIT](seqlock.h.md#macro-seq_lock_init) | Initializer of `seqlock_t`.  |

##  Macro `SEQ_LOCK_INIT`

```c
SEQ_LOCK_INIT()
```

 
_Initializer of_ `seqlock_t`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [seqlock_init](seqlock.h.md#function-seqlock_init) | Initializes the seqlock.  |
| [seqlock_acquire](seqlock.h.md#function-seqlock_acquire) | Acquires the lock in write-mode.  |
| [seqlock_release](seqlock.h.md#function-seqlock_release) | Releases the lock in write-mode.  |
| [seqlock_rbegin](seqlock.h.md#function-seqlock_rbegin) | Begins reader critical section.  |
| [seqlock_rend](seqlock.h.md#function-seqlock_rend) | Ends reader critical section.  |

##  Function `seqlock_init`

```c
static void seqlock_init(seqlock_t *seq)
``` 
_Initializes the seqlock._ 




**Parameters:**

- `seq`: address of seqlock_t object.


> **Note:** alternatively use `SEQ_LOCK_INIT`. 


##  Function `seqlock_acquire`

```c
static void seqlock_acquire(seqlock_t *seq)
``` 
_Acquires the lock in write-mode._ 




**Parameters:**

- `seq`: address of seqlock_t object.


**Postcondition:** once writers are done accessing the critical section, they must call `seqlock_release`.

> **Note:** only writers need to call this function, in order to enter the critical section. 


##  Function `seqlock_release`

```c
static void seqlock_release(seqlock_t *seq)
``` 
_Releases the lock in write-mode._ 




**Parameters:**

- `seq`: address of seqlock_t object.


**Precondition:** prior to accessing the critical section, writers must call `seqlock_acquire`.

> **Note:** only writers need to call this function, once they are done accessing the critical section. 


##  Function `seqlock_rbegin`

```c
static seqvalue_t seqlock_rbegin(seqlock_t *seq)
``` 
_Begins reader critical section._ 


This function returns a value that must be later passed to `seqlock_rend`.



**Parameters:**

- `seq`: address of seqlock_t object.


**Returns:** `seqvalue_t` an unsigned integer value.

**Postcondition:** readers must call `seqlock_rend` once they are done accessing the critical section. 



##  Function `seqlock_rend`

```c
static vbool_t seqlock_rend(seqlock_t *seq, seqvalue_t sv)
``` 
_Ends reader critical section._ 




**Parameters:**

- `seq`: address of seqlock_t object. 
- `sv`: the value `seqlock_rbegin` returned, before the read attempt 


**Returns:** true, if no write happened in between. 

**Returns:** false, if a write happened in between, and repeating the CS is required to read consistent data.

**Precondition:** readers must call `seqlock_rbegin` before reading critical section data. 




---
