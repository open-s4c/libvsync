#  [vsync](../README.md) / [map](README.md) / treeset_rb_fine.h
_This implementation of treeset uses balanced red-black tree (RB) and fine-grained locking._ 

**Groups:** [Linearizable](../GROUP_linearizable.md)

Refer to [treeset_bst_coarse.h](treeset_bst_coarse.h.md) for more general information about treeset.


### Example:



```c
#define TREESET_LOCK_RW
#include <vsync/map/treeset_rb_fine.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N       4
#define MIN_KEY 0
#define MAX_KEY 3

typedef vuintptr_t value_t;

treeset_t tree;

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;

    for (treeset_key_t key = MIN_KEY; key <= MAX_KEY; key++) {
        value_t value = tid;
        value_t old_value;

        // insert
        vbool_t res =
            treeset_add(&tree, key, (void *)value, (void *)&old_value);

        if (res) {
            // insert succeeded
            printf("[%lu] key %lu inserted\n", tid, key);
        } else {
            printf(
                "[%lu] key %lu not inserted, already in tree with value %lu\n",
                tid, key, old_value);
        }

        // search
        res = treeset_contains(&tree, key, (void *)&old_value);

        if (res) {
            // search successful
            printf("[%lu] key %lu in tree with value %lu\n", tid, key,
                   old_value);
        } else {
            printf("[%lu] key %lu not in tree\n", tid, key);
        }

        // remove
        res = treeset_remove(&tree, key, (void *)&old_value);

        if (res) {
            // remove succeeded
            printf("[%lu] key %lu removed, old value was %lu\n", tid, key,
                   old_value);
        } else {
            printf("[%lu] key %lu not removed\n", tid, key);
        }
    }

    return NULL;
}

void *
malloc_cb(vsize_t sz, void *arg)
{
    (void)arg;
    return malloc(sz);
}

void
free_cb(void *ptr, void *arg)
{
    (void)arg;
    free(ptr);
}

void
free_visitor(treeset_key_t key, void *value, void *arg)
{
    (void)key;
    (void)arg;
    free((value_t *)value);
}

int
main(void)
{
    pthread_t threads[N];

    vmem_lib_t mem_lib = {.free_fun   = free_cb,
                          .malloc_fun = malloc_cb,
                          .arg        = NULL};

    treeset_init(&tree, mem_lib);

    for (vsize_t i = 0; i < N; ++i) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    treeset_visit(&tree, free_visitor, NULL);
    treeset_destroy(&tree);

    return 0;
}
```

 

---
# Functions 

| Function | Description |
|---|---|
| [treeset_init](treeset_rb_fine.h.md#function-treeset_init) | Initializes the treeset.  |
| [treeset_destroy](treeset_rb_fine.h.md#function-treeset_destroy) | Destroys all the remaining nodes in the treeset.  |
| [treeset_add](treeset_rb_fine.h.md#function-treeset_add) | Attempts to insert an element with a given key and value into the treeset.  |
| [treeset_remove](treeset_rb_fine.h.md#function-treeset_remove) | Attempts to remove an element with a given key from the treeset.  |
| [treeset_contains](treeset_rb_fine.h.md#function-treeset_contains) | Searches the treeset for an element with a given key.  |
| [treeset_visit](treeset_rb_fine.h.md#function-treeset_visit) | Visits all elements in the treeset.  |

##  Function `treeset_init`

```c
static void treeset_init(treeset_t *tree, vmem_lib_t mem_lib)
``` 
_Initializes the treeset._ 


> **Note:** must be called before threads access the treeset. 

**Parameters:**

- `tree`: address of the treeset_t object. 
- `mem_lib`: object of type `vmem_lib_t` containing malloc/free functions to allocate/free internal nodes. 




##  Function `treeset_destroy`

```c
static void treeset_destroy(treeset_t *tree)
``` 
_Destroys all the remaining nodes in the treeset._ 


> **Note:** call only after thread join, or after all threads finished accessing the treeset. 

**Parameters:**

- `tree`: address of the treeset_t object. 




##  Function `treeset_add`

```c
static vbool_t treeset_add(treeset_t *tree, treeset_key_t key, void *value, void **out_value)
``` 
_Attempts to insert an element with a given key and value into the treeset._ 




**Parameters:**

- `tree`: address of the treeset_t object. 
- `key`: the key to be inserted. 
- `value`: value to be associated with inserted key. 
- `out_value`: out parameter for the previous value associated with the key. 


**Returns:** true operation succeeded. 

**Returns:** false operation failed, since the given key was already in the treeset, in the `out_value` the value of this element is returned. 



##  Function `treeset_remove`

```c
static vbool_t treeset_remove(treeset_t *tree, treeset_key_t key, void **out_value)
``` 
_Attempts to remove an element with a given key from the treeset._ 




**Parameters:**

- `tree`: address of the treeset_t object. 
- `key`: the key to be removed. 
- `out_value`: out parameter for the value associated with the key. 


**Returns:** true operation succeeded, in the `out_value` the value of the removed element is returned. 

**Returns:** false operation failed, there is no element with the given key. 



##  Function `treeset_contains`

```c
static vbool_t treeset_contains(treeset_t *tree, treeset_key_t key, void **out_value)
``` 
_Searches the treeset for an element with a given key._ 




**Parameters:**

- `tree`: address of the treeset_t object. 
- `key`: the key to be searched for. 
- `out_value`: out parameter for the value associated with the key. 


**Returns:** true operation succeeded, in the `out_value` the value of the found element is returned. 

**Returns:** false operation failed, there is no element with the given key. 



##  Function `treeset_visit`

```c
static void treeset_visit(treeset_t *tree, treeset_visitor visitor, void *arg)
``` 
_Visits all elements in the treeset._ 


> **Note:** call only after thread join, or after all threads finished accessing the treeset. 

**Parameters:**

- `tree`: address of the treeset_t object. 
- `visitor`: address of the function to call on each element. 
- `arg`: the third argument to the visitor function. 





---
