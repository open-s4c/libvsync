#  [vsync](../README.md) / [atomic](README.md) / await_ptr.h
_Atomic await functions for vatomicptr_t variables._ 

This file declares and documents the atomic await functions operating on vatomicptr_t variables.

Please do not include this file directly, instead use: 

```c
#include <vsync/atomic/await.h>
```

 

---
# Functions 

| Function | Description |
|---|---|
| [vatomicptr_await_neq](await_ptr.h.md#function-vatomicptr_await_neq) | Politely awaits the value pointed by a to be different than v.  |
| [vatomicptr_await_neq_acq](await_ptr.h.md#function-vatomicptr_await_neq_acq) | Politely awaits the value pointed by a to be different than v.  |
| [vatomicptr_await_neq_rlx](await_ptr.h.md#function-vatomicptr_await_neq_rlx) | Politely awaits the value pointed by a to be different than v.  |
| [vatomicptr_await_eq](await_ptr.h.md#function-vatomicptr_await_eq) | Politely awaits the value pointed by a to be equal to v.  |
| [vatomicptr_await_eq_acq](await_ptr.h.md#function-vatomicptr_await_eq_acq) | Politely awaits the value pointed by a to be equal to v.  |
| [vatomicptr_await_eq_rlx](await_ptr.h.md#function-vatomicptr_await_eq_rlx) | Politely awaits the value pointed by a to be equal to v.  |
| [vatomicptr_await_eq_set](await_ptr.h.md#function-vatomicptr_await_eq_set) | Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied.  |
| [vatomicptr_await_eq_set_acq](await_ptr.h.md#function-vatomicptr_await_eq_set_acq) | Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied.  |
| [vatomicptr_await_eq_set_rel](await_ptr.h.md#function-vatomicptr_await_eq_set_rel) | Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied.  |
| [vatomicptr_await_eq_set_rlx](await_ptr.h.md#function-vatomicptr_await_eq_set_rlx) | Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied.  |
| [vatomicptr_await_neq_set](await_ptr.h.md#function-vatomicptr_await_neq_set) | Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied.  |
| [vatomicptr_await_neq_set_acq](await_ptr.h.md#function-vatomicptr_await_neq_set_acq) | Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied.  |
| [vatomicptr_await_neq_set_rel](await_ptr.h.md#function-vatomicptr_await_neq_set_rel) | Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied.  |
| [vatomicptr_await_neq_set_rlx](await_ptr.h.md#function-vatomicptr_await_neq_set_rlx) | Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied.  |

##  Function `vatomicptr_await_neq`

```c
static void* vatomicptr_await_neq(const vatomicptr_t *a, void *v)
``` 
_Politely awaits the value pointed by a to be different than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomicptr_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: seq_cst 


##  Function `vatomicptr_await_neq_acq`

```c
static void* vatomicptr_await_neq_acq(const vatomicptr_t *a, void *v)
``` 
_Politely awaits the value pointed by a to be different than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomicptr_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: acquire 


##  Function `vatomicptr_await_neq_rlx`

```c
static void* vatomicptr_await_neq_rlx(const vatomicptr_t *a, void *v)
``` 
_Politely awaits the value pointed by a to be different than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomicptr_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: relaxed 


##  Function `vatomicptr_await_eq`

```c
static void* vatomicptr_await_eq(const vatomicptr_t *a, void *v)
``` 
_Politely awaits the value pointed by a to be equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomicptr_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** last value before satisfying condition

**Memory ordering**: seq_cst 


##  Function `vatomicptr_await_eq_acq`

```c
static void* vatomicptr_await_eq_acq(const vatomicptr_t *a, void *v)
``` 
_Politely awaits the value pointed by a to be equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomicptr_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** last value before satisfying condition

**Memory ordering**: acquire 


##  Function `vatomicptr_await_eq_rlx`

```c
static void* vatomicptr_await_eq_rlx(const vatomicptr_t *a, void *v)
``` 
_Politely awaits the value pointed by a to be equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomicptr_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** last value before satisfying condition

**Memory ordering**: relaxed 


##  Function `vatomicptr_await_eq_set`

```c
static void* vatomicptr_await_eq_set(vatomicptr_t *a, void *c, void *v)
``` 
_Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicptr_await_eq_set_acq`

```c
static void* vatomicptr_await_eq_set_acq(vatomicptr_t *a, void *c, void *v)
``` 
_Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicptr_await_eq_set_rel`

```c
static void* vatomicptr_await_eq_set_rel(vatomicptr_t *a, void *c, void *v)
``` 
_Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicptr_await_eq_set_rlx`

```c
static void* vatomicptr_await_eq_set_rlx(vatomicptr_t *a, void *c, void *v)
``` 
_Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomicptr_await_neq_set`

```c
static void* vatomicptr_await_neq_set(vatomicptr_t *a, void *c, void *v)
``` 
_Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicptr_await_neq_set_acq`

```c
static void* vatomicptr_await_neq_set_acq(vatomicptr_t *a, void *c, void *v)
``` 
_Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicptr_await_neq_set_rel`

```c
static void* vatomicptr_await_neq_set_rel(vatomicptr_t *a, void *c, void *v)
``` 
_Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicptr_await_neq_set_rlx`

```c
static void* vatomicptr_await_neq_set_rlx(vatomicptr_t *a, void *c, void *v)
``` 
_Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 



---
