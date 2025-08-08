#  [vsync](../README.md) / [map](README.md) / simpleht.h
_Simple lock-free hashtable._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md)

This is an adaptation of the simple lock-free hashtable mentioned in the reference. The adaptation allow for remove operation if users need it. Users should be aware that enabling the remove incurs overhead on add/get operations. The remove operation can be quite slow as it rebalances the hashtable and when that happens lock-freedom is compromised.

This hashtable is ideal for when no remove operations are needed, in that case users should disable the support of remove by compiling with `-DVSIMPLEHT_DISABLE_REMOVE`. Or keep it, and make sure remove is rarely called.

# Operating Conditions

The hashtable is supposed to keep gaps for optimal performance. Users should create the table with at least a capacity of 2*N, where N is the maximum number of entries to be inserted.

As long as the hashtable is not full, every search is guaranteed to finish either by locating the desired key, or by locating an entry whose key is zero, which means that the desired key does not exist in the hashtable.

There is a tradeoff between performance and the number of empty slots in the hashtable. The performance degrades as the hashtable fills. lock-freedom of add is compromised if the hashtable becomes full.


### Example:



```c
#include <vsync/map/simpleht.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N                  3U
#define HASHTABLE_CAPACITY 32U
#define MIN_KEY            1U
#define MAX_KEY            (HASHTABLE_CAPACITY / 2)

typedef struct data_s {
    vuintptr_t key;
    // data
} data_t;

vsimpleht_t g_hashtable;

vuint64_t
hash_cb(vuintptr_t key)
{
    vuint64_t h = key;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

vint8_t
cmp_key_cb(vuintptr_t a, vuintptr_t b)
{
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    } else {
        return 0;
    }
}

void
destroy_cb(void *data)
{
    free(data);
}

void *
run(void *args)
{
    vsize_t tid         = (vsize_t)(vuintptr_t)args;
    data_t *data        = NULL;
    vsimpleht_ret_t ret = VSIMPLEHT_RET_OK;

    vsimpleht_thread_register(&g_hashtable);

    for (vuintptr_t key = MIN_KEY; key < MAX_KEY; key++) {
        data = vsimpleht_get(&g_hashtable, key);
        if (data) {
            printf("T%zu: found key %lu\n", tid, key);
            ASSERT(data->key == key);
            ret = vsimpleht_remove(&g_hashtable, key);
            if (ret == VSIMPLEHT_RET_OK) {
                printf("T%zu: removed key %lu\n", tid, key);
            } else {
                printf("T%zu: key %lu does not exist\n", tid, key);
            }
        } else {
            data      = malloc(sizeof(data_t));
            data->key = key;
            ret       = vsimpleht_add(&g_hashtable, key, data);
            if (ret == VSIMPLEHT_RET_OK) {
                printf("T%zu: added key %lu\n", tid, key);
            } else if (ret == VSIMPLEHT_RET_TBL_FULL) {
                printf("T%zu: table is full! cannot add\n", tid);
                free(data);
            } else if (ret == VSIMPLEHT_RET_KEY_EXISTS) {
                printf("T%zu: key %lu already exists\n", tid, key);
                free(data);
            }
        }
    }
    vsimpleht_thread_deregister(&g_hashtable);
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    vsize_t size = vsimpleht_buff_size(HASHTABLE_CAPACITY);
    void *buffer = malloc(size);

    vsimpleht_init(&g_hashtable, buffer, HASHTABLE_CAPACITY, cmp_key_cb,
                   hash_cb, destroy_cb);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    vsimpleht_destroy(&g_hashtable);
    free(buffer);
    return 0;
}
```




### References:

