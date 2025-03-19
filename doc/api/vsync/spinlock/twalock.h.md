#  [vsync](../README.md) / [spinlock](README.md) / twalock.h
_Ticketlock with waiting array (TWA)._ 

To use the TWA lock, one must declare the global waiting array once in the program. Use `TWALOCK_ARRAY_DECL` to declare the array where convenient.


### Example:



```c

#include <vsync/spinlock/twalock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            12
#define EXPECTED_VAL N

TWALOCK_ARRAY_DECL;
twalock_t g_lock = TWALOCK_INIT();

vuint32_t g_x = 0;
vuint32_t g_y = 0;

void *
run(void *args)
{
    twalock_acquire(&g_lock);
    g_x++;
    g_y++;
    twalock_release(&g_lock);

    (void)args;
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

Dice and Kogan - [TWA - Ticket Locks Augmented with a Waiting Array, EuroPar-19] 

---
# Macros 

| Macro | Description |
|---|---|
| [TWALOCK_ARRAY_DECL](twalock.h.md#macro-twalock_array_decl) | TWALOCK_ARRAY_DECL declares the global g_vtwa_array variable.  |
| [TWALOCK_INIT](twalock.h.md#macro-twalock_init) | Initializer of `twalock_t`.  |

##  Macro `TWALOCK_ARRAY_DECL`

 
_TWALOCK_ARRAY_DECL declares the global g_vtwa_array variable._ 



##  Macro `TWALOCK_INIT`

```c
TWALOCK_INIT()
```

 
_Initializer of_ `twalock_t`_._ 



---
# Functions 

| Function | Description |
|---|---|
| [twalock_init](twalock.h.md#function-twalock_init) | Initializes the given TWA lock.  |
| [twalock_acquire](twalock.h.md#function-twalock_acquire) | Acquires the given TWA lock.  |
| [twalock_release](twalock.h.md#function-twalock_release) | Releases the given TWA lock.  |
| [twalock_tryacquire](twalock.h.md#function-twalock_tryacquire) | Tries to acquire the given TWA lock.  |

##  Function `twalock_init`

```c
static void twalock_init(twalock_t *l)
``` 
_Initializes the given TWA lock._ 




**Parameters:**

- `l`: address of twalock_t object.


> **Note:** alternatively use TWALOCK_INIT 


##  Function `twalock_acquire`

```c
static void twalock_acquire(twalock_t *l)
``` 
_Acquires the given TWA lock._ 




**Parameters:**

- `l`: address of twalock_t object. 




##  Function `twalock_release`

```c
static void twalock_release(twalock_t *l)
``` 
_Releases the given TWA lock._ 




**Parameters:**

- `l`: address of twalock_t object. 




##  Function `twalock_tryacquire`

```c
static vbool_t twalock_tryacquire(twalock_t *l)
``` 
_Tries to acquire the given TWA lock._ 




**Parameters:**

- `l`: address of twalock_t object. 


**Returns:** true on success. 

**Returns:** false on failure. 




---
