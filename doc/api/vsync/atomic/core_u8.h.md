#  [vsync](../README.md) / [atomic](README.md) / core_u8.h
_Atomic functions for vatomic8_t variables._ 

This file declares and documents the core atomic functions operating on vatomic8_t variables.

Please do not include this file directly, instead use: 

```c
#include <vsync/atomic/core.h>
```

 

---
# Functions 

| Function | Description |
|---|---|
| [vatomic8_init](core_u8.h.md#function-vatomic8_init) | Initializes the atomic variable a with value v.  |
| [vatomic8_read](core_u8.h.md#function-vatomic8_read) | Returns the value of the atomic variable pointed by a.  |
| [vatomic8_read_acq](core_u8.h.md#function-vatomic8_read_acq) | Returns the value of the atomic variable pointed by a.  |
| [vatomic8_read_rlx](core_u8.h.md#function-vatomic8_read_rlx) | Returns the value of the atomic variable pointed by a.  |
| [vatomic8_write](core_u8.h.md#function-vatomic8_write) | Writes value v in the atomic variable pointed by a.  |
| [vatomic8_write_rel](core_u8.h.md#function-vatomic8_write_rel) | Writes value v in the atomic variable pointed by a.  |
| [vatomic8_write_rlx](core_u8.h.md#function-vatomic8_write_rlx) | Writes value v in the atomic variable pointed by a.  |
| [vatomic8_xchg](core_u8.h.md#function-vatomic8_xchg) | Writes v in a and returns old value.  |
| [vatomic8_xchg_acq](core_u8.h.md#function-vatomic8_xchg_acq) | Writes v in a and returns old value.  |
| [vatomic8_xchg_rel](core_u8.h.md#function-vatomic8_xchg_rel) | Writes v in a and returns old value.  |
| [vatomic8_xchg_rlx](core_u8.h.md#function-vatomic8_xchg_rlx) | Writes v in a and returns old value.  |
| [vatomic8_cmpxchg](core_u8.h.md#function-vatomic8_cmpxchg) | Writes value v in a if e is the current value.  |
| [vatomic8_cmpxchg_acq](core_u8.h.md#function-vatomic8_cmpxchg_acq) | Writes value v in a if e is the current value.  |
| [vatomic8_cmpxchg_rel](core_u8.h.md#function-vatomic8_cmpxchg_rel) | Writes value v in a if e is the current value.  |
| [vatomic8_cmpxchg_rlx](core_u8.h.md#function-vatomic8_cmpxchg_rlx) | Writes value v in a if e is the current value.  |
| [vatomic8_get_max](core_u8.h.md#function-vatomic8_get_max) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomic8_get_max_acq](core_u8.h.md#function-vatomic8_get_max_acq) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomic8_get_max_rel](core_u8.h.md#function-vatomic8_get_max_rel) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomic8_get_max_rlx](core_u8.h.md#function-vatomic8_get_max_rlx) | Writes v to a if v is greater than *a and returns the old value.  |
| [vatomic8_max_get](core_u8.h.md#function-vatomic8_max_get) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomic8_max_get_acq](core_u8.h.md#function-vatomic8_max_get_acq) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomic8_max_get_rel](core_u8.h.md#function-vatomic8_max_get_rel) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomic8_max_get_rlx](core_u8.h.md#function-vatomic8_max_get_rlx) | Writes v to a if v is greater than *a and returns the new value.  |
| [vatomic8_max](core_u8.h.md#function-vatomic8_max) | Writes v to a if v is greater than *a.  |
| [vatomic8_max_rel](core_u8.h.md#function-vatomic8_max_rel) | Writes v to a if v is greater than *a.  |
| [vatomic8_max_rlx](core_u8.h.md#function-vatomic8_max_rlx) | Writes v to a if v is greater than *a.  |
| [vatomic8_get_and](core_u8.h.md#function-vatomic8_get_and) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomic8_get_and_acq](core_u8.h.md#function-vatomic8_get_and_acq) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomic8_get_and_rel](core_u8.h.md#function-vatomic8_get_and_rel) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomic8_get_and_rlx](core_u8.h.md#function-vatomic8_get_and_rlx) | Applies bitwise and to the value of a and returns the old value.  |
| [vatomic8_and_get](core_u8.h.md#function-vatomic8_and_get) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomic8_and_get_acq](core_u8.h.md#function-vatomic8_and_get_acq) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomic8_and_get_rel](core_u8.h.md#function-vatomic8_and_get_rel) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomic8_and_get_rlx](core_u8.h.md#function-vatomic8_and_get_rlx) | Applies bitwise and operation with v to the value of a and returns the new value.  |
| [vatomic8_and](core_u8.h.md#function-vatomic8_and) | Applies bitwise and operation with v to the value of a.  |
| [vatomic8_and_rel](core_u8.h.md#function-vatomic8_and_rel) | Applies bitwise and operation with v to the value of a.  |
| [vatomic8_and_rlx](core_u8.h.md#function-vatomic8_and_rlx) | Applies bitwise and operation with v to the value of a.  |
| [vatomic8_get_or](core_u8.h.md#function-vatomic8_get_or) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomic8_get_or_acq](core_u8.h.md#function-vatomic8_get_or_acq) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomic8_get_or_rel](core_u8.h.md#function-vatomic8_get_or_rel) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomic8_get_or_rlx](core_u8.h.md#function-vatomic8_get_or_rlx) | Applies bitwise or operation with v to the value of a and returns the old value.  |
| [vatomic8_or_get](core_u8.h.md#function-vatomic8_or_get) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomic8_or_get_acq](core_u8.h.md#function-vatomic8_or_get_acq) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomic8_or_get_rel](core_u8.h.md#function-vatomic8_or_get_rel) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomic8_or_get_rlx](core_u8.h.md#function-vatomic8_or_get_rlx) | Applies bitwise or operation with v to the value of a and returns the new value.  |
| [vatomic8_or](core_u8.h.md#function-vatomic8_or) | Applies bitwise or operation with v to the value of a.  |
| [vatomic8_or_rel](core_u8.h.md#function-vatomic8_or_rel) | Applies bitwise or operation with v to the value of a.  |
| [vatomic8_or_rlx](core_u8.h.md#function-vatomic8_or_rlx) | Applies bitwise or operation with v to the value of a.  |
| [vatomic8_get_xor](core_u8.h.md#function-vatomic8_get_xor) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomic8_get_xor_acq](core_u8.h.md#function-vatomic8_get_xor_acq) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomic8_get_xor_rel](core_u8.h.md#function-vatomic8_get_xor_rel) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomic8_get_xor_rlx](core_u8.h.md#function-vatomic8_get_xor_rlx) | Applies bitwise xor operation with v to the value of a and returns the old value.  |
| [vatomic8_xor_get](core_u8.h.md#function-vatomic8_xor_get) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomic8_xor_get_acq](core_u8.h.md#function-vatomic8_xor_get_acq) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomic8_xor_get_rel](core_u8.h.md#function-vatomic8_xor_get_rel) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomic8_xor_get_rlx](core_u8.h.md#function-vatomic8_xor_get_rlx) | Applies bitwise xor operation with v to the value of a and returns the new value.  |
| [vatomic8_xor](core_u8.h.md#function-vatomic8_xor) | Applies bitwise xor opeation with v to the value of a.  |
| [vatomic8_xor_rel](core_u8.h.md#function-vatomic8_xor_rel) | Applies bitwise xor opeation with v to the value of a.  |
| [vatomic8_xor_rlx](core_u8.h.md#function-vatomic8_xor_rlx) | Applies bitwise xor opeation with v to the value of a.  |
| [vatomic8_get_add](core_u8.h.md#function-vatomic8_get_add) | Adds v to the value of a and returns the old value.  |
| [vatomic8_get_add_acq](core_u8.h.md#function-vatomic8_get_add_acq) | Adds v to the value of a and returns the old value.  |
| [vatomic8_get_add_rel](core_u8.h.md#function-vatomic8_get_add_rel) | Adds v to the value of a and returns the old value.  |
| [vatomic8_get_add_rlx](core_u8.h.md#function-vatomic8_get_add_rlx) | Adds v to the value of a and returns the old value.  |
| [vatomic8_add_get](core_u8.h.md#function-vatomic8_add_get) | Adds v to the value of a and returns the new value.  |
| [vatomic8_add_get_acq](core_u8.h.md#function-vatomic8_add_get_acq) | Adds v to the value of a and returns the new value.  |
| [vatomic8_add_get_rel](core_u8.h.md#function-vatomic8_add_get_rel) | Adds v to the value of a and returns the new value.  |
| [vatomic8_add_get_rlx](core_u8.h.md#function-vatomic8_add_get_rlx) | Adds v to the value of a and returns the new value.  |
| [vatomic8_add](core_u8.h.md#function-vatomic8_add) | Adds v to the value of a.  |
| [vatomic8_add_rel](core_u8.h.md#function-vatomic8_add_rel) | Adds v to the value of a.  |
| [vatomic8_add_rlx](core_u8.h.md#function-vatomic8_add_rlx) | Adds v to the value of a.  |
| [vatomic8_get_inc](core_u8.h.md#function-vatomic8_get_inc) | Increments the value of a and returns the old value.  |
| [vatomic8_get_inc_acq](core_u8.h.md#function-vatomic8_get_inc_acq) | Increments the value of a and returns the old value.  |
| [vatomic8_get_inc_rel](core_u8.h.md#function-vatomic8_get_inc_rel) | Increments the value of a and returns the old value.  |
| [vatomic8_get_inc_rlx](core_u8.h.md#function-vatomic8_get_inc_rlx) | Increments the value of a and returns the old value.  |
| [vatomic8_inc_get](core_u8.h.md#function-vatomic8_inc_get) | Increments the value of a and returns the new value.  |
| [vatomic8_inc_get_acq](core_u8.h.md#function-vatomic8_inc_get_acq) | Increments the value of a and returns the new value.  |
| [vatomic8_inc_get_rel](core_u8.h.md#function-vatomic8_inc_get_rel) | Increments the value of a and returns the new value.  |
| [vatomic8_inc_get_rlx](core_u8.h.md#function-vatomic8_inc_get_rlx) | Increments the value of a and returns the new value.  |
| [vatomic8_inc](core_u8.h.md#function-vatomic8_inc) | Increments the value of a.  |
| [vatomic8_inc_rel](core_u8.h.md#function-vatomic8_inc_rel) | Increments the value of a.  |
| [vatomic8_inc_rlx](core_u8.h.md#function-vatomic8_inc_rlx) | Increments the value of a.  |
| [vatomic8_get_sub](core_u8.h.md#function-vatomic8_get_sub) | Subtracts v from a and returns the old value.  |
| [vatomic8_get_sub_acq](core_u8.h.md#function-vatomic8_get_sub_acq) | Subtracts v from a and returns the old value.  |
| [vatomic8_get_sub_rel](core_u8.h.md#function-vatomic8_get_sub_rel) | Subtracts v from a and returns the old value.  |
| [vatomic8_get_sub_rlx](core_u8.h.md#function-vatomic8_get_sub_rlx) | Subtracts v from a and returns the old value.  |
| [vatomic8_sub_get](core_u8.h.md#function-vatomic8_sub_get) | Subtracts v from a and returns the new value.  |
| [vatomic8_sub_get_acq](core_u8.h.md#function-vatomic8_sub_get_acq) | Subtracts v from a and returns the new value.  |
| [vatomic8_sub_get_rel](core_u8.h.md#function-vatomic8_sub_get_rel) | Subtracts v from a and returns the new value.  |
| [vatomic8_sub_get_rlx](core_u8.h.md#function-vatomic8_sub_get_rlx) | Subtracts v from a and returns the new value.  |
| [vatomic8_sub](core_u8.h.md#function-vatomic8_sub) | Subtracts v from a.  |
| [vatomic8_sub_rel](core_u8.h.md#function-vatomic8_sub_rel) | Subtracts v from a.  |
| [vatomic8_sub_rlx](core_u8.h.md#function-vatomic8_sub_rlx) | Subtracts v from a.  |
| [vatomic8_get_dec](core_u8.h.md#function-vatomic8_get_dec) | Decrements the value of a and returns the old value.  |
| [vatomic8_get_dec_acq](core_u8.h.md#function-vatomic8_get_dec_acq) | Decrements the value of a and returns the old value.  |
| [vatomic8_get_dec_rel](core_u8.h.md#function-vatomic8_get_dec_rel) | Decrements the value of a and returns the old value.  |
| [vatomic8_get_dec_rlx](core_u8.h.md#function-vatomic8_get_dec_rlx) | Decrements the value of a and returns the old value.  |
| [vatomic8_dec_get](core_u8.h.md#function-vatomic8_dec_get) | Decrements the value of a and returns the new value.  |
| [vatomic8_dec_get_acq](core_u8.h.md#function-vatomic8_dec_get_acq) | Decrements the value of a and returns the new value.  |
| [vatomic8_dec_get_rel](core_u8.h.md#function-vatomic8_dec_get_rel) | Decrements the value of a and returns the new value.  |
| [vatomic8_dec_get_rlx](core_u8.h.md#function-vatomic8_dec_get_rlx) | Decrements the value of a and returns the new value.  |
| [vatomic8_dec](core_u8.h.md#function-vatomic8_dec) | Decrements the value of a.  |
| [vatomic8_dec_rel](core_u8.h.md#function-vatomic8_dec_rel) | Decrements the value of a.  |
| [vatomic8_dec_rlx](core_u8.h.md#function-vatomic8_dec_rlx) | Decrements the value of a.  |

##  Function `vatomic8_init`

```c
static void vatomic8_init(vatomic8_t *a, vuint8_t v)
``` 
_Initializes the atomic variable a with value v._ 


The initialization is equivalent to an [vatomic8_write()](core_u8.h.md#function-vatomic8_write).



**Parameters:**

- `a`: atomic variable 
- `v`: initial value


**Memory ordering**: seq_cst 


##  Function `vatomic8_read`

```c
static vuint8_t vatomic8_read(const vatomic8_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: seq_cst 


##  Function `vatomic8_read_acq`

```c
static vuint8_t vatomic8_read_acq(const vatomic8_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: acquire 


##  Function `vatomic8_read_rlx`

```c
static vuint8_t vatomic8_read_rlx(const vatomic8_t *a)
``` 
_Returns the value of the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: relaxed 


##  Function `vatomic8_write`

```c
static void vatomic8_write(vatomic8_t *a, vuint8_t v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: seq_cst 


##  Function `vatomic8_write_rel`

```c
static void vatomic8_write_rel(vatomic8_t *a, vuint8_t v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: release 


##  Function `vatomic8_write_rlx`

```c
static void vatomic8_write_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Writes value v in the atomic variable pointed by a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: relaxed 


##  Function `vatomic8_xchg`

```c
static vuint8_t vatomic8_xchg(vatomic8_t *a, vuint8_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic8_xchg_acq`

```c
static vuint8_t vatomic8_xchg_acq(vatomic8_t *a, vuint8_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic8_xchg_rel`

```c
static vuint8_t vatomic8_xchg_rel(vatomic8_t *a, vuint8_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic8_xchg_rlx`

```c
static vuint8_t vatomic8_xchg_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Writes v in a and returns old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic8_cmpxchg`

```c
static vuint8_t vatomic8_cmpxchg(vatomic8_t *a, vuint8_t e, vuint8_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomic8_cmpxchg_acq`

```c
static vuint8_t vatomic8_cmpxchg_acq(vatomic8_t *a, vuint8_t e, vuint8_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomic8_cmpxchg_rel`

```c
static vuint8_t vatomic8_cmpxchg_rel(vatomic8_t *a, vuint8_t e, vuint8_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomic8_cmpxchg_rlx`

```c
static vuint8_t vatomic8_cmpxchg_rlx(vatomic8_t *a, vuint8_t e, vuint8_t v)
``` 
_Writes value v in a if e is the current value._ 




**Parameters:**

- `a`: atomic variable 
- `e`: expected value 
- `v`: new value 


**Returns:** old value 



##  Function `vatomic8_get_max`

```c
static vuint8_t vatomic8_get_max(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: seq_cst 


##  Function `vatomic8_get_max_acq`

```c
static vuint8_t vatomic8_get_max_acq(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: acquire 


##  Function `vatomic8_get_max_rel`

```c
static vuint8_t vatomic8_get_max_rel(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: release 


##  Function `vatomic8_get_max_rlx`

```c
static vuint8_t vatomic8_get_max_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** old max value

**Memory ordering**: relaxed 


##  Function `vatomic8_max_get`

```c
static vuint8_t vatomic8_max_get(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: seq_cst 


##  Function `vatomic8_max_get_acq`

```c
static vuint8_t vatomic8_max_get_acq(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: acquire 


##  Function `vatomic8_max_get_rel`

```c
static vuint8_t vatomic8_max_get_rel(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: release 


##  Function `vatomic8_max_get_rlx`

```c
static vuint8_t vatomic8_max_get_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value 


**Returns:** new max value

**Memory ordering**: relaxed 


##  Function `vatomic8_max`

```c
static void vatomic8_max(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value


**Memory ordering**: seq_cst 


##  Function `vatomic8_max_rel`

```c
static void vatomic8_max_rel(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value


**Memory ordering**: release 


##  Function `vatomic8_max_rlx`

```c
static void vatomic8_max_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Writes v to a if v is greater than *a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: potential max value


**Memory ordering**: relaxed 


##  Function `vatomic8_get_and`

```c
static vuint8_t vatomic8_get_and(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic8_get_and_acq`

```c
static vuint8_t vatomic8_get_and_acq(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic8_get_and_rel`

```c
static vuint8_t vatomic8_get_and_rel(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic8_get_and_rlx`

```c
static vuint8_t vatomic8_get_and_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic8_and_get`

```c
static vuint8_t vatomic8_and_get(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic8_and_get_acq`

```c
static vuint8_t vatomic8_and_get_acq(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic8_and_get_rel`

```c
static vuint8_t vatomic8_and_get_rel(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic8_and_get_rlx`

```c
static vuint8_t vatomic8_and_get_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic8_and`

```c
static void vatomic8_and(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: seq_cst 


##  Function `vatomic8_and_rel`

```c
static void vatomic8_and_rel(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: release 


##  Function `vatomic8_and_rlx`

```c
static void vatomic8_and_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise and operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: relaxed 


##  Function `vatomic8_get_or`

```c
static vuint8_t vatomic8_get_or(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic8_get_or_acq`

```c
static vuint8_t vatomic8_get_or_acq(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic8_get_or_rel`

```c
static vuint8_t vatomic8_get_or_rel(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic8_get_or_rlx`

```c
static vuint8_t vatomic8_get_or_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic8_or_get`

```c
static vuint8_t vatomic8_or_get(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic8_or_get_acq`

```c
static vuint8_t vatomic8_or_get_acq(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic8_or_get_rel`

```c
static vuint8_t vatomic8_or_get_rel(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic8_or_get_rlx`

```c
static vuint8_t vatomic8_or_get_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic8_or`

```c
static void vatomic8_or(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: seq_cst 


##  Function `vatomic8_or_rel`

```c
static void vatomic8_or_rel(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: release 


##  Function `vatomic8_or_rlx`

```c
static void vatomic8_or_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise or operation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: relaxed 


##  Function `vatomic8_get_xor`

```c
static vuint8_t vatomic8_get_xor(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic8_get_xor_acq`

```c
static vuint8_t vatomic8_get_xor_acq(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic8_get_xor_rel`

```c
static vuint8_t vatomic8_get_xor_rel(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic8_get_xor_rlx`

```c
static vuint8_t vatomic8_get_xor_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic8_xor_get`

```c
static vuint8_t vatomic8_xor_get(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic8_xor_get_acq`

```c
static vuint8_t vatomic8_xor_get_acq(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic8_xor_get_rel`

```c
static vuint8_t vatomic8_xor_get_rel(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic8_xor_get_rlx`

```c
static vuint8_t vatomic8_xor_get_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor operation with v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic8_xor`

```c
static void vatomic8_xor(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor opeation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: seq_cst 


##  Function `vatomic8_xor_rel`

```c
static void vatomic8_xor_rel(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor opeation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: release 


##  Function `vatomic8_xor_rlx`

```c
static void vatomic8_xor_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Applies bitwise xor opeation with v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: mask value


**Memory ordering**: relaxed 


##  Function `vatomic8_get_add`

```c
static vuint8_t vatomic8_get_add(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic8_get_add_acq`

```c
static vuint8_t vatomic8_get_add_acq(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic8_get_add_rel`

```c
static vuint8_t vatomic8_get_add_rel(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic8_get_add_rlx`

```c
static vuint8_t vatomic8_get_add_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic8_add_get`

```c
static vuint8_t vatomic8_add_get(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic8_add_get_acq`

```c
static vuint8_t vatomic8_add_get_acq(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic8_add_get_rel`

```c
static vuint8_t vatomic8_add_get_rel(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic8_add_get_rlx`

```c
static vuint8_t vatomic8_add_get_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic8_add`

```c
static void vatomic8_add(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value


**Memory ordering**: seq_cst 


##  Function `vatomic8_add_rel`

```c
static void vatomic8_add_rel(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value


**Memory ordering**: release 


##  Function `vatomic8_add_rlx`

```c
static void vatomic8_add_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Adds v to the value of a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: addend value


**Memory ordering**: relaxed 


##  Function `vatomic8_get_inc`

```c
static vuint8_t vatomic8_get_inc(vatomic8_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic8_get_inc_acq`

```c
static vuint8_t vatomic8_get_inc_acq(vatomic8_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic8_get_inc_rel`

```c
static vuint8_t vatomic8_get_inc_rel(vatomic8_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic8_get_inc_rlx`

```c
static vuint8_t vatomic8_get_inc_rlx(vatomic8_t *a)
``` 
_Increments the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic8_inc_get`

```c
static vuint8_t vatomic8_inc_get(vatomic8_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic8_inc_get_acq`

```c
static vuint8_t vatomic8_inc_get_acq(vatomic8_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic8_inc_get_rel`

```c
static vuint8_t vatomic8_inc_get_rel(vatomic8_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic8_inc_get_rlx`

```c
static vuint8_t vatomic8_inc_get_rlx(vatomic8_t *a)
``` 
_Increments the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic8_inc`

```c
static void vatomic8_inc(vatomic8_t *a)
``` 
_Increments the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: seq_cst 


##  Function `vatomic8_inc_rel`

```c
static void vatomic8_inc_rel(vatomic8_t *a)
``` 
_Increments the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: release 


##  Function `vatomic8_inc_rlx`

```c
static void vatomic8_inc_rlx(vatomic8_t *a)
``` 
_Increments the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: relaxed 


##  Function `vatomic8_get_sub`

```c
static vuint8_t vatomic8_get_sub(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic8_get_sub_acq`

```c
static vuint8_t vatomic8_get_sub_acq(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic8_get_sub_rel`

```c
static vuint8_t vatomic8_get_sub_rel(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic8_get_sub_rlx`

```c
static vuint8_t vatomic8_get_sub_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic8_sub_get`

```c
static vuint8_t vatomic8_sub_get(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic8_sub_get_acq`

```c
static vuint8_t vatomic8_sub_get_acq(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic8_sub_get_rel`

```c
static vuint8_t vatomic8_sub_get_rel(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic8_sub_get_rlx`

```c
static vuint8_t vatomic8_sub_get_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic8_sub`

```c
static void vatomic8_sub(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value


**Memory ordering**: seq_cst 


##  Function `vatomic8_sub_rel`

```c
static void vatomic8_sub_rel(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value


**Memory ordering**: release 


##  Function `vatomic8_sub_rlx`

```c
static void vatomic8_sub_rlx(vatomic8_t *a, vuint8_t v)
``` 
_Subtracts v from a._ 




**Parameters:**

- `a`: atomic variable 
- `v`: subtrahend value


**Memory ordering**: relaxed 


##  Function `vatomic8_get_dec`

```c
static vuint8_t vatomic8_get_dec(vatomic8_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic8_get_dec_acq`

```c
static vuint8_t vatomic8_get_dec_acq(vatomic8_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic8_get_dec_rel`

```c
static vuint8_t vatomic8_get_dec_rel(vatomic8_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic8_get_dec_rlx`

```c
static vuint8_t vatomic8_get_dec_rlx(vatomic8_t *a)
``` 
_Decrements the value of a and returns the old value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic8_dec_get`

```c
static vuint8_t vatomic8_dec_get(vatomic8_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Function `vatomic8_dec_get_acq`

```c
static vuint8_t vatomic8_dec_get_acq(vatomic8_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: acquire 


##  Function `vatomic8_dec_get_rel`

```c
static vuint8_t vatomic8_dec_get_rel(vatomic8_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: release 


##  Function `vatomic8_dec_get_rlx`

```c
static vuint8_t vatomic8_dec_get_rlx(vatomic8_t *a)
``` 
_Decrements the value of a and returns the new value._ 




**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: relaxed 


##  Function `vatomic8_dec`

```c
static void vatomic8_dec(vatomic8_t *a)
``` 
_Decrements the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: seq_cst 


##  Function `vatomic8_dec_rel`

```c
static void vatomic8_dec_rel(vatomic8_t *a)
``` 
_Decrements the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: release 


##  Function `vatomic8_dec_rlx`

```c
static void vatomic8_dec_rlx(vatomic8_t *a)
``` 
_Decrements the value of a._ 




**Parameters:**

- `a`: atomic variable


**Memory ordering**: relaxed 



---
