#  [vsync](../README.md) / [atomic](README.md) / core_ptr.h
_Atomic functions for vatomicptr_t variables._ 

This file declares and documents the core atomic functions operating on vatomicptr_t variables.

Please do not include this file directly, instead use: 

```c
#include <vsync/atomic/core.h>
```

 

---
# Functions 

| Function | Description |
|---|---|
| [vatomicptr_init](core_ptr.h.md#function-vatomicptr_init) | Initializes the atomic variable a with value v.  |
| [vatomicptr_read](core_ptr.h.md#function-vatomicptr_read) | Returns the value of the atomic variable pointed by a.  |
| [vatomicptr_read_acq](core_ptr.h.md#function-vatomicptr_read_acq) | Returns the value of the atomic variable pointed by a.  |
| [vatomicptr_read_rlx](core_ptr.h.md#function-vatomicptr_read_rlx) | Returns the value of the atomic variable pointed by a.  |
| [vatomicptr_write](core_ptr.h.md#function-vatomicptr_write) | Writes value v in the atomic variable pointed by a.  |
| [vatomicptr_write_rel](core_ptr.h.md#function-vatomicptr_write_rel) | Writes value v in the atomic variable pointed by a.  |
| [vatomicptr_write_rlx](core_ptr.h.md#function-vatomicptr_write_rlx) | Writes value v in the atomic variable pointed by a.  |
| [vatomicptr_xchg](core_ptr.h.md#function-vatomicptr_xchg) | Writes v in a and returns old value.  |
| [vatomicptr_xchg_acq](core_ptr.h.md#function-vatomicptr_xchg_acq) | Writes v in a and returns old value.  |
| [vatomicptr_xchg_rel](core_ptr.h.md#function-vatomicptr_xchg_rel) | Writes v in a and returns old value.  |
| [vatomicptr_xchg_rlx](core_ptr.h.md#function-vatomicptr_xchg_rlx) | Writes v in a and returns old value.  |
| [vatomicptr_cmpxchg](core_ptr.h.md#function-vatomicptr_cmpxchg) | Writes value v in a if e is the current value.  |
| [vatomicptr_cmpxchg_acq](core_ptr.h.md#function-vatomicptr_cmpxchg_acq) | Writes value v in a if e is the current value.  |
| [vatomicptr_cmpxchg_rel](core_ptr.h.md#function-vatomicptr_cmpxchg_rel) | Writes value v in a if e is the current value.  |
| [vatomicptr_cmpxchg_rlx](core_ptr.h.md#function-vatomicptr_cmpxchg_rlx) | Writes value v in a if e is the current value.  |

##  Function `vatomicptr_init`

```c
static void vatomicptr_init(vatomicptr_t *a, void *v)
``` 
_Initializes the atomic variable a with value v._ 


The initialization is equivalent to an [vatomicptr_write()](core_ptr.h.md#function-vatomicptr_write).



**Parameters:**

- `a`: atomic variable 
- `v`: initial value


**Memory ordering**: seq_cst 


##  Function `vatomicptr_read`

```c
static void* vatomicptr_read(const vatomicptr_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: seq_cst 


##  Function `vatomicptr_read_acq`

```c
static void* vatomicptr_read_acq(const vatomicptr_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: acquire 


##  Function `vatomicptr_read_rlx`

```c
static void* vatomicptr_read_rlx(const vatomicptr_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: relaxed 


##  Function `vatomicptr_write`

```c
static void vatomicptr_write(vatomicptr_t *a, void *v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: seq_cst 


##  Function `vatomicptr_write_rel`

```c
static void vatomicptr_write_rel(vatomicptr_t *a, void *v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: release 


##  Function `vatomicptr_write_rlx`

```c
static void vatomicptr_write_rlx(vatomicptr_t *a, void *v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: relaxed 


##  Function `vatomicptr_xchg`

```c
static void* vatomicptr_xchg(vatomicptr_t *a, void *v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicptr_xchg_acq`

```c
static void* vatomicptr_xchg_acq(vatomicptr_t *a, void *v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicptr_xchg_rel`

```c
static void* vatomicptr_xchg_rel(vatomicptr_t *a, void *v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicptr_xchg_rlx`

```c
static void* vatomicptr_xchg_rlx(vatomicptr_t *a, void *v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomicptr_cmpxchg`

```c
static void* vatomicptr_cmpxchg(vatomicptr_t *a, void *e, void *v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomicptr_cmpxchg_acq`

```c
static void* vatomicptr_cmpxchg_acq(vatomicptr_t *a, void *e, void *v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomicptr_cmpxchg_rel`

```c
static void* vatomicptr_cmpxchg_rel(vatomicptr_t *a, void *e, void *v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomicptr_cmpxchg_rlx`

```c
static void* vatomicptr_cmpxchg_rlx(vatomicptr_t *a, void *e, void *v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 




---
