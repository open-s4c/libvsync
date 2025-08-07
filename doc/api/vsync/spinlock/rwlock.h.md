#  [vsync](../README.md) / [spinlock](README.md) / rwlock.h
_Write-preferring rwlock based on semaphore and an atomic vbool_t._ 

see [vsync/spinlock/semaphore.h](semaphore.h.md)

When acquiring a read lock, wait for writers and then reserve one resource. When acquiring a write lock, lock writer bit and reserve all N resources.

Algorithm:



```
write_acquire:
 await (wb = 0)
 wb = 1

    await (resources = N)
    resources = 0

write_release:
    wb = 0
    resources = N

read_acquire:
 await (wb = 0)

    await (resources > 0)
    resources--

read_release:
    resources++
```




### Example:



```c

#include <vsync/spinlock/rwlock.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>

#define N            24
#define EXPECTED_VAL (N / 2)

rwlock_t g_lock = RWLOCK_INIT();
vuint32_t g_x   = 0;
vuint32_t g_y   = 0;

void
writer(void)
{
    rwlock_write_acquire(&g_lock);
    g_x++;
    g_y++;
    rwlock_write_release(&g_lock);
}

void
reader(void)
{
    vuint32_t a = 0;
    vuint32_t b = 0;

    rwlock_read_acquire(&g_lock);
    a = g_x;
    b = g_y;
    rwlock_read_release(&g_lock);

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

 

---
# Macros 

| Macro | Description |
|---|---|
| [RWLOCK_INITN](rwlock.h.md#macro-rwlock_initn) | Initializer of `rwlock_t`.  |
| [RWLOCK_INIT](rwlock.h.md#macro-rwlock_init) | Initializer of `rwlock_t` with the default number of resources.  |

##  Macro `RWLOCK_INITN`

```c
RWLOCK_INITN(N)
```

 
_Initializer of_ `rwlock_t`_._ 



##  Macro `RWLOCK_INIT`

```c
RWLOCK_INIT()
```

 
_Initializer of_ `rwlock_t` _with the default number of resources._ 



---
# Functions 

| Function | Description |
|---|---|
| [rwlock_init](rwlock.h.md#function-rwlock_init) | Initializes the rwlock.  |
| [rwlock_write_acquire](rwlock.h.md#function-rwlock_write_acquire) | Acquires the write lock.  |
| [rwlock_write_tryacquire](rwlock.h.md#function-rwlock_write_tryacquire) | Tries to acquire the write lock.  |
| [rwlock_write_release](rwlock.h.md#function-rwlock_write_release) | Releases the write lock.  |
| [rwlock_read_acquire](rwlock.h.md#function-rwlock_read_acquire) | Acquires the read lock.  |
| [rwlock_read_tryacquire](rwlock.h.md#function-rwlock_read_tryacquire) | Tries to acquire the read lock.  |
| [rwlock_read_release](rwlock.h.md#function-rwlock_read_release) | Releases the read lock.  |
| [rwlock_acquired_by_writer](rwlock.h.md#function-rwlock_acquired_by_writer) | Returns true if a writer has acquired the lock, or waiting on the readers to release it.  |
| [rwlock_acquired_by_readers](rwlock.h.md#function-rwlock_acquired_by_readers) | Returns true if the lock is acquired by readers.  |

##  Function `rwlock_init`

```c
static void rwlock_init(rwlock_t *l)
``` 
_Initializes the rwlock._ 




**Parameters:**

- `l`: address of rwlock_t object.


> **Note:** alternatively use `RWLOCK_INIT`. 


##  Function `rwlock_write_acquire`

```c
static void rwlock_write_acquire(rwlock_t *l)
``` 
_Acquires the write lock._ 




**Parameters:**

- `l`: address of rwlock_t object. 




##  Function `rwlock_write_tryacquire`

```c
static vbool_t rwlock_write_tryacquire(rwlock_t *l)
``` 
_Tries to acquire the write lock._ 




**Parameters:**

- `l`: address of rwlock_t object. 


**Returns:** true, if lock is acquired successfully. 

**Returns:** false, if failed to acquire the lock. 



##  Function `rwlock_write_release`

```c
static void rwlock_write_release(rwlock_t *l)
``` 
_Releases the write lock._ 




**Parameters:**

- `l`: address of rwlock_t object. 




##  Function `rwlock_read_acquire`

```c
static void rwlock_read_acquire(rwlock_t *l)
``` 
_Acquires the read lock._ 




**Parameters:**

- `l`: address of rwlock_t object. 




##  Function `rwlock_read_tryacquire`

```c
static vbool_t rwlock_read_tryacquire(rwlock_t *l)
``` 
_Tries to acquire the read lock._ 




**Parameters:**

- `l`: address of rwlock_t object. 


**Returns:** true, if lock is acquired successfully. 

**Returns:** false, if failed to acquire the lock. 



##  Function `rwlock_read_release`

```c
static void rwlock_read_release(rwlock_t *l)
``` 
_Releases the read lock._ 




**Parameters:**

- `l`: address of rwlock_t object. 




##  Function `rwlock_acquired_by_writer`

```c
static vbool_t rwlock_acquired_by_writer(rwlock_t *l)
``` 
_Returns true if a writer has acquired the lock, or waiting on the readers to release it._ 




**Parameters:**

- `l`: address of rwlock_t object. 


**Returns:** true a writer has acquired or waiting on readers to release the lock. 

**Returns:** false the lock is not acquired by a writer. 



##  Function `rwlock_acquired_by_readers`

```c
static vbool_t rwlock_acquired_by_readers(rwlock_t *l)
``` 
_Returns true if the lock is acquired by readers._ 




**Parameters:**

- `l`: address of rwlock_t object. 


**Returns:** true if the lock is acquired by readers. 

**Returns:** false if the lock is not acquired by readers. 




---
