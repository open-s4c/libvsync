#  [vsync](../README.md) / [atomic](README.md) / await_u32.h
_Atomic await functions for vatomic32_t variables._ 

This file declares and documents the atomic await functions operating on vatomic32_t variables.

Please do not include this file directly, instead use: 

```c
#include <vsync/atomic/await.h>
```

 

---
# Functions 

| Function | Description |
|---|---|
| [vatomic32_await_lt](await_u32.h.md#function-vatomic32_await_lt) | Politely awaits the value pointed by a to be less than v.  |
| [vatomic32_await_lt_acq](await_u32.h.md#function-vatomic32_await_lt_acq) | Politely awaits the value pointed by a to be less than v.  |
| [vatomic32_await_lt_rlx](await_u32.h.md#function-vatomic32_await_lt_rlx) | Politely awaits the value pointed by a to be less than v.  |
| [vatomic32_await_le](await_u32.h.md#function-vatomic32_await_le) | Politely awaits the value pointed by a to be less than or equal to v.  |
| [vatomic32_await_le_acq](await_u32.h.md#function-vatomic32_await_le_acq) | Politely awaits the value pointed by a to be less than or equal to v.  |
| [vatomic32_await_le_rlx](await_u32.h.md#function-vatomic32_await_le_rlx) | Politely awaits the value pointed by a to be less than or equal to v.  |
| [vatomic32_await_gt](await_u32.h.md#function-vatomic32_await_gt) | Politely awaits the value pointed by a to be greater than v.  |
| [vatomic32_await_gt_acq](await_u32.h.md#function-vatomic32_await_gt_acq) | Politely awaits the value pointed by a to be greater than v.  |
| [vatomic32_await_gt_rlx](await_u32.h.md#function-vatomic32_await_gt_rlx) | Politely awaits the value pointed by a to be greater than v.  |
| [vatomic32_await_ge](await_u32.h.md#function-vatomic32_await_ge) | Politely awaits the value pointed by a to be greater than or equal to v.  |
| [vatomic32_await_ge_acq](await_u32.h.md#function-vatomic32_await_ge_acq) | Politely awaits the value pointed by a to be greater than or equal to v.  |
| [vatomic32_await_ge_rlx](await_u32.h.md#function-vatomic32_await_ge_rlx) | Politely awaits the value pointed by a to be greater than or equal to v.  |
| [vatomic32_await_neq](await_u32.h.md#function-vatomic32_await_neq) | Politely awaits the value pointed by a to be different than v.  |
| [vatomic32_await_neq_acq](await_u32.h.md#function-vatomic32_await_neq_acq) | Politely awaits the value pointed by a to be different than v.  |
| [vatomic32_await_neq_rlx](await_u32.h.md#function-vatomic32_await_neq_rlx) | Politely awaits the value pointed by a to be different than v.  |
| [vatomic32_await_eq](await_u32.h.md#function-vatomic32_await_eq) | Politely awaits the value pointed by a to be equal to v.  |
| [vatomic32_await_eq_acq](await_u32.h.md#function-vatomic32_await_eq_acq) | Politely awaits the value pointed by a to be equal to v.  |
| [vatomic32_await_eq_rlx](await_u32.h.md#function-vatomic32_await_eq_rlx) | Politely awaits the value pointed by a to be equal to v.  |
| [vatomic32_await_eq_add](await_u32.h.md#function-vatomic32_await_eq_add) | Politely awaits the value pointed by a to be equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_eq_add_acq](await_u32.h.md#function-vatomic32_await_eq_add_acq) | Politely awaits the value pointed by a to be equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_eq_add_rel](await_u32.h.md#function-vatomic32_await_eq_add_rel) | Politely awaits the value pointed by a to be equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_eq_add_rlx](await_u32.h.md#function-vatomic32_await_eq_add_rlx) | Politely awaits the value pointed by a to be equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_eq_sub](await_u32.h.md#function-vatomic32_await_eq_sub) | Politely awaits the value pointed by a to be equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_eq_sub_acq](await_u32.h.md#function-vatomic32_await_eq_sub_acq) | Politely awaits the value pointed by a to be equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_eq_sub_rel](await_u32.h.md#function-vatomic32_await_eq_sub_rel) | Politely awaits the value pointed by a to be equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_eq_sub_rlx](await_u32.h.md#function-vatomic32_await_eq_sub_rlx) | Politely awaits the value pointed by a to be equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_eq_set](await_u32.h.md#function-vatomic32_await_eq_set) | Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_eq_set_acq](await_u32.h.md#function-vatomic32_await_eq_set_acq) | Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_eq_set_rel](await_u32.h.md#function-vatomic32_await_eq_set_rel) | Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_eq_set_rlx](await_u32.h.md#function-vatomic32_await_eq_set_rlx) | Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_neq_add](await_u32.h.md#function-vatomic32_await_neq_add) | Politely awaits the value pointed by a to be different than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_neq_add_acq](await_u32.h.md#function-vatomic32_await_neq_add_acq) | Politely awaits the value pointed by a to be different than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_neq_add_rel](await_u32.h.md#function-vatomic32_await_neq_add_rel) | Politely awaits the value pointed by a to be different than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_neq_add_rlx](await_u32.h.md#function-vatomic32_await_neq_add_rlx) | Politely awaits the value pointed by a to be different than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_neq_sub](await_u32.h.md#function-vatomic32_await_neq_sub) | Politely awaits the value pointed by a to be different than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_neq_sub_acq](await_u32.h.md#function-vatomic32_await_neq_sub_acq) | Politely awaits the value pointed by a to be different than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_neq_sub_rel](await_u32.h.md#function-vatomic32_await_neq_sub_rel) | Politely awaits the value pointed by a to be different than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_neq_sub_rlx](await_u32.h.md#function-vatomic32_await_neq_sub_rlx) | Politely awaits the value pointed by a to be different than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_neq_set](await_u32.h.md#function-vatomic32_await_neq_set) | Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_neq_set_acq](await_u32.h.md#function-vatomic32_await_neq_set_acq) | Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_neq_set_rel](await_u32.h.md#function-vatomic32_await_neq_set_rel) | Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_neq_set_rlx](await_u32.h.md#function-vatomic32_await_neq_set_rlx) | Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_lt_add](await_u32.h.md#function-vatomic32_await_lt_add) | Politely awaits the value pointed by a to be less than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_lt_add_acq](await_u32.h.md#function-vatomic32_await_lt_add_acq) | Politely awaits the value pointed by a to be less than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_lt_add_rel](await_u32.h.md#function-vatomic32_await_lt_add_rel) | Politely awaits the value pointed by a to be less than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_lt_add_rlx](await_u32.h.md#function-vatomic32_await_lt_add_rlx) | Politely awaits the value pointed by a to be less than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_lt_sub](await_u32.h.md#function-vatomic32_await_lt_sub) | Politely awaits the value pointed by a to be less than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_lt_sub_acq](await_u32.h.md#function-vatomic32_await_lt_sub_acq) | Politely awaits the value pointed by a to be less than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_lt_sub_rel](await_u32.h.md#function-vatomic32_await_lt_sub_rel) | Politely awaits the value pointed by a to be less than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_lt_sub_rlx](await_u32.h.md#function-vatomic32_await_lt_sub_rlx) | Politely awaits the value pointed by a to be less than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_lt_set](await_u32.h.md#function-vatomic32_await_lt_set) | Politely awaits the value pointed by a to be less than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_lt_set_acq](await_u32.h.md#function-vatomic32_await_lt_set_acq) | Politely awaits the value pointed by a to be less than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_lt_set_rel](await_u32.h.md#function-vatomic32_await_lt_set_rel) | Politely awaits the value pointed by a to be less than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_lt_set_rlx](await_u32.h.md#function-vatomic32_await_lt_set_rlx) | Politely awaits the value pointed by a to be less than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_le_add](await_u32.h.md#function-vatomic32_await_le_add) | Politely awaits the value pointed by a to be less than or equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_le_add_acq](await_u32.h.md#function-vatomic32_await_le_add_acq) | Politely awaits the value pointed by a to be less than or equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_le_add_rel](await_u32.h.md#function-vatomic32_await_le_add_rel) | Politely awaits the value pointed by a to be less than or equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_le_add_rlx](await_u32.h.md#function-vatomic32_await_le_add_rlx) | Politely awaits the value pointed by a to be less than or equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_le_sub](await_u32.h.md#function-vatomic32_await_le_sub) | Politely awaits the value pointed by a to be less than or equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_le_sub_acq](await_u32.h.md#function-vatomic32_await_le_sub_acq) | Politely awaits the value pointed by a to be less than or equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_le_sub_rel](await_u32.h.md#function-vatomic32_await_le_sub_rel) | Politely awaits the value pointed by a to be less than or equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_le_sub_rlx](await_u32.h.md#function-vatomic32_await_le_sub_rlx) | Politely awaits the value pointed by a to be less than or equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_le_set](await_u32.h.md#function-vatomic32_await_le_set) | Politely awaits the value pointed by a to be less than or equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_le_set_acq](await_u32.h.md#function-vatomic32_await_le_set_acq) | Politely awaits the value pointed by a to be less than or equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_le_set_rel](await_u32.h.md#function-vatomic32_await_le_set_rel) | Politely awaits the value pointed by a to be less than or equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_le_set_rlx](await_u32.h.md#function-vatomic32_await_le_set_rlx) | Politely awaits the value pointed by a to be less than or equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_gt_add](await_u32.h.md#function-vatomic32_await_gt_add) | Politely awaits the value pointed by a to be greater than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_gt_add_acq](await_u32.h.md#function-vatomic32_await_gt_add_acq) | Politely awaits the value pointed by a to be greater than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_gt_add_rel](await_u32.h.md#function-vatomic32_await_gt_add_rel) | Politely awaits the value pointed by a to be greater than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_gt_add_rlx](await_u32.h.md#function-vatomic32_await_gt_add_rlx) | Politely awaits the value pointed by a to be greater than c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_gt_sub](await_u32.h.md#function-vatomic32_await_gt_sub) | Politely awaits the value pointed by a to be greater than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_gt_sub_acq](await_u32.h.md#function-vatomic32_await_gt_sub_acq) | Politely awaits the value pointed by a to be greater than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_gt_sub_rel](await_u32.h.md#function-vatomic32_await_gt_sub_rel) | Politely awaits the value pointed by a to be greater than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_gt_sub_rlx](await_u32.h.md#function-vatomic32_await_gt_sub_rlx) | Politely awaits the value pointed by a to be greater than c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_gt_set](await_u32.h.md#function-vatomic32_await_gt_set) | Politely awaits the value pointed by a to be greater than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_gt_set_acq](await_u32.h.md#function-vatomic32_await_gt_set_acq) | Politely awaits the value pointed by a to be greater than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_gt_set_rel](await_u32.h.md#function-vatomic32_await_gt_set_rel) | Politely awaits the value pointed by a to be greater than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_gt_set_rlx](await_u32.h.md#function-vatomic32_await_gt_set_rlx) | Politely awaits the value pointed by a to be greater than c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_ge_add](await_u32.h.md#function-vatomic32_await_ge_add) | Politely awaits the value pointed by a to be greater than or equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_ge_add_acq](await_u32.h.md#function-vatomic32_await_ge_add_acq) | Politely awaits the value pointed by a to be greater than or equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_ge_add_rel](await_u32.h.md#function-vatomic32_await_ge_add_rel) | Politely awaits the value pointed by a to be greater than or equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_ge_add_rlx](await_u32.h.md#function-vatomic32_await_ge_add_rlx) | Politely awaits the value pointed by a to be greater than or equal to c, adding v to a once the condition is satisfied.  |
| [vatomic32_await_ge_sub](await_u32.h.md#function-vatomic32_await_ge_sub) | Politely awaits the value pointed by a to be greater than or equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_ge_sub_acq](await_u32.h.md#function-vatomic32_await_ge_sub_acq) | Politely awaits the value pointed by a to be greater than or equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_ge_sub_rel](await_u32.h.md#function-vatomic32_await_ge_sub_rel) | Politely awaits the value pointed by a to be greater than or equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_ge_sub_rlx](await_u32.h.md#function-vatomic32_await_ge_sub_rlx) | Politely awaits the value pointed by a to be greater than or equal to c, subtracting v to a once the condition is satisfied.  |
| [vatomic32_await_ge_set](await_u32.h.md#function-vatomic32_await_ge_set) | Politely awaits the value pointed by a to be greater than or equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_ge_set_acq](await_u32.h.md#function-vatomic32_await_ge_set_acq) | Politely awaits the value pointed by a to be greater than or equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_ge_set_rel](await_u32.h.md#function-vatomic32_await_ge_set_rel) | Politely awaits the value pointed by a to be greater than or equal to c, setting v to a once the condition is satisfied.  |
| [vatomic32_await_ge_set_rlx](await_u32.h.md#function-vatomic32_await_ge_set_rlx) | Politely awaits the value pointed by a to be greater than or equal to c, setting v to a once the condition is satisfied.  |

##  Function `vatomic32_await_lt`

```c
static vuint32_t vatomic32_await_lt(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_lt_acq`

```c
static vuint32_t vatomic32_await_lt_acq(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: acquire 


##  Function `vatomic32_await_lt_rlx`

```c
static vuint32_t vatomic32_await_lt_rlx(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: relaxed 


##  Function `vatomic32_await_le`

```c
static vuint32_t vatomic32_await_le(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_le_acq`

```c
static vuint32_t vatomic32_await_le_acq(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: acquire 


##  Function `vatomic32_await_le_rlx`

```c
static vuint32_t vatomic32_await_le_rlx(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: relaxed 


##  Function `vatomic32_await_gt`

```c
static vuint32_t vatomic32_await_gt(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_gt_acq`

```c
static vuint32_t vatomic32_await_gt_acq(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: acquire 


##  Function `vatomic32_await_gt_rlx`

```c
static vuint32_t vatomic32_await_gt_rlx(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: relaxed 


##  Function `vatomic32_await_ge`

```c
static vuint32_t vatomic32_await_ge(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_ge_acq`

```c
static vuint32_t vatomic32_await_ge_acq(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: acquire 


##  Function `vatomic32_await_ge_rlx`

```c
static vuint32_t vatomic32_await_ge_rlx(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: relaxed 


##  Function `vatomic32_await_neq`

```c
static vuint32_t vatomic32_await_neq(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_neq_acq`

```c
static vuint32_t vatomic32_await_neq_acq(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: acquire 


##  Function `vatomic32_await_neq_rlx`

```c
static vuint32_t vatomic32_await_neq_rlx(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** value satisfying conditon

**Memory ordering**: relaxed 


##  Function `vatomic32_await_eq`

```c
static vuint32_t vatomic32_await_eq(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** last value before satisfying condition

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_eq_acq`

```c
static vuint32_t vatomic32_await_eq_acq(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** last value before satisfying condition

**Memory ordering**: acquire 


##  Function `vatomic32_await_eq_rlx`

```c
static vuint32_t vatomic32_await_eq_rlx(const vatomic32_t *a, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to v._ 


The return value is the first read value that satisfies the condition, as if read with vatomic32_read(a).



**Parameters:**

- `a`: atomic variable 
- `v`: value to compare 


**Returns:** last value before satisfying condition

**Memory ordering**: relaxed 


##  Function `vatomic32_await_eq_add`

```c
static vuint32_t vatomic32_await_eq_add(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_eq_add_acq`

```c
static vuint32_t vatomic32_await_eq_add_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_eq_add_rel`

```c
static vuint32_t vatomic32_await_eq_add_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_eq_add_rlx`

```c
static vuint32_t vatomic32_await_eq_add_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_eq_sub`

```c
static vuint32_t vatomic32_await_eq_sub(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_eq_sub_acq`

```c
static vuint32_t vatomic32_await_eq_sub_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_eq_sub_rel`

```c
static vuint32_t vatomic32_await_eq_sub_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_eq_sub_rlx`

```c
static vuint32_t vatomic32_await_eq_sub_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_eq_set`

```c
static vuint32_t vatomic32_await_eq_set(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_eq_set_acq`

```c
static vuint32_t vatomic32_await_eq_set_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_eq_set_rel`

```c
static vuint32_t vatomic32_await_eq_set_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_eq_set_rlx`

```c
static vuint32_t vatomic32_await_eq_set_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_neq_add`

```c
static vuint32_t vatomic32_await_neq_add(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_neq_add_acq`

```c
static vuint32_t vatomic32_await_neq_add_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_neq_add_rel`

```c
static vuint32_t vatomic32_await_neq_add_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_neq_add_rlx`

```c
static vuint32_t vatomic32_await_neq_add_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_neq_sub`

```c
static vuint32_t vatomic32_await_neq_sub(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_neq_sub_acq`

```c
static vuint32_t vatomic32_await_neq_sub_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_neq_sub_rel`

```c
static vuint32_t vatomic32_await_neq_sub_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_neq_sub_rlx`

```c
static vuint32_t vatomic32_await_neq_sub_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_neq_set`

```c
static vuint32_t vatomic32_await_neq_set(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_neq_set_acq`

```c
static vuint32_t vatomic32_await_neq_set_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_neq_set_rel`

```c
static vuint32_t vatomic32_await_neq_set_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_neq_set_rlx`

```c
static vuint32_t vatomic32_await_neq_set_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be different than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_lt_add`

```c
static vuint32_t vatomic32_await_lt_add(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_lt_add_acq`

```c
static vuint32_t vatomic32_await_lt_add_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_lt_add_rel`

```c
static vuint32_t vatomic32_await_lt_add_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_lt_add_rlx`

```c
static vuint32_t vatomic32_await_lt_add_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_lt_sub`

```c
static vuint32_t vatomic32_await_lt_sub(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_lt_sub_acq`

```c
static vuint32_t vatomic32_await_lt_sub_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_lt_sub_rel`

```c
static vuint32_t vatomic32_await_lt_sub_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_lt_sub_rlx`

```c
static vuint32_t vatomic32_await_lt_sub_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_lt_set`

```c
static vuint32_t vatomic32_await_lt_set(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_lt_set_acq`

```c
static vuint32_t vatomic32_await_lt_set_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_lt_set_rel`

```c
static vuint32_t vatomic32_await_lt_set_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_lt_set_rlx`

```c
static vuint32_t vatomic32_await_lt_set_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_le_add`

```c
static vuint32_t vatomic32_await_le_add(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_le_add_acq`

```c
static vuint32_t vatomic32_await_le_add_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_le_add_rel`

```c
static vuint32_t vatomic32_await_le_add_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_le_add_rlx`

```c
static vuint32_t vatomic32_await_le_add_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_le_sub`

```c
static vuint32_t vatomic32_await_le_sub(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_le_sub_acq`

```c
static vuint32_t vatomic32_await_le_sub_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_le_sub_rel`

```c
static vuint32_t vatomic32_await_le_sub_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_le_sub_rlx`

```c
static vuint32_t vatomic32_await_le_sub_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_le_set`

```c
static vuint32_t vatomic32_await_le_set(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_le_set_acq`

```c
static vuint32_t vatomic32_await_le_set_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_le_set_rel`

```c
static vuint32_t vatomic32_await_le_set_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_le_set_rlx`

```c
static vuint32_t vatomic32_await_le_set_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be less than or equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_gt_add`

```c
static vuint32_t vatomic32_await_gt_add(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_gt_add_acq`

```c
static vuint32_t vatomic32_await_gt_add_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_gt_add_rel`

```c
static vuint32_t vatomic32_await_gt_add_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_gt_add_rlx`

```c
static vuint32_t vatomic32_await_gt_add_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_gt_sub`

```c
static vuint32_t vatomic32_await_gt_sub(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_gt_sub_acq`

```c
static vuint32_t vatomic32_await_gt_sub_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_gt_sub_rel`

```c
static vuint32_t vatomic32_await_gt_sub_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_gt_sub_rlx`

```c
static vuint32_t vatomic32_await_gt_sub_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_gt_set`

```c
static vuint32_t vatomic32_await_gt_set(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_gt_set_acq`

```c
static vuint32_t vatomic32_await_gt_set_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_gt_set_rel`

```c
static vuint32_t vatomic32_await_gt_set_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_gt_set_rlx`

```c
static vuint32_t vatomic32_await_gt_set_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_ge_add`

```c
static vuint32_t vatomic32_await_ge_add(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_ge_add_acq`

```c
static vuint32_t vatomic32_await_ge_add_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_ge_add_rel`

```c
static vuint32_t vatomic32_await_ge_add_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_ge_add_rlx`

```c
static vuint32_t vatomic32_await_ge_add_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, adding v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_ge_sub`

```c
static vuint32_t vatomic32_await_ge_sub(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_ge_sub_acq`

```c
static vuint32_t vatomic32_await_ge_sub_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_ge_sub_rel`

```c
static vuint32_t vatomic32_await_ge_sub_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_ge_sub_rlx`

```c
static vuint32_t vatomic32_await_ge_sub_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, subtracting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 


##  Function `vatomic32_await_ge_set`

```c
static vuint32_t vatomic32_await_ge_set(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Function `vatomic32_await_ge_set_acq`

```c
static vuint32_t vatomic32_await_ge_set_acq(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: acquire 


##  Function `vatomic32_await_ge_set_rel`

```c
static vuint32_t vatomic32_await_ge_set_rel(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: release 


##  Function `vatomic32_await_ge_set_rlx`

```c
static vuint32_t vatomic32_await_ge_set_rlx(vatomic32_t *a, vuint32_t c, vuint32_t v)
``` 
_Politely awaits the value pointed by a to be greater than or equal to c, setting v to a once the condition is satisfied._ 




**Parameters:**

- `a`: atomic variable 
- `c`: value to compare 
- `v`: value to apply the operation 


**Returns:** old value

**Memory ordering**: relaxed 



---
