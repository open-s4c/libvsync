#  [vsync](../README.md) / [spinlock](README.md) / seqcount.h
_Lightweight single-writer multi-reader optimistic lock._ 

A seqcount allows a writer to modify shared variables without being blocked by readers. Readers optimistically read the shared variables and subsequently check their consistency. If the shared variables were read while a write occurred, the readers must retry.

> **Note:** this algorithm is suitable for one writer, multiple readers. See [vsync/spinlock/seqlock.h](seqlock.h.md) for multiple writer support.


### Example:



```c
#include <vsync/spinlock/seqcount.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define IT           10
#define EXPECTED_VAL IT

seqcount_t g_seqcount = SEQCOUNT_INIT();
vuint32_t g_x         = 0;
vuint32_t g_y         = 0;

void
writer(void)
{
    for (vsize_t i = 0; i < IT; i++) {
        seqvalue_t seq = seqcount_wbegin(&g_seqcount);
        g_x++;
        g_y++;
        seqcount_wend(&g_seqcount, seq);
    }
}

void
reader(void)
{
    vuint32_t a  = 0;
    vuint32_t b  = 0;
    seqvalue_t s = 0;

    do {
        s = seqcount_rbegin(&g_seqcount);
        a = g_x;
        b = g_y;
    } while (!seqcount_rend(&g_seqcount, s));

    /* what we read must be consistent */
    ASSERT(a == b);
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;
    if (tid == 0) {
        /* one writer at most */
        writer();
    } else {
        reader();
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




### References:
 [The seqcount latch lock type](https://lwn.net/Articles/831540/) 

---
# Macros 

| Macro | Description |
|---|---|
| [SEQCOUNT_INIT](seqcount.h.md#macro-seqcount_init) | Initializer of `seqcount_t`.  |

##  Macro `SEQCOUNT_INIT`

```c
SEQCOUNT_INIT()
```

 
_Initializer of_ `seqcount_t`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [seqcount_init](seqcount.h.md#function-seqcount_init) | Initializes seqcount.  |
| [seqcount_wbegin](seqcount.h.md#function-seqcount_wbegin) | Begins writer critical section.  |
| [seqcount_wend](seqcount.h.md#function-seqcount_wend) | Ends writer critical section.  |
| [seqcount_rbegin](seqcount.h.md#function-seqcount_rbegin) | Begins reader critical section.  |
| [seqcount_rend](seqcount.h.md#function-seqcount_rend) | Tries to end the reader critical section.  |

##  Function `seqcount_init`

```c
static void seqcount_init(seqcount_t *sc)
``` 
_Initializes seqcount._ 




**Parameters:**

- `sc`: address of seqcount_t object.


> **Note:** alternatively use `SEQCOUNT_INIT`. 


##  Function `seqcount_wbegin`

```c
static seqvalue_t seqcount_wbegin(seqcount_t *sc)
``` 
_Begins writer critical section._ 


This function can only be called by a single thread. If multiple writers are necessary, the user has to protect this call with an additional lock. Alternatively [vsync/spinlock/seqlock.h](seqlock.h.md) can be used instead.



**Parameters:**

- `sc`: address of seqcount_t object. 


**Returns:** sequence value.

> **Note:** Can be called by a single thread. 


##  Function `seqcount_wend`

```c
static void seqcount_wend(seqcount_t *sc, seqvalue_t s)
``` 
_Ends writer critical section._ 




**Parameters:**

- `sc`: address of seqcount_t object. 
- `s`: sequence value returned by `seqcount_wbegin`. 




##  Function `seqcount_rbegin`

```c
static seqvalue_t seqcount_rbegin(seqcount_t *sc)
``` 
_Begins reader critical section._ 


> **Note:** This function can be called by multiple reader threads.



**Parameters:**

- `sc`: address of seqcount_t object. 


**Returns:** sequence value. 



##  Function `seqcount_rend`

```c
static vbool_t seqcount_rend(seqcount_t *sc, seqvalue_t s)
``` 
_Tries to end the reader critical section._ 




**Parameters:**

- `sc`: address of seqcount_t object. 
- `s`: sequence value returned by `seqcount_rbegin`. 


**Returns:** true, reader CS can be ended, read data is consistent. 

**Returns:** false, a write took place, data is inconsistent and reader need to repeat the CS. 




---
