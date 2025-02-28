#  [vsync](../README.md) / [map](README.md) / rbtree.h
_This implementation of red-black tree uses a latch sequence counter to get lockless readers and non-starving writers._ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [SMR-required](../GROUP_requires_smr.md)

A red-black tree is a balanced binary search tree with nodes that are ordered by unique keys.

The writers are protected by a lock and update two copies of the rbtree. Thus at least one copy is always available for the readers. Should a reader not finish before a writer starts updating another copy, this will be spotted by the sequence counter, and the reader will restart.


### Example:



```c
#include <vsync/map/rbtree.h>

typedef struct my_node_s {
    vrbtree_key_t key;
    vrbtree_node_t embed;
} my_node_t;

vint32_t
_cmp_fun(vrbtree_node_t *node, vrbtree_key_t key)
{
    my_node_t *nn = V_CONTAINER_OF(node, my_node_t, embed);
    if (nn->key < key) {
        return -1;
    } else if (nn->key > key) {
        return 1;
    } else {
        return 0;
    }
}

void
_retire_fun(vrbtree_node_t *node, void *args)
{
    V_UNUSED(node, args);
}

int
main(void)
{
    vrbtree_t tree;
    vrbtree_key_t key = 3;
    vbool_t res;
    vrbtree_node_t *res_node = NULL;

    vrbtree_init(&tree, _retire_fun, NULL, _cmp_fun);

    my_node_t node;
    node.key = key;

    res = vrbtree_add(&tree, key, &node.embed);
    ASSERT(res);

    res_node = vrbtree_get(&tree, key);
    ASSERT(res_node == &node.embed);

    res = vrbtree_remove(&tree, key);
    ASSERT(res);

    res_node = vrbtree_get(&tree, key);
    ASSERT(!res_node);

    vrbtree_destroy(&tree);

    return 0;
}
```

 

---
# Functions 

