#  [vsync](../README.md) / [stack](README.md) / quack.h
_Lockfree concurrent stack/queue (Treiber stack)_ 

**Groups:** [Linearizable](../GROUP_linearizable.md), [Lock-free](../GROUP_lock_free.md)

It's a queue! It's a stack! No, it's a quack!

Quack is a concurrent stack that can be accessed as queue. It is also known as Treiber stack. The producers call [quack_push()](quack.h.md#function-quack_push) to add new elements to the stack. The consumers call [quack_popall()](quack.h.md#function-quack_popall) to return a singled-linked list in LIFO order (as a stack). The caller can change the order to FIFO order by calling [quack_reverse()](quack.h.md#function-quack_reverse). The caller is reposible for iterating over the elements of the list.

## Example quack



```
typedef struct {
     quack_node_t n; // embed quack_node in your data structure
     int value;
} node_t;

quack_t stack;
node_t *mynode;

// to push into the stack
quack_push(&stack, &mynode->n);

// to pop from and iterate in FIFO order
quack_node_t *n = quack_reverse(quack_popall(&stack));
for (; n != NULL; n = n->next) {
    node_t *nn = (node_t*) n;
    // use node
}
```

 

## Reference

Treiber, R.K., 1986. Systems programming: Coping with parallelism. 

---
# Macros 

| Macro | Description |
|---|---|
| [QUACK_INIT](quack.h.md#macro-quack_init) | Initializer of quack.  |

##  Macro `QUACK_INIT`

```c
QUACK_INIT()
```

 
_Initializer of quack._ 



---
# Functions 

| Function | Description |
|---|---|
| [quack_init](quack.h.md#function-quack_init) | Initializes quack.  |
| [quack_push](quack.h.md#function-quack_push) | Pushes node into the quack.  |
| [quack_is_empty](quack.h.md#function-quack_is_empty) | Checks if quack is empty.  |
| [quack_popall](quack.h.md#function-quack_popall) | Pops all elements from the quack in LIFO order.  |
| [quack_reverse](quack.h.md#function-quack_reverse) | Reverse order of elements taken from the quack (from LIFO to FIFO or vice-versa).  |

##  Function `quack_init`

```c
static void quack_init(quack_t *s)
``` 
_Initializes quack._ 




**Parameters:**

- `q`: quack data structure 




##  Function `quack_push`

```c
static void quack_push(quack_t *q, quack_node_t *n)
``` 
_Pushes node into the quack._ 




**Parameters:**

- `q`: quack data structure 
- `n`: node element 




##  Function `quack_is_empty`

```c
static vbool_t quack_is_empty(quack_t *const q)
``` 
_Checks if quack is empty._ 




**Parameters:**

- `q`: quack data structure 


**Returns:** true if there is an element in quack, false otherwise. 



##  Function `quack_popall`

```c
static quack_node_t* quack_popall(quack_t *q)
``` 
_Pops all elements from the quack in LIFO order._ 


The user is responsible for iterating over the elements (accessible with `next` pointer).



**Parameters:**

- `q`: quack data structure 


**Returns:** head of quack 



##  Function `quack_reverse`

```c
static quack_node_t* quack_reverse(quack_node_t *n)
``` 
_Reverse order of elements taken from the quack (from LIFO to FIFO or vice-versa)._ 




**Parameters:**

- `n`: head or tail of quack 


**Returns:** tail or head of quack 




---