[The World's Simplest Lock-Free Hash Table](https://preshing.com/20130605/the-worlds-simplest-lock-free-hash-table/) 

---
# Macros 

| Macro | Description |
|---|---|
| [VSIMPLEHT_RELATIVE_THRESHOLD](simpleht.h.md#macro-vsimpleht_relative_threshold) | Controls when the rebalancing of the hashtable is triggered.  |

##  Macro `VSIMPLEHT_RELATIVE_THRESHOLD`

 
_Controls when the rebalancing of the hashtable is triggered._ 


Everytime the number of removed nodes is equal to capacity/VSIMPLEHT_RELATIVE_THRESHOLD. 


---
# Functions 

| Function | Description |
|---|---|
| [vsimpleht_buff_size](simpleht.h.md#function-vsimpleht_buff_size) | Calculates the size of the buffer needed by the hashtable.  |
| [vsimpleht_init](simpleht.h.md#function-vsimpleht_init) | Initializes the hashtable.  |
| [vsimpleht_destroy](simpleht.h.md#function-vsimpleht_destroy) | Destroys the stored hashtable values.  |
| [vsimpleht_thread_register](simpleht.h.md#function-vsimpleht_thread_register) | Registers the caller thread.  |
| [vsimpleht_thread_deregister](simpleht.h.md#function-vsimpleht_thread_deregister) | Deregisters the caller thread.  |
| [vsimpleht_add](simpleht.h.md#function-vsimpleht_add) | Inserts the given value into the hashtable.  |
| [vsimpleht_get](simpleht.h.md#function-vsimpleht_get) | Searches the hashtable for a value associated with the given key.  |
| [vsimpleht_iter_init](simpleht.h.md#function-vsimpleht_iter_init) | Initializes the given iterator object `iter` for operating `tbl` object.  |
| [vsimpleht_iter_next](simpleht.h.md#function-vsimpleht_iter_next) | Reads the current entry (key/value) the iterator has reached.  |
| [vsimpleht_remove](simpleht.h.md#function-vsimpleht_remove) | Removes the node associated with the given key from the hashtable.  |

##  Function `vsimpleht_buff_size`

```c
static vsize_t vsimpleht_buff_size(vsize_t capacity)
``` 
_Calculates the size of the buffer needed by the hashtable._ 




**Parameters:**

- `capacity`: capacity of the hashtable. Maximum number of items to be inserted in the hashtable. 


**Returns:** vsize_t required buffer size to fit `len` items.

> **Note:** capacity must be power of two. 


##  Function `vsimpleht_init`

```c
static void vsimpleht_init(vsimpleht_t *tbl, void *buff, vsize_t capacity, vsimpleht_cmp_key_t cmp_fun, vsimpleht_hash_key_t hash_fun, vsimpleht_destroy_entry_t destroy_cb)
``` 
_Initializes the hashtable._ 




**Parameters:**

- `tbl`: address of vsimpleht_t object. 
- `buff`: address of the hashtable buffer. Allocated by the user. 
- `capacity`: capacity of the hashtable. Same value passed to vsimpleht_buff_size. 
- `cmp_key`: compare key callback function address. 
- `hash_key`: address of callback function. Used to hash the key. 
- `destroy_cb`: address of destroy callback function. Used to destroy the hashtable stored objects. Called via `vsimpleht_remove` and `vsimpleht_destroy`. 




##  Function `vsimpleht_destroy`

```c
static void vsimpleht_destroy(vsimpleht_t *tbl)
``` 
_Destroys the stored hashtable values._ 




**Parameters:**

- `tbl`: address of vsimpleht_t object. 




##  Function `vsimpleht_thread_register`

```c
static void vsimpleht_thread_register(vsimpleht_t *tbl)
``` 
_Registers the caller thread._ 




**Parameters:**

- `tbl`: address of vsimpleht_t object.


> **Note:** should be called once per thread, before the calling thread accesses the hashtable for the first time. 


##  Function `vsimpleht_thread_deregister`

```c
static void vsimpleht_thread_deregister(vsimpleht_t *tbl)
``` 
_Deregisters the caller thread._ 




**Parameters:**

- `tbl`: address of vsimpleht_t object.


> **Note:** should be called once per thread, after the calling thread accesses the hashtable for the last time. 


##  Function `vsimpleht_add`

```c
static vsimpleht_ret_t vsimpleht_add(vsimpleht_t *tbl, vuintptr_t key, void *value)
``` 
_Inserts the given value into the hashtable._ 




**Parameters:**

- `tbl`: address of vsimpleht_t object. 
- `key`: key of the value to add. 
- `value`: address of the object to insert. 


**Returns:** VSIMPLEHT_RET_OK key does not exist, value was added. 

**Returns:** VSIMPLEHT_RET_KEY_EXISTS key exists, value was not added. 

**Returns:** VSIMPLEHT_RET_TBL_FULL table is full. Consider
- increasing the capacity
- decreasing VSIMPLEHT_RELATIVE_THRESHOLD
- removing items from the table> **Note:** neither key can be 0 nor value can be NULL. 




##  Function `vsimpleht_get`

```c
static void* vsimpleht_get(vsimpleht_t *tbl, vuintptr_t key)
``` 
_Searches the hashtable for a value associated with the given key._ 




**Parameters:**

- `tbl`: address of vsimpleht_t object. 
- `key`: key to search for. 


**Returns:** void* address of the object associated with the given key if exists. 

**Returns:** NULL if there is no value found associated with given key. 



##  Function `vsimpleht_iter_init`

```c
static void vsimpleht_iter_init(vsimpleht_t *tbl, vsimpleht_iter_t *iter)
``` 
_Initializes the given iterator object_ `iter` _for operating_ `tbl` _object._ 




**Parameters:**

- `tbl`: address of vsimpleht_t object. 
- `iter`: address of vsimpleht_iter_t object to initialize. 




##  Function `vsimpleht_iter_next`

```c
static vbool_t vsimpleht_iter_next(vsimpleht_iter_t *iter, vuintptr_t *key, void **val)
``` 
_Reads the current entry (key/value) the iterator has reached._ 


After the values are read, the iterator advances to the next entry.



**Parameters:**

- `iter`: address of vsimpleht_iter_t object. 
- `key`: output parameter where the current key value is stored. 
- `val`: output parameter where the current object value is stored. 


**Returns:** true the current output of key and val is valid. 

**Returns:** false there are no more objects to iterate. Stored values in key, val parameters should be ignored by the user.

**Precondition:** iter must be initalized via vsimpleht_iter_init.

> **Note:** the iterator is mainly meant for sequential setting. When called in parallel the results might not be consistent.


### Example:



```c
vuintptr_t key = 0;
data_t *date = NULL;
vsimpleht_iter_t iter;
vsimpleht_iter_init(&g_simpleht, &iter);
 while (vsimpleht_iter_next(&iter, &key, (void **)&data)) {
     // access key
     // access data
 }
```

 


##  Function `vsimpleht_remove`

```c
static vsimpleht_ret_t vsimpleht_remove(vsimpleht_t *tbl, vuintptr_t key)
``` 
_Removes the node associated with the given key from the hashtable._ 




**Parameters:**

- `tbl`: address of vsimpleht_t object. 
- `key`: key to remove the value associated with. 


**Returns:** VSIMPLEHT_RET_OK key exists, node was removed. 

**Returns:** VSIMPLEHT_RET_KEY_DNE key does not exist.

> **Note:** this operation is not lock-free, if the the removal is successful. It can be super-slow, and the associated value can be freed via the destroy callback registered at `vsimpleht_init`. 



---