| Function | Description |
|---|---|
| [vrbtree_init](rbtree.h.md#function-vrbtree_init) | Initializes the given vrbtree_t object `tree`.  |
| [vrbtree_destroy](rbtree.h.md#function-vrbtree_destroy) | Destroys all the remaining nodes in the rbtree.  |
| [vrbtree_add](rbtree.h.md#function-vrbtree_add) | Inserts the given node into the rbtree.  |
| [vrbtree_remove](rbtree.h.md#function-vrbtree_remove) | Removes the node associated with the given key from the rbtree.  |
| [vrbtree_get](rbtree.h.md#function-vrbtree_get) | Looks for the rbtree node associated with the given key.  |
| [vrbtree_size](rbtree.h.md#function-vrbtree_size) | Returns the number of nodes in the tree.  |
| [vrbtree_first](rbtree.h.md#function-vrbtree_first) | Returns the node of the smallest key in the tree.  |
| [vrbtree_last](rbtree.h.md#function-vrbtree_last) | Returns the node of the greatest key in the tree.  |
| [vrbtree_successor](rbtree.h.md#function-vrbtree_successor) | Looks for the rbtree node associated with the smallest key greater than the given key.  |
| [vrbtree_predecessor](rbtree.h.md#function-vrbtree_predecessor) | Looks for the rbtree node associated with the largest key smaller than the given key.  |

##  Function `vrbtree_init`

```c
static void vrbtree_init(vrbtree_t *tree, vrbtree_handle_node_t retire_fun, void *retire_fun_arg, vrbtree_cmp_key_t cmp_fun)
``` 
_Initializes the given vrbtree_t object_ `tree`_._ 


> **Note:** must be called before threads access the rbtree.



**Parameters:**

- `tree`: address of vrbtree_t object. 
- `retire_fun`: address of the function used to retire removed nodes. 
- `retire_fun_arg`: optional second parameter to pass to `retire_fun`. 
- `cmp_fun`: address of the function used for comparing a key in `vrbtree_node_t` object with a given key. 




##  Function `vrbtree_destroy`

```c
static void vrbtree_destroy(vrbtree_t *tree)
``` 
_Destroys all the remaining nodes in the rbtree._ 


> **Note:** call only after thread join, or after all threads finished accessing the given vrbtree_t object.



**Parameters:**

- `tree`: address of vrbtree_t object. 




##  Function `vrbtree_add`

```c
static vbool_t vrbtree_add(vrbtree_t *tree, vrbtree_key_t key, vrbtree_node_t *node)
``` 
_Inserts the given node into the rbtree._ 


The node is only inserted if there is no other node associated with the given key exists in the rbtree.

> **Note:** if the operation fails, users can safely free or reuse the given node.


**Precondition:** the key must be set in the given `node` before calling add. 


**Parameters:**

- `tree`: address of vrbtree_t object. 
- `key`: the key value that is used to identify the node. 
- `node`: address of vrbtree_node_t object. 


**Returns:** true operation succeeded, the given node has been added. 

**Returns:** false operation failed, the given node has not been added, because there exists another node associated with the given key.

> **Note:** must be called inside SMR critical section. 


##  Function `vrbtree_remove`

```c
static vbool_t vrbtree_remove(vrbtree_t *tree, vrbtree_key_t key)
``` 
_Removes the node associated with the given key from the rbtree._ 


> **Note:** the registered retire callback will be called on the removed node.



**Parameters:**

- `tree`: address of vrbtree_t object. 
- `key`: the key identifying the node to remove. 


**Returns:** true the node associated with the given key was removed successfully. 

**Returns:** false could not find a node holding the given key to remove.

> **Note:** must be called inside SMR critical section. 


##  Function `vrbtree_get`

```c
static vrbtree_node_t* vrbtree_get(vrbtree_t *tree, vrbtree_key_t key)
``` 
_Looks for the rbtree node associated with the given key._ 




**Parameters:**

- `tree`: address of vrbtree_t object. 
- `key`: key value. 


**Returns:** vrbtree_node_t* address of the rbtree node associated with the given key. 

**Returns:** NULL if no node associated with the given key was found.

> **Note:** must be called inside SMR critical section. 


##  Function `vrbtree_size`

```c
static vsize_t vrbtree_size(vrbtree_t *tree)
``` 
_Returns the number of nodes in the tree._ 




**Parameters:**

- `tree`: address of vrbtree_t object.


**Returns:** number of nodes in the tree. 



##  Function `vrbtree_first`

```c
static vrbtree_node_t* vrbtree_first(vrbtree_t *tree)
``` 
_Returns the node of the smallest key in the tree._ 




**Parameters:**

- `tree`: address of vrbtree_t object.


**Returns:** address of the rbtree node associated with the smallest key in the tree. 

**Returns:** NULL if the tree is empty.

> **Note:** must be called inside SMR critical section. 


##  Function `vrbtree_last`

```c
static vrbtree_node_t* vrbtree_last(vrbtree_t *tree)
``` 
_Returns the node of the greatest key in the tree._ 




**Parameters:**

- `tree`: address of vrbtree_t object.


**Returns:** address of the rbtree node associated with the greatest key in the tree. 

**Returns:** NULL if the tree is empty.

> **Note:** must be called inside SMR critical section. 


##  Function `vrbtree_successor`

```c
static vrbtree_node_t* vrbtree_successor(vrbtree_t *tree, vrbtree_key_t key)
``` 
_Looks for the rbtree node associated with the smallest key greater than the given key._ 




**Parameters:**

- `tree`: address of vrbtree_t object. 
- `key`: key value.


**Returns:** address of the rbtree node associated with the successor key. 

**Returns:** NULL if there is no successor.

> **Note:** must be called inside SMR critical section. 


##  Function `vrbtree_predecessor`

```c
static vrbtree_node_t* vrbtree_predecessor(vrbtree_t *tree, vrbtree_key_t key)
``` 
_Looks for the rbtree node associated with the largest key smaller than the given key._ 




**Parameters:**

- `tree`: address of vrbtree_t object. 
- `key`: key value.


**Returns:** address of the rbtree node associated with the predecessor key. 

**Returns:** NULL if there is no predecessor.

> **Note:** must be called inside SMR critical section. 



---
