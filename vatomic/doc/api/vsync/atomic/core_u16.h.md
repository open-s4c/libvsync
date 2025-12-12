#  [vsync](../README.md) / [atomic](README.md) / core_u16.h
_Atomic functions for vatomic16_t variables._ 

This file declares and documents the core atomic functions operating on vatomic16_t variables.

Please do not include this file directly, instead use: 

```c
#include <vsync/atomic/core.h>
```

 

---
# Functions 

| Function | Description |
|---|---|
| [vatomic16_init](core_u16.h.md#function-vatomic16_init) | Initializes the atomic variable a with value v.  |
| [vatomic16_read](core_u16.h.md#function-vatomic16_read) | Returns the value of the atomic variable pointed by a.  |
| [vatomic16_read_acq](core_u16.h.md#function-vatomic16_read_acq) | Returns the value of the atomic variable pointed by a.  |
| [vatomic16_read_rlx](core_u16.h.md#function-vatomic16_read_rlx) | Returns the value of the atomic variable pointed by a.  |
| [vatomic16_write](core_u16.h.md#function-vatomic16_write) | Writes value v in the atomic variable pointed by a.  |
| [vatomic16_write_rel](core_u16.h.md#function-vatomic16_write_rel) | Writes value v in the atomic variable pointed by a.  |
| [vatomic16_write_rlx](core_u16.h.md#function-vatomic16_write_rlx) | Writes value v in the atomic variable pointed by a.  |
| [vatomic16_xchg](core_u16.h.md#function-vatomic16_xchg) | Writes v in a and returns old value.  |
| [vatomic16_xchg_acq](core_u16.h.md#function-vatomic16_xchg_acq) | Writes v in a and returns old value.  |
| [vatomic16_xchg_rel](core_u16.h.md#function-vatomic16_xchg_rel) | Writes v in a and returns old value.  |
| [vatomic16_xchg_rlx](core_u16.h.md#function-vatomic16_xchg_rlx) | Writes v in a and returns old value.  |
| [vatomic16_cmpxchg](core_u16.h.md#function-vatomic16_cmpxchg) | Writes value v in a if e is the current value.  |
| [vatomic16_cmpxchg_acq](core_u16.h.md#function-vatomic16_cmpxchg_acq) | Writes value v in a if e is the current value.  |
| [vatomic16_cmpxchg_rel](core_u16.h.md#function-vatomic16_cmpxchg_rel) | Writes value v in a if e is the current value.  |
| [vatomic16_cmpxchg_rlx](core_u16.h.md#function-vatomic16_cmpxchg_rlx) | Writes value v in a if e is the current value.  |
| [vatomic16_get_max](core_u16.h.md#function-vatomic16_get_max) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomic16_get_max_acq](core_u16.h.md#function-vatomic16_get_max_acq) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomic16_get_max_rel](core_u16.h.md#function-vatomic16_get_max_rel) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomic16_get_max_rlx](core_u16.h.md#function-vatomic16_get_max_rlx) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomic16_max_get](core_u16.h.md#function-vatomic16_max_get) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomic16_max_get_acq](core_u16.h.md#function-vatomic16_max_get_acq) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomic16_max_get_rel](core_u16.h.md#function-vatomic16_max_get_rel) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomic16_max_get_rlx](core_u16.h.md#function-vatomic16_max_get_rlx) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomic16_max](core_u16.h.md#function-vatomic16_max) | Writes v to a if v is greater than *a.  |
| [vatomic16_max_rel](core_u16.h.md#function-vatomic16_max_rel) | Writes v to a if v is greater than *a.  |
| [vatomic16_max_rlx](core_u16.h.md#function-vatomic16_max_rlx) | Writes v to a if v is greater than *a.  |
| [vatomic16_get_and](core_u16.h.md#function-vatomic16_get_and) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomic16_get_and_acq](core_u16.h.md#function-vatomic16_get_and_acq) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomic16_get_and_rel](core_u16.h.md#function-vatomic16_get_and_rel) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomic16_get_and_rlx](core_u16.h.md#function-vatomic16_get_and_rlx) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomic16_and_get](core_u16.h.md#function-vatomic16_and_get) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomic16_and_get_acq](core_u16.h.md#function-vatomic16_and_get_acq) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomic16_and_get_rel](core_u16.h.md#function-vatomic16_and_get_rel) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomic16_and_get_rlx](core_u16.h.md#function-vatomic16_and_get_rlx) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomic16_and](core_u16.h.md#function-vatomic16_and) | Applies bitwise and operation with v to the value of a.  |
| [vatomic16_and_rel](core_u16.h.md#function-vatomic16_and_rel) | Applies bitwise and operation with v to the value of a.  |
| [vatomic16_and_rlx](core_u16.h.md#function-vatomic16_and_rlx) | Applies bitwise and operation with v to the value of a.  |
| [vatomic16_get_or](core_u16.h.md#function-vatomic16_get_or) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomic16_get_or_acq](core_u16.h.md#function-vatomic16_get_or_acq) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomic16_get_or_rel](core_u16.h.md#function-vatomic16_get_or_rel) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomic16_get_or_rlx](core_u16.h.md#function-vatomic16_get_or_rlx) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomic16_or_get](core_u16.h.md#function-vatomic16_or_get) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomic16_or_get_acq](core_u16.h.md#function-vatomic16_or_get_acq) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomic16_or_get_rel](core_u16.h.md#function-vatomic16_or_get_rel) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomic16_or_get_rlx](core_u16.h.md#function-vatomic16_or_get_rlx) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomic16_or](core_u16.h.md#function-vatomic16_or) | Applies bitwise or operation with v to the value of a.  |
| [vatomic16_or_rel](core_u16.h.md#function-vatomic16_or_rel) | Applies bitwise or operation with v to the value of a.  |
| [vatomic16_or_rlx](core_u16.h.md#function-vatomic16_or_rlx) | Applies bitwise or operation with v to the value of a.  |
| [vatomic16_get_xor](core_u16.h.md#function-vatomic16_get_xor) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomic16_get_xor_acq](core_u16.h.md#function-vatomic16_get_xor_acq) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomic16_get_xor_rel](core_u16.h.md#function-vatomic16_get_xor_rel) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomic16_get_xor_rlx](core_u16.h.md#function-vatomic16_get_xor_rlx) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomic16_xor_get](core_u16.h.md#function-vatomic16_xor_get) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomic16_xor_get_acq](core_u16.h.md#function-vatomic16_xor_get_acq) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomic16_xor_get_rel](core_u16.h.md#function-vatomic16_xor_get_rel) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomic16_xor_get_rlx](core_u16.h.md#function-vatomic16_xor_get_rlx) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomic16_xor](core_u16.h.md#function-vatomic16_xor) | Applies bitwise xor opeation with v to the value of a.  |
| [vatomic16_xor_rel](core_u16.h.md#function-vatomic16_xor_rel) | Applies bitwise xor opeation with v to the value of a.  |
| [vatomic16_xor_rlx](core_u16.h.md#function-vatomic16_xor_rlx) | Applies bitwise xor opeation with v to the value of a.  |
| [vatomic16_get_add](core_u16.h.md#function-vatomic16_get_add) | Adds v to the value of a and returns the old value.  |
| [vatomic16_get_add_acq](core_u16.h.md#function-vatomic16_get_add_acq) | Adds v to the value of a and returns the old value.  |
| [vatomic16_get_add_rel](core_u16.h.md#function-vatomic16_get_add_rel) | Adds v to the value of a and returns the old value.  |
| [vatomic16_get_add_rlx](core_u16.h.md#function-vatomic16_get_add_rlx) | Adds v to the value of a and returns the old value.  |
| [vatomic16_add_get](core_u16.h.md#function-vatomic16_add_get) | Adds v to the value of a and returns the new value.  |
| [vatomic16_add_get_acq](core_u16.h.md#function-vatomic16_add_get_acq) | Adds v to the value of a and returns the new value.  |
| [vatomic16_add_get_rel](core_u16.h.md#function-vatomic16_add_get_rel) | Adds v to the value of a and returns the new value.  |
| [vatomic16_add_get_rlx](core_u16.h.md#function-vatomic16_add_get_rlx) | Adds v to the value of a and returns the new value.  |
| [vatomic16_add](core_u16.h.md#function-vatomic16_add) | Adds v to the value of a.  |
| [vatomic16_add_rel](core_u16.h.md#function-vatomic16_add_rel) | Adds v to the value of a.  |
| [vatomic16_add_rlx](core_u16.h.md#function-vatomic16_add_rlx) | Adds v to the value of a.  |
| [vatomic16_get_inc](core_u16.h.md#function-vatomic16_get_inc) | Increments the value of a and returns the old value.  |
| [vatomic16_get_inc_acq](core_u16.h.md#function-vatomic16_get_inc_acq) | Increments the value of a and returns the old value.  |
| [vatomic16_get_inc_rel](core_u16.h.md#function-vatomic16_get_inc_rel) | Increments the value of a and returns the old value.  |
| [vatomic16_get_inc_rlx](core_u16.h.md#function-vatomic16_get_inc_rlx) | Increments the value of a and returns the old value.  |
| [vatomic16_inc_get](core_u16.h.md#function-vatomic16_inc_get) | Increments the value of a and returns the new value.  |
| [vatomic16_inc_get_acq](core_u16.h.md#function-vatomic16_inc_get_acq) | Increments the value of a and returns the new value.  |
| [vatomic16_inc_get_rel](core_u16.h.md#function-vatomic16_inc_get_rel) | Increments the value of a and returns the new value.  |
| [vatomic16_inc_get_rlx](core_u16.h.md#function-vatomic16_inc_get_rlx) | Increments the value of a and returns the new value.  |
| [vatomic16_inc](core_u16.h.md#function-vatomic16_inc) | Increments the value of a.  |
| [vatomic16_inc_rel](core_u16.h.md#function-vatomic16_inc_rel) | Increments the value of a.  |
| [vatomic16_inc_rlx](core_u16.h.md#function-vatomic16_inc_rlx) | Increments the value of a.  |
| [vatomic16_get_sub](core_u16.h.md#function-vatomic16_get_sub) | Subtracts v from a and returns the old value.  |
| [vatomic16_get_sub_acq](core_u16.h.md#function-vatomic16_get_sub_acq) | Subtracts v from a and returns the old value.  |
| [vatomic16_get_sub_rel](core_u16.h.md#function-vatomic16_get_sub_rel) | Subtracts v from a and returns the old value.  |
| [vatomic16_get_sub_rlx](core_u16.h.md#function-vatomic16_get_sub_rlx) | Subtracts v from a and returns the old value.  |
| [vatomic16_sub_get](core_u16.h.md#function-vatomic16_sub_get) | Subtracts v from a and returns the new value.  |
| [vatomic16_sub_get_acq](core_u16.h.md#function-vatomic16_sub_get_acq) | Subtracts v from a and returns the new value.  |
| [vatomic16_sub_get_rel](core_u16.h.md#function-vatomic16_sub_get_rel) | Subtracts v from a and returns the new value.  |
| [vatomic16_sub_get_rlx](core_u16.h.md#function-vatomic16_sub_get_rlx) | Subtracts v from a and returns the new value.  |
| [vatomic16_sub](core_u16.h.md#function-vatomic16_sub) | Subtracts v from a.  |
| [vatomic16_sub_rel](core_u16.h.md#function-vatomic16_sub_rel) | Subtracts v from a.  |
| [vatomic16_sub_rlx](core_u16.h.md#function-vatomic16_sub_rlx) | Subtracts v from a.  |
| [vatomic16_get_dec](core_u16.h.md#function-vatomic16_get_dec) | Decrements the value of a and returns the old value.  |
| [vatomic16_get_dec_acq](core_u16.h.md#function-vatomic16_get_dec_acq) | Decrements the value of a and returns the old value.  |
| [vatomic16_get_dec_rel](core_u16.h.md#function-vatomic16_get_dec_rel) | Decrements the value of a and returns the old value.  |
| [vatomic16_get_dec_rlx](core_u16.h.md#function-vatomic16_get_dec_rlx) | Decrements the value of a and returns the old value.  |
| [vatomic16_dec_get](core_u16.h.md#function-vatomic16_dec_get) | Decrements the value of a and returns the new value.  |
| [vatomic16_dec_get_acq](core_u16.h.md#function-vatomic16_dec_get_acq) | Decrements the value of a and returns the new value.  |
| [vatomic16_dec_get_rel](core_u16.h.md#function-vatomic16_dec_get_rel) | Decrements the value of a and returns the new value.  |
| [vatomic16_dec_get_rlx](core_u16.h.md#function-vatomic16_dec_get_rlx) | Decrements the value of a and returns the new value.  |
| [vatomic16_dec](core_u16.h.md#function-vatomic16_dec) | Decrements the value of a.  |
| [vatomic16_dec_rel](core_u16.h.md#function-vatomic16_dec_rel) | Decrements the value of a.  |
| [vatomic16_dec_rlx](core_u16.h.md#function-vatomic16_dec_rlx) | Decrements the value of a.  |

##  Function `vatomic16_init`

```c
static void vatomic16_init(vatomic16_t *a, vuint16_t v)
``` 
_Initializes the atomic variable a with value v._ 


The initialization is equivalent to an [vatomic16_write()](core_u16.h.md#function-vatomic16_write).



**Parameters:**

- `a`: atomic variable 
- `v`: initial value


**Memory ordering**: seq_cst 


##  Function `vatomic16_read`

```c
static vuint16_t vatomic16_read(const vatomic16_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: seq_cst 


##  Function `vatomic16_read_acq`

```c
static vuint16_t vatomic16_read_acq(const vatomic16_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: acquire 


##  Function `vatomic16_read_rlx`

```c
static vuint16_t vatomic16_read_rlx(const vatomic16_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: relaxed 


##  Function `vatomic16_write`

```c
static void vatomic16_write(vatomic16_t *a, vuint16_t v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: seq_cst 


##  Function `vatomic16_write_rel`

```c
static void vatomic16_write_rel(vatomic16_t *a, vuint16_t v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: release 


##  Function `vatomic16_write_rlx`

```c
static void vatomic16_write_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: relaxed 


##  Function `vatomic16_xchg`

```c
static vuint16_t vatomic16_xchg(vatomic16_t *a, vuint16_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic16_xchg_acq`

```c
static vuint16_t vatomic16_xchg_acq(vatomic16_t *a, vuint16_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic16_xchg_rel`

```c
static vuint16_t vatomic16_xchg_rel(vatomic16_t *a, vuint16_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic16_xchg_rlx`

```c
static vuint16_t vatomic16_xchg_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic16_cmpxchg`

```c
static vuint16_t vatomic16_cmpxchg(vatomic16_t *a, vuint16_t e, vuint16_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomic16_cmpxchg_acq`

```c
static vuint16_t vatomic16_cmpxchg_acq(vatomic16_t *a, vuint16_t e, vuint16_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomic16_cmpxchg_rel`

```c
static vuint16_t vatomic16_cmpxchg_rel(vatomic16_t *a, vuint16_t e, vuint16_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomic16_cmpxchg_rlx`

```c
static vuint16_t vatomic16_cmpxchg_rlx(vatomic16_t *a, vuint16_t e, vuint16_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomic16_get_max`

```c
static vuint16_t vatomic16_get_max(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: seq_cst 


##  Function `vatomic16_get_max_acq`

```c
static vuint16_t vatomic16_get_max_acq(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: acquire 


##  Function `vatomic16_get_max_rel`

```c
static vuint16_t vatomic16_get_max_rel(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: release 


##  Function `vatomic16_get_max_rlx`

```c
static vuint16_t vatomic16_get_max_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: relaxed 


##  Function `vatomic16_max_get`

```c
static vuint16_t vatomic16_max_get(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: seq_cst 


##  Function `vatomic16_max_get_acq`

```c
static vuint16_t vatomic16_max_get_acq(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: acquire 


##  Function `vatomic16_max_get_rel`

```c
static vuint16_t vatomic16_max_get_rel(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: release 


##  Function `vatomic16_max_get_rlx`

```c
static vuint16_t vatomic16_max_get_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: relaxed 


##  Function `vatomic16_max`

```c
static void vatomic16_max(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value


**Memory ordering**: seq_cst 


##  Function `vatomic16_max_rel`

```c
static void vatomic16_max_rel(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value


**Memory ordering**: release 


##  Function `vatomic16_max_rlx`

```c
static void vatomic16_max_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Writes v to a if v is greater than *a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value


**Memory ordering**: relaxed 


##  Function `vatomic16_get_and`

```c
static vuint16_t vatomic16_get_and(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic16_get_and_acq`

```c
static vuint16_t vatomic16_get_and_acq(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic16_get_and_rel`

```c
static vuint16_t vatomic16_get_and_rel(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic16_get_and_rlx`

```c
static vuint16_t vatomic16_get_and_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic16_and_get`

```c
static vuint16_t vatomic16_and_get(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic16_and_get_acq`

```c
static vuint16_t vatomic16_and_get_acq(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic16_and_get_rel`

```c
static vuint16_t vatomic16_and_get_rel(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic16_and_get_rlx`

```c
static vuint16_t vatomic16_and_get_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic16_and`

```c
static void vatomic16_and(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: seq_cst 


##  Function `vatomic16_and_rel`

```c
static void vatomic16_and_rel(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: release 


##  Function `vatomic16_and_rlx`

```c
static void vatomic16_and_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise and operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: relaxed 


##  Function `vatomic16_get_or`

```c
static vuint16_t vatomic16_get_or(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic16_get_or_acq`

```c
static vuint16_t vatomic16_get_or_acq(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic16_get_or_rel`

```c
static vuint16_t vatomic16_get_or_rel(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic16_get_or_rlx`

```c
static vuint16_t vatomic16_get_or_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic16_or_get`

```c
static vuint16_t vatomic16_or_get(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic16_or_get_acq`

```c
static vuint16_t vatomic16_or_get_acq(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic16_or_get_rel`

```c
static vuint16_t vatomic16_or_get_rel(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic16_or_get_rlx`

```c
static vuint16_t vatomic16_or_get_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic16_or`

```c
static void vatomic16_or(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: seq_cst 


##  Function `vatomic16_or_rel`

```c
static void vatomic16_or_rel(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: release 


##  Function `vatomic16_or_rlx`

```c
static void vatomic16_or_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise or operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: relaxed 


##  Function `vatomic16_get_xor`

```c
static vuint16_t vatomic16_get_xor(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic16_get_xor_acq`

```c
static vuint16_t vatomic16_get_xor_acq(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic16_get_xor_rel`

```c
static vuint16_t vatomic16_get_xor_rel(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic16_get_xor_rlx`

```c
static vuint16_t vatomic16_get_xor_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic16_xor_get`

```c
static vuint16_t vatomic16_xor_get(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic16_xor_get_acq`

```c
static vuint16_t vatomic16_xor_get_acq(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic16_xor_get_rel`

```c
static vuint16_t vatomic16_xor_get_rel(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic16_xor_get_rlx`

```c
static vuint16_t vatomic16_xor_get_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic16_xor`

```c
static void vatomic16_xor(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor opeation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: seq_cst 


##  Function `vatomic16_xor_rel`

```c
static void vatomic16_xor_rel(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor opeation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: release 


##  Function `vatomic16_xor_rlx`

```c
static void vatomic16_xor_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Applies bitwise xor opeation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: relaxed 


##  Function `vatomic16_get_add`

```c
static vuint16_t vatomic16_get_add(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic16_get_add_acq`

```c
static vuint16_t vatomic16_get_add_acq(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic16_get_add_rel`

```c
static vuint16_t vatomic16_get_add_rel(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic16_get_add_rlx`

```c
static vuint16_t vatomic16_get_add_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic16_add_get`

```c
static vuint16_t vatomic16_add_get(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic16_add_get_acq`

```c
static vuint16_t vatomic16_add_get_acq(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic16_add_get_rel`

```c
static vuint16_t vatomic16_add_get_rel(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic16_add_get_rlx`

```c
static vuint16_t vatomic16_add_get_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic16_add`

```c
static void vatomic16_add(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value


**Memory ordering**: seq_cst 


##  Function `vatomic16_add_rel`

```c
static void vatomic16_add_rel(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value


**Memory ordering**: release 


##  Function `vatomic16_add_rlx`

```c
static void vatomic16_add_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Adds v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value


**Memory ordering**: relaxed 


##  Function `vatomic16_get_inc`

```c
static vuint16_t vatomic16_get_inc(vatomic16_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic16_get_inc_acq`

```c
static vuint16_t vatomic16_get_inc_acq(vatomic16_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic16_get_inc_rel`

```c
static vuint16_t vatomic16_get_inc_rel(vatomic16_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic16_get_inc_rlx`

```c
static vuint16_t vatomic16_get_inc_rlx(vatomic16_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic16_inc_get`

```c
static vuint16_t vatomic16_inc_get(vatomic16_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic16_inc_get_acq`

```c
static vuint16_t vatomic16_inc_get_acq(vatomic16_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic16_inc_get_rel`

```c
static vuint16_t vatomic16_inc_get_rel(vatomic16_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic16_inc_get_rlx`

```c
static vuint16_t vatomic16_inc_get_rlx(vatomic16_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic16_inc`

```c
static void vatomic16_inc(vatomic16_t *a)
``` 
_Increments the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: seq_cst 


##  Function `vatomic16_inc_rel`

```c
static void vatomic16_inc_rel(vatomic16_t *a)
``` 
_Increments the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: release 


##  Function `vatomic16_inc_rlx`

```c
static void vatomic16_inc_rlx(vatomic16_t *a)
``` 
_Increments the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: relaxed 


##  Function `vatomic16_get_sub`

```c
static vuint16_t vatomic16_get_sub(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic16_get_sub_acq`

```c
static vuint16_t vatomic16_get_sub_acq(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic16_get_sub_rel`

```c
static vuint16_t vatomic16_get_sub_rel(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic16_get_sub_rlx`

```c
static vuint16_t vatomic16_get_sub_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic16_sub_get`

```c
static vuint16_t vatomic16_sub_get(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic16_sub_get_acq`

```c
static vuint16_t vatomic16_sub_get_acq(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic16_sub_get_rel`

```c
static vuint16_t vatomic16_sub_get_rel(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic16_sub_get_rlx`

```c
static vuint16_t vatomic16_sub_get_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic16_sub`

```c
static void vatomic16_sub(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value


**Memory ordering**: seq_cst 


##  Function `vatomic16_sub_rel`

```c
static void vatomic16_sub_rel(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value


**Memory ordering**: release 


##  Function `vatomic16_sub_rlx`

```c
static void vatomic16_sub_rlx(vatomic16_t *a, vuint16_t v)
``` 
_Subtracts v from a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value


**Memory ordering**: relaxed 


##  Function `vatomic16_get_dec`

```c
static vuint16_t vatomic16_get_dec(vatomic16_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic16_get_dec_acq`

```c
static vuint16_t vatomic16_get_dec_acq(vatomic16_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic16_get_dec_rel`

```c
static vuint16_t vatomic16_get_dec_rel(vatomic16_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic16_get_dec_rlx`

```c
static vuint16_t vatomic16_get_dec_rlx(vatomic16_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic16_dec_get`

```c
static vuint16_t vatomic16_dec_get(vatomic16_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic16_dec_get_acq`

```c
static vuint16_t vatomic16_dec_get_acq(vatomic16_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic16_dec_get_rel`

```c
static vuint16_t vatomic16_dec_get_rel(vatomic16_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic16_dec_get_rlx`

```c
static vuint16_t vatomic16_dec_get_rlx(vatomic16_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic16_dec`

```c
static void vatomic16_dec(vatomic16_t *a)
``` 
_Decrements the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: seq_cst 


##  Function `vatomic16_dec_rel`

```c
static void vatomic16_dec_rel(vatomic16_t *a)
``` 
_Decrements the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: release 


##  Function `vatomic16_dec_rlx`

```c
static void vatomic16_dec_rlx(vatomic16_t *a)
``` 
_Decrements the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: relaxed 



---
