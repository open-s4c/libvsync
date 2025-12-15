#  [vsync](../README.md) / [atomic](README.md) / core_sz.h
_Atomic functions for vatomicsz_t variables._ 

This file declares and documents the core atomic functions operating on vatomicsz_t variables.

Please do not include this file directly, instead use: 

```c
#include <vsync/atomic/core.h>
```

 

---
# Functions 

| Function | Description |
|---|---|
| [vatomicsz_init](core_sz.h.md#function-vatomicsz_init) | Initializes the atomic variable a with value v.  |
| [vatomicsz_read](core_sz.h.md#function-vatomicsz_read) | Returns the value of the atomic variable pointed by a.  |
| [vatomicsz_read_acq](core_sz.h.md#function-vatomicsz_read_acq) | Returns the value of the atomic variable pointed by a.  |
| [vatomicsz_read_rlx](core_sz.h.md#function-vatomicsz_read_rlx) | Returns the value of the atomic variable pointed by a.  |
| [vatomicsz_write](core_sz.h.md#function-vatomicsz_write) | Writes value v in the atomic variable pointed by a.  |
| [vatomicsz_write_rel](core_sz.h.md#function-vatomicsz_write_rel) | Writes value v in the atomic variable pointed by a.  |
| [vatomicsz_write_rlx](core_sz.h.md#function-vatomicsz_write_rlx) | Writes value v in the atomic variable pointed by a.  |
| [vatomicsz_xchg](core_sz.h.md#function-vatomicsz_xchg) | Writes v in a and returns old value.  |
| [vatomicsz_xchg_acq](core_sz.h.md#function-vatomicsz_xchg_acq) | Writes v in a and returns old value.  |
| [vatomicsz_xchg_rel](core_sz.h.md#function-vatomicsz_xchg_rel) | Writes v in a and returns old value.  |
| [vatomicsz_xchg_rlx](core_sz.h.md#function-vatomicsz_xchg_rlx) | Writes v in a and returns old value.  |
| [vatomicsz_cmpxchg](core_sz.h.md#function-vatomicsz_cmpxchg) | Writes value v in a if e is the current value.  |
| [vatomicsz_cmpxchg_acq](core_sz.h.md#function-vatomicsz_cmpxchg_acq) | Writes value v in a if e is the current value.  |
| [vatomicsz_cmpxchg_rel](core_sz.h.md#function-vatomicsz_cmpxchg_rel) | Writes value v in a if e is the current value.  |
| [vatomicsz_cmpxchg_rlx](core_sz.h.md#function-vatomicsz_cmpxchg_rlx) | Writes value v in a if e is the current value.  |
| [vatomicsz_get_max](core_sz.h.md#function-vatomicsz_get_max) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomicsz_get_max_acq](core_sz.h.md#function-vatomicsz_get_max_acq) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomicsz_get_max_rel](core_sz.h.md#function-vatomicsz_get_max_rel) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomicsz_get_max_rlx](core_sz.h.md#function-vatomicsz_get_max_rlx) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomicsz_max_get](core_sz.h.md#function-vatomicsz_max_get) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomicsz_max_get_acq](core_sz.h.md#function-vatomicsz_max_get_acq) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomicsz_max_get_rel](core_sz.h.md#function-vatomicsz_max_get_rel) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomicsz_max_get_rlx](core_sz.h.md#function-vatomicsz_max_get_rlx) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomicsz_max](core_sz.h.md#function-vatomicsz_max) | Writes v to a if v is greater than *a.  |
| [vatomicsz_max_rel](core_sz.h.md#function-vatomicsz_max_rel) | Writes v to a if v is greater than *a.  |
| [vatomicsz_max_rlx](core_sz.h.md#function-vatomicsz_max_rlx) | Writes v to a if v is greater than *a.  |
| [vatomicsz_get_and](core_sz.h.md#function-vatomicsz_get_and) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomicsz_get_and_acq](core_sz.h.md#function-vatomicsz_get_and_acq) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomicsz_get_and_rel](core_sz.h.md#function-vatomicsz_get_and_rel) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomicsz_get_and_rlx](core_sz.h.md#function-vatomicsz_get_and_rlx) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomicsz_and_get](core_sz.h.md#function-vatomicsz_and_get) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomicsz_and_get_acq](core_sz.h.md#function-vatomicsz_and_get_acq) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomicsz_and_get_rel](core_sz.h.md#function-vatomicsz_and_get_rel) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomicsz_and_get_rlx](core_sz.h.md#function-vatomicsz_and_get_rlx) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomicsz_and](core_sz.h.md#function-vatomicsz_and) | Applies bitwise and operation with v to the value of a.  |
| [vatomicsz_and_rel](core_sz.h.md#function-vatomicsz_and_rel) | Applies bitwise and operation with v to the value of a.  |
| [vatomicsz_and_rlx](core_sz.h.md#function-vatomicsz_and_rlx) | Applies bitwise and operation with v to the value of a.  |
| [vatomicsz_get_or](core_sz.h.md#function-vatomicsz_get_or) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomicsz_get_or_acq](core_sz.h.md#function-vatomicsz_get_or_acq) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomicsz_get_or_rel](core_sz.h.md#function-vatomicsz_get_or_rel) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomicsz_get_or_rlx](core_sz.h.md#function-vatomicsz_get_or_rlx) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomicsz_or_get](core_sz.h.md#function-vatomicsz_or_get) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomicsz_or_get_acq](core_sz.h.md#function-vatomicsz_or_get_acq) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomicsz_or_get_rel](core_sz.h.md#function-vatomicsz_or_get_rel) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomicsz_or_get_rlx](core_sz.h.md#function-vatomicsz_or_get_rlx) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomicsz_or](core_sz.h.md#function-vatomicsz_or) | Applies bitwise or operation with v to the value of a.  |
| [vatomicsz_or_rel](core_sz.h.md#function-vatomicsz_or_rel) | Applies bitwise or operation with v to the value of a.  |
| [vatomicsz_or_rlx](core_sz.h.md#function-vatomicsz_or_rlx) | Applies bitwise or operation with v to the value of a.  |
| [vatomicsz_get_xor](core_sz.h.md#function-vatomicsz_get_xor) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomicsz_get_xor_acq](core_sz.h.md#function-vatomicsz_get_xor_acq) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomicsz_get_xor_rel](core_sz.h.md#function-vatomicsz_get_xor_rel) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomicsz_get_xor_rlx](core_sz.h.md#function-vatomicsz_get_xor_rlx) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomicsz_xor_get](core_sz.h.md#function-vatomicsz_xor_get) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomicsz_xor_get_acq](core_sz.h.md#function-vatomicsz_xor_get_acq) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomicsz_xor_get_rel](core_sz.h.md#function-vatomicsz_xor_get_rel) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomicsz_xor_get_rlx](core_sz.h.md#function-vatomicsz_xor_get_rlx) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomicsz_xor](core_sz.h.md#function-vatomicsz_xor) | Applies bitwise xor opeation with v to the value of a.  |
| [vatomicsz_xor_rel](core_sz.h.md#function-vatomicsz_xor_rel) | Applies bitwise xor opeation with v to the value of a.  |
| [vatomicsz_xor_rlx](core_sz.h.md#function-vatomicsz_xor_rlx) | Applies bitwise xor opeation with v to the value of a.  |
| [vatomicsz_get_add](core_sz.h.md#function-vatomicsz_get_add) | Adds v to the value of a and returns the old value.  |
| [vatomicsz_get_add_acq](core_sz.h.md#function-vatomicsz_get_add_acq) | Adds v to the value of a and returns the old value.  |
| [vatomicsz_get_add_rel](core_sz.h.md#function-vatomicsz_get_add_rel) | Adds v to the value of a and returns the old value.  |
| [vatomicsz_get_add_rlx](core_sz.h.md#function-vatomicsz_get_add_rlx) | Adds v to the value of a and returns the old value.  |
| [vatomicsz_add_get](core_sz.h.md#function-vatomicsz_add_get) | Adds v to the value of a and returns the new value.  |
| [vatomicsz_add_get_acq](core_sz.h.md#function-vatomicsz_add_get_acq) | Adds v to the value of a and returns the new value.  |
| [vatomicsz_add_get_rel](core_sz.h.md#function-vatomicsz_add_get_rel) | Adds v to the value of a and returns the new value.  |
| [vatomicsz_add_get_rlx](core_sz.h.md#function-vatomicsz_add_get_rlx) | Adds v to the value of a and returns the new value.  |
| [vatomicsz_add](core_sz.h.md#function-vatomicsz_add) | Adds v to the value of a.  |
| [vatomicsz_add_rel](core_sz.h.md#function-vatomicsz_add_rel) | Adds v to the value of a.  |
| [vatomicsz_add_rlx](core_sz.h.md#function-vatomicsz_add_rlx) | Adds v to the value of a.  |
| [vatomicsz_get_inc](core_sz.h.md#function-vatomicsz_get_inc) | Increments the value of a and returns the old value.  |
| [vatomicsz_get_inc_acq](core_sz.h.md#function-vatomicsz_get_inc_acq) | Increments the value of a and returns the old value.  |
| [vatomicsz_get_inc_rel](core_sz.h.md#function-vatomicsz_get_inc_rel) | Increments the value of a and returns the old value.  |
| [vatomicsz_get_inc_rlx](core_sz.h.md#function-vatomicsz_get_inc_rlx) | Increments the value of a and returns the old value.  |
| [vatomicsz_inc_get](core_sz.h.md#function-vatomicsz_inc_get) | Increments the value of a and returns the new value.  |
| [vatomicsz_inc_get_acq](core_sz.h.md#function-vatomicsz_inc_get_acq) | Increments the value of a and returns the new value.  |
| [vatomicsz_inc_get_rel](core_sz.h.md#function-vatomicsz_inc_get_rel) | Increments the value of a and returns the new value.  |
| [vatomicsz_inc_get_rlx](core_sz.h.md#function-vatomicsz_inc_get_rlx) | Increments the value of a and returns the new value.  |
| [vatomicsz_inc](core_sz.h.md#function-vatomicsz_inc) | Increments the value of a.  |
| [vatomicsz_inc_rel](core_sz.h.md#function-vatomicsz_inc_rel) | Increments the value of a.  |
| [vatomicsz_inc_rlx](core_sz.h.md#function-vatomicsz_inc_rlx) | Increments the value of a.  |
| [vatomicsz_get_sub](core_sz.h.md#function-vatomicsz_get_sub) | Subtracts v from a and returns the old value.  |
| [vatomicsz_get_sub_acq](core_sz.h.md#function-vatomicsz_get_sub_acq) | Subtracts v from a and returns the old value.  |
| [vatomicsz_get_sub_rel](core_sz.h.md#function-vatomicsz_get_sub_rel) | Subtracts v from a and returns the old value.  |
| [vatomicsz_get_sub_rlx](core_sz.h.md#function-vatomicsz_get_sub_rlx) | Subtracts v from a and returns the old value.  |
| [vatomicsz_sub_get](core_sz.h.md#function-vatomicsz_sub_get) | Subtracts v from a and returns the new value.  |
| [vatomicsz_sub_get_acq](core_sz.h.md#function-vatomicsz_sub_get_acq) | Subtracts v from a and returns the new value.  |
| [vatomicsz_sub_get_rel](core_sz.h.md#function-vatomicsz_sub_get_rel) | Subtracts v from a and returns the new value.  |
| [vatomicsz_sub_get_rlx](core_sz.h.md#function-vatomicsz_sub_get_rlx) | Subtracts v from a and returns the new value.  |
| [vatomicsz_sub](core_sz.h.md#function-vatomicsz_sub) | Subtracts v from a.  |
| [vatomicsz_sub_rel](core_sz.h.md#function-vatomicsz_sub_rel) | Subtracts v from a.  |
| [vatomicsz_sub_rlx](core_sz.h.md#function-vatomicsz_sub_rlx) | Subtracts v from a.  |
| [vatomicsz_get_dec](core_sz.h.md#function-vatomicsz_get_dec) | Decrements the value of a and returns the old value.  |
| [vatomicsz_get_dec_acq](core_sz.h.md#function-vatomicsz_get_dec_acq) | Decrements the value of a and returns the old value.  |
| [vatomicsz_get_dec_rel](core_sz.h.md#function-vatomicsz_get_dec_rel) | Decrements the value of a and returns the old value.  |
| [vatomicsz_get_dec_rlx](core_sz.h.md#function-vatomicsz_get_dec_rlx) | Decrements the value of a and returns the old value.  |
| [vatomicsz_dec_get](core_sz.h.md#function-vatomicsz_dec_get) | Decrements the value of a and returns the new value.  |
| [vatomicsz_dec_get_acq](core_sz.h.md#function-vatomicsz_dec_get_acq) | Decrements the value of a and returns the new value.  |
| [vatomicsz_dec_get_rel](core_sz.h.md#function-vatomicsz_dec_get_rel) | Decrements the value of a and returns the new value.  |
| [vatomicsz_dec_get_rlx](core_sz.h.md#function-vatomicsz_dec_get_rlx) | Decrements the value of a and returns the new value.  |
| [vatomicsz_dec](core_sz.h.md#function-vatomicsz_dec) | Decrements the value of a.  |
| [vatomicsz_dec_rel](core_sz.h.md#function-vatomicsz_dec_rel) | Decrements the value of a.  |
| [vatomicsz_dec_rlx](core_sz.h.md#function-vatomicsz_dec_rlx) | Decrements the value of a.  |

##  Function `vatomicsz_init`

```c
static void vatomicsz_init(vatomicsz_t *a, vsize_t v)
``` 
_Initializes the atomic variable a with value v._ 


The initialization is equivalent to an [vatomicsz_write()](core_sz.h.md#function-vatomicsz_write).



**Parameters:**

- `a`: atomic variable 
- `v`: initial value


**Memory ordering**: seq_cst 


##  Function `vatomicsz_read`

```c
static vsize_t vatomicsz_read(const vatomicsz_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_read_acq`

```c
static vsize_t vatomicsz_read_acq(const vatomicsz_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: acquire 


##  Function `vatomicsz_read_rlx`

```c
static vsize_t vatomicsz_read_rlx(const vatomicsz_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: relaxed 


##  Function `vatomicsz_write`

```c
static void vatomicsz_write(vatomicsz_t *a, vsize_t v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: seq_cst 


##  Function `vatomicsz_write_rel`

```c
static void vatomicsz_write_rel(vatomicsz_t *a, vsize_t v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: release 


##  Function `vatomicsz_write_rlx`

```c
static void vatomicsz_write_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: relaxed 


##  Function `vatomicsz_xchg`

```c
static vsize_t vatomicsz_xchg(vatomicsz_t *a, vsize_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_xchg_acq`

```c
static vsize_t vatomicsz_xchg_acq(vatomicsz_t *a, vsize_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicsz_xchg_rel`

```c
static vsize_t vatomicsz_xchg_rel(vatomicsz_t *a, vsize_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicsz_xchg_rlx`

```c
static vsize_t vatomicsz_xchg_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomicsz_cmpxchg`

```c
static vsize_t vatomicsz_cmpxchg(vatomicsz_t *a, vsize_t e, vsize_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomicsz_cmpxchg_acq`

```c
static vsize_t vatomicsz_cmpxchg_acq(vatomicsz_t *a, vsize_t e, vsize_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomicsz_cmpxchg_rel`

```c
static vsize_t vatomicsz_cmpxchg_rel(vatomicsz_t *a, vsize_t e, vsize_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomicsz_cmpxchg_rlx`

```c
static vsize_t vatomicsz_cmpxchg_rlx(vatomicsz_t *a, vsize_t e, vsize_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomicsz_get_max`

```c
static vsize_t vatomicsz_get_max(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_get_max_acq`

```c
static vsize_t vatomicsz_get_max_acq(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: acquire 


##  Function `vatomicsz_get_max_rel`

```c
static vsize_t vatomicsz_get_max_rel(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: release 


##  Function `vatomicsz_get_max_rlx`

```c
static vsize_t vatomicsz_get_max_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: relaxed 


##  Function `vatomicsz_max_get`

```c
static vsize_t vatomicsz_max_get(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_max_get_acq`

```c
static vsize_t vatomicsz_max_get_acq(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: acquire 


##  Function `vatomicsz_max_get_rel`

```c
static vsize_t vatomicsz_max_get_rel(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: release 


##  Function `vatomicsz_max_get_rlx`

```c
static vsize_t vatomicsz_max_get_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: relaxed 


##  Function `vatomicsz_max`

```c
static void vatomicsz_max(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value


**Memory ordering**: seq_cst 


##  Function `vatomicsz_max_rel`

```c
static void vatomicsz_max_rel(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value


**Memory ordering**: release 


##  Function `vatomicsz_max_rlx`

```c
static void vatomicsz_max_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Writes v to a if v is greater than *a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value


**Memory ordering**: relaxed 


##  Function `vatomicsz_get_and`

```c
static vsize_t vatomicsz_get_and(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_get_and_acq`

```c
static vsize_t vatomicsz_get_and_acq(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicsz_get_and_rel`

```c
static vsize_t vatomicsz_get_and_rel(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicsz_get_and_rlx`

```c
static vsize_t vatomicsz_get_and_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomicsz_and_get`

```c
static vsize_t vatomicsz_and_get(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_and_get_acq`

```c
static vsize_t vatomicsz_and_get_acq(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomicsz_and_get_rel`

```c
static vsize_t vatomicsz_and_get_rel(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomicsz_and_get_rlx`

```c
static vsize_t vatomicsz_and_get_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomicsz_and`

```c
static void vatomicsz_and(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: seq_cst 


##  Function `vatomicsz_and_rel`

```c
static void vatomicsz_and_rel(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: release 


##  Function `vatomicsz_and_rlx`

```c
static void vatomicsz_and_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise and operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: relaxed 


##  Function `vatomicsz_get_or`

```c
static vsize_t vatomicsz_get_or(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_get_or_acq`

```c
static vsize_t vatomicsz_get_or_acq(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicsz_get_or_rel`

```c
static vsize_t vatomicsz_get_or_rel(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicsz_get_or_rlx`

```c
static vsize_t vatomicsz_get_or_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomicsz_or_get`

```c
static vsize_t vatomicsz_or_get(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_or_get_acq`

```c
static vsize_t vatomicsz_or_get_acq(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomicsz_or_get_rel`

```c
static vsize_t vatomicsz_or_get_rel(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomicsz_or_get_rlx`

```c
static vsize_t vatomicsz_or_get_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomicsz_or`

```c
static void vatomicsz_or(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: seq_cst 


##  Function `vatomicsz_or_rel`

```c
static void vatomicsz_or_rel(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: release 


##  Function `vatomicsz_or_rlx`

```c
static void vatomicsz_or_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise or operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: relaxed 


##  Function `vatomicsz_get_xor`

```c
static vsize_t vatomicsz_get_xor(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_get_xor_acq`

```c
static vsize_t vatomicsz_get_xor_acq(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicsz_get_xor_rel`

```c
static vsize_t vatomicsz_get_xor_rel(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicsz_get_xor_rlx`

```c
static vsize_t vatomicsz_get_xor_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomicsz_xor_get`

```c
static vsize_t vatomicsz_xor_get(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_xor_get_acq`

```c
static vsize_t vatomicsz_xor_get_acq(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomicsz_xor_get_rel`

```c
static vsize_t vatomicsz_xor_get_rel(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomicsz_xor_get_rlx`

```c
static vsize_t vatomicsz_xor_get_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomicsz_xor`

```c
static void vatomicsz_xor(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor opeation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: seq_cst 


##  Function `vatomicsz_xor_rel`

```c
static void vatomicsz_xor_rel(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor opeation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: release 


##  Function `vatomicsz_xor_rlx`

```c
static void vatomicsz_xor_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Applies bitwise xor opeation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: relaxed 


##  Function `vatomicsz_get_add`

```c
static vsize_t vatomicsz_get_add(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_get_add_acq`

```c
static vsize_t vatomicsz_get_add_acq(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicsz_get_add_rel`

```c
static vsize_t vatomicsz_get_add_rel(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicsz_get_add_rlx`

```c
static vsize_t vatomicsz_get_add_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomicsz_add_get`

```c
static vsize_t vatomicsz_add_get(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_add_get_acq`

```c
static vsize_t vatomicsz_add_get_acq(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomicsz_add_get_rel`

```c
static vsize_t vatomicsz_add_get_rel(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomicsz_add_get_rlx`

```c
static vsize_t vatomicsz_add_get_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomicsz_add`

```c
static void vatomicsz_add(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value


**Memory ordering**: seq_cst 


##  Function `vatomicsz_add_rel`

```c
static void vatomicsz_add_rel(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value


**Memory ordering**: release 


##  Function `vatomicsz_add_rlx`

```c
static void vatomicsz_add_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Adds v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value


**Memory ordering**: relaxed 


##  Function `vatomicsz_get_inc`

```c
static vsize_t vatomicsz_get_inc(vatomicsz_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_get_inc_acq`

```c
static vsize_t vatomicsz_get_inc_acq(vatomicsz_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicsz_get_inc_rel`

```c
static vsize_t vatomicsz_get_inc_rel(vatomicsz_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicsz_get_inc_rlx`

```c
static vsize_t vatomicsz_get_inc_rlx(vatomicsz_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomicsz_inc_get`

```c
static vsize_t vatomicsz_inc_get(vatomicsz_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_inc_get_acq`

```c
static vsize_t vatomicsz_inc_get_acq(vatomicsz_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomicsz_inc_get_rel`

```c
static vsize_t vatomicsz_inc_get_rel(vatomicsz_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomicsz_inc_get_rlx`

```c
static vsize_t vatomicsz_inc_get_rlx(vatomicsz_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomicsz_inc`

```c
static void vatomicsz_inc(vatomicsz_t *a)
``` 
_Increments the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: seq_cst 


##  Function `vatomicsz_inc_rel`

```c
static void vatomicsz_inc_rel(vatomicsz_t *a)
``` 
_Increments the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: release 


##  Function `vatomicsz_inc_rlx`

```c
static void vatomicsz_inc_rlx(vatomicsz_t *a)
``` 
_Increments the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: relaxed 


##  Function `vatomicsz_get_sub`

```c
static vsize_t vatomicsz_get_sub(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_get_sub_acq`

```c
static vsize_t vatomicsz_get_sub_acq(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicsz_get_sub_rel`

```c
static vsize_t vatomicsz_get_sub_rel(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicsz_get_sub_rlx`

```c
static vsize_t vatomicsz_get_sub_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomicsz_sub_get`

```c
static vsize_t vatomicsz_sub_get(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_sub_get_acq`

```c
static vsize_t vatomicsz_sub_get_acq(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomicsz_sub_get_rel`

```c
static vsize_t vatomicsz_sub_get_rel(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomicsz_sub_get_rlx`

```c
static vsize_t vatomicsz_sub_get_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomicsz_sub`

```c
static void vatomicsz_sub(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value


**Memory ordering**: seq_cst 


##  Function `vatomicsz_sub_rel`

```c
static void vatomicsz_sub_rel(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value


**Memory ordering**: release 


##  Function `vatomicsz_sub_rlx`

```c
static void vatomicsz_sub_rlx(vatomicsz_t *a, vsize_t v)
``` 
_Subtracts v from a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value


**Memory ordering**: relaxed 


##  Function `vatomicsz_get_dec`

```c
static vsize_t vatomicsz_get_dec(vatomicsz_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_get_dec_acq`

```c
static vsize_t vatomicsz_get_dec_acq(vatomicsz_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomicsz_get_dec_rel`

```c
static vsize_t vatomicsz_get_dec_rel(vatomicsz_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomicsz_get_dec_rlx`

```c
static vsize_t vatomicsz_get_dec_rlx(vatomicsz_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomicsz_dec_get`

```c
static vsize_t vatomicsz_dec_get(vatomicsz_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomicsz_dec_get_acq`

```c
static vsize_t vatomicsz_dec_get_acq(vatomicsz_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomicsz_dec_get_rel`

```c
static vsize_t vatomicsz_dec_get_rel(vatomicsz_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomicsz_dec_get_rlx`

```c
static vsize_t vatomicsz_dec_get_rlx(vatomicsz_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomicsz_dec`

```c
static void vatomicsz_dec(vatomicsz_t *a)
``` 
_Decrements the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: seq_cst 


##  Function `vatomicsz_dec_rel`

```c
static void vatomicsz_dec_rel(vatomicsz_t *a)
``` 
_Decrements the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: release 


##  Function `vatomicsz_dec_rlx`

```c
static void vatomicsz_dec_rlx(vatomicsz_t *a)
``` 
_Decrements the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: relaxed 



---
