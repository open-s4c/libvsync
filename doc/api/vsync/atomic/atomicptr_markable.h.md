#  [vsync](../README.md) / [atomic](README.md) / atomicptr_markable.h
_C implementation of AtomicMarkableReference in Java._ 


**Precondition:** Addresses must be 4-byte-aligned in order to use vatomicptr_markable_t.

Extends `vatomicptr_t` to `vatomicptr_markable_t`. The LSB of the address is utilized as a mark.


### References:
 [AtomicMarkableReference](https://bit.ly/2FqwH8u) 

---
# Macros 

| Macro | Description |
|---|---|
| [V_ATOMICPTR_MARKABLE_IS_ALIGNED](atomicptr_markable.h.md#macro-v_atomicptr_markable_is_aligned) | Evaluates to `true`, if the given pointer `_ptr_` is 4-byte-aligned.  |

##  Macro `V_ATOMICPTR_MARKABLE_IS_ALIGNED`

```c
V_ATOMICPTR_MARKABLE_IS_ALIGNED(_ptr_)
```

 
_Evaluates to_ `true`_, if the given pointer_ `_ptr_` _is 4-byte-aligned._ 



---
# Functions 

| Function | Description |
|---|---|
| [vatomicptr_markable_cmpxchg](atomicptr_markable.h.md#function-vatomicptr_markable_cmpxchg) | Compares and exchanges the address and mark value in `a`.  |
| [vatomicptr_markable_cmpxchg_acq](atomicptr_markable.h.md#function-vatomicptr_markable_cmpxchg_acq) | Compares and exchanges the address and mark value in `a`.  |
| [vatomicptr_markable_cmpxchg_rel](atomicptr_markable.h.md#function-vatomicptr_markable_cmpxchg_rel) | Compares and exchanges the address and mark value in `a`.  |
| [vatomicptr_markable_cmpxchg_rlx](atomicptr_markable.h.md#function-vatomicptr_markable_cmpxchg_rlx) | Compares and exchanges the address and mark value in `a`.  |
| [vatomicptr_markable_attempt_mark](atomicptr_markable.h.md#function-vatomicptr_markable_attempt_mark) | Attemps to update the mark in `a`.  |
| [vatomicptr_markable_attempt_mark_acq](atomicptr_markable.h.md#function-vatomicptr_markable_attempt_mark_acq) | Attemps to update the mark in `a`.  |
| [vatomicptr_markable_attempt_mark_rel](atomicptr_markable.h.md#function-vatomicptr_markable_attempt_mark_rel) | Attemps to update the mark in `a`.  |
| [vatomicptr_markable_attempt_mark_rlx](atomicptr_markable.h.md#function-vatomicptr_markable_attempt_mark_rlx) | Attemps to update the mark in `a`.  |
| [vatomicptr_markable_set](atomicptr_markable.h.md#function-vatomicptr_markable_set) | Sets the address and mark of a to the given `v` and `m`.  |
| [vatomicptr_markable_set_rel](atomicptr_markable.h.md#function-vatomicptr_markable_set_rel) | Sets the address and mark of a to the given `v` and `m`.  |
| [vatomicptr_markable_set_rlx](atomicptr_markable.h.md#function-vatomicptr_markable_set_rlx) | Sets the address and mark of a to the given `v` and `m`.  |
| [vatomicptr_markable_get](atomicptr_markable.h.md#function-vatomicptr_markable_get) | Returns the address value of a.  |
| [vatomicptr_markable_get_acq](atomicptr_markable.h.md#function-vatomicptr_markable_get_acq) | Returns the address value of a.  |
| [vatomicptr_markable_get_rlx](atomicptr_markable.h.md#function-vatomicptr_markable_get_rlx) | Returns the address value of a.  |
| [vatomicptr_markable_get_pointer](atomicptr_markable.h.md#function-vatomicptr_markable_get_pointer) | Returns the address value of `a`.  |
| [vatomicptr_markable_get_pointer_acq](atomicptr_markable.h.md#function-vatomicptr_markable_get_pointer_acq) | Returns the address value of `a`.  |
| [vatomicptr_markable_get_pointer_rlx](atomicptr_markable.h.md#function-vatomicptr_markable_get_pointer_rlx) | Returns the address value of `a`.  |
| [vatomicptr_markable_get_mark](atomicptr_markable.h.md#function-vatomicptr_markable_get_mark) | Returns the mark value of `a`.  |
| [vatomicptr_markable_get_mark_acq](atomicptr_markable.h.md#function-vatomicptr_markable_get_mark_acq) | Returns the mark value of `a`.  |
| [vatomicptr_markable_get_mark_rlx](atomicptr_markable.h.md#function-vatomicptr_markable_get_mark_rlx) | Returns the mark value of `a`.  |

##  Function `vatomicptr_markable_cmpxchg`

```c
static vbool_t vatomicptr_markable_cmpxchg(vatomicptr_markable_t *a, void *e_v, vbool_t e_m, void *v, vbool_t m)
``` 
_Compares and exchanges the address and mark value in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `e_v`: expected address value. 
- `e_m`: expected mark value. 
- `v`: new address value to set. 
- `m`: new mark value to set. 


**Returns:** true address and mark were set. 

**Returns:** false address and mark were not updated, because either the current address value in `a` != `e_v`, or the current mark value in `a` != `e_m`. 



##  Function `vatomicptr_markable_cmpxchg_acq`

```c
static vbool_t vatomicptr_markable_cmpxchg_acq(vatomicptr_markable_t *a, void *e_v, vbool_t e_m, void *v, vbool_t m)
``` 
_Compares and exchanges the address and mark value in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `e_v`: expected address value. 
- `e_m`: expected mark value. 
- `v`: new address value to set. 
- `m`: new mark value to set. 


**Returns:** true address and mark were set. 

**Returns:** false address and mark were not updated, because either the current address value in `a` != `e_v`, or the current mark value in `a` != `e_m`. 



##  Function `vatomicptr_markable_cmpxchg_rel`

```c
static vbool_t vatomicptr_markable_cmpxchg_rel(vatomicptr_markable_t *a, void *e_v, vbool_t e_m, void *v, vbool_t m)
``` 
_Compares and exchanges the address and mark value in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `e_v`: expected address value. 
- `e_m`: expected mark value. 
- `v`: new address value to set. 
- `m`: new mark value to set. 


**Returns:** true address and mark were set. 

**Returns:** false address and mark were not updated, because either the current address value in `a` != `e_v`, or the current mark value in `a` != `e_m`. 



##  Function `vatomicptr_markable_cmpxchg_rlx`

```c
static vbool_t vatomicptr_markable_cmpxchg_rlx(vatomicptr_markable_t *a, void *e_v, vbool_t e_m, void *v, vbool_t m)
``` 
_Compares and exchanges the address and mark value in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `e_v`: expected address value. 
- `e_m`: expected mark value. 
- `v`: new address value to set. 
- `m`: new mark value to set. 


**Returns:** true address and mark were set. 

**Returns:** false address and mark were not updated, because either the current address value in `a` != `e_v`, or the current mark value in `a` != `e_m`. 



##  Function `vatomicptr_markable_attempt_mark`

```c
static vbool_t vatomicptr_markable_attempt_mark(vatomicptr_markable_t *a, void *e_v, vbool_t m)
``` 
_Attemps to update the mark in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `e_v`: expected address value. 
- `m`: new mark value. 


**Returns:** true mark was set to `m`. 

**Returns:** false mark is not set, because current address value in `a` != `e_v`. 



##  Function `vatomicptr_markable_attempt_mark_acq`

```c
static vbool_t vatomicptr_markable_attempt_mark_acq(vatomicptr_markable_t *a, void *e_v, vbool_t m)
``` 
_Attemps to update the mark in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `e_v`: expected address value. 
- `m`: new mark value. 


**Returns:** true mark was set to `m`. 

**Returns:** false mark is not set, because current address value in `a` != `e_v`. 



##  Function `vatomicptr_markable_attempt_mark_rel`

```c
static vbool_t vatomicptr_markable_attempt_mark_rel(vatomicptr_markable_t *a, void *e_v, vbool_t m)
``` 
_Attemps to update the mark in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `e_v`: expected address value. 
- `m`: new mark value. 


**Returns:** true mark was set to `m`. 

**Returns:** false mark is not set, because current address value in `a` != `e_v`. 



##  Function `vatomicptr_markable_attempt_mark_rlx`

```c
static vbool_t vatomicptr_markable_attempt_mark_rlx(vatomicptr_markable_t *a, void *e_v, vbool_t m)
``` 
_Attemps to update the mark in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `e_v`: expected address value. 
- `m`: new mark value. 


**Returns:** true mark was set to `m`. 

**Returns:** false mark is not set, because current address value in `a` != `e_v`. 



##  Function `vatomicptr_markable_set`

```c
static void vatomicptr_markable_set(vatomicptr_markable_t *a, void *v, vbool_t m)
``` 
_Sets the address and mark of a to the given_ `v` _and_ `m`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `v`: address value to set. 
- `m`: mark value to set. 




##  Function `vatomicptr_markable_set_rel`

```c
static void vatomicptr_markable_set_rel(vatomicptr_markable_t *a, void *v, vbool_t m)
``` 
_Sets the address and mark of a to the given_ `v` _and_ `m`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `v`: address value to set. 
- `m`: mark value to set. 




##  Function `vatomicptr_markable_set_rlx`

```c
static void vatomicptr_markable_set_rlx(vatomicptr_markable_t *a, void *v, vbool_t m)
``` 
_Sets the address and mark of a to the given_ `v` _and_ `m`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `v`: address value to set. 
- `m`: mark value to set. 




##  Function `vatomicptr_markable_get`

```c
static void* vatomicptr_markable_get(vatomicptr_markable_t *a, vbool_t *out_m)
``` 
_Returns the address value of a._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `out_m`: output param of type vbool_t. The mark value in `a`. 


**Returns:** void* address value in `a`. 



##  Function `vatomicptr_markable_get_acq`

```c
static void* vatomicptr_markable_get_acq(vatomicptr_markable_t *a, vbool_t *out_m)
``` 
_Returns the address value of a._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `out_m`: output param of type vbool_t. The mark value in `a`. 


**Returns:** void* address value in `a`. 



##  Function `vatomicptr_markable_get_rlx`

```c
static void* vatomicptr_markable_get_rlx(vatomicptr_markable_t *a, vbool_t *out_m)
``` 
_Returns the address value of a._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 
- `out_m`: output param of type vbool_t. The mark value in `a`. 


**Returns:** void* address value in `a`. 



##  Function `vatomicptr_markable_get_pointer`

```c
static void* vatomicptr_markable_get_pointer(vatomicptr_markable_t *a)
``` 
_Returns the address value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 


**Returns:** void* stored address in `a`. 



##  Function `vatomicptr_markable_get_pointer_acq`

```c
static void* vatomicptr_markable_get_pointer_acq(vatomicptr_markable_t *a)
``` 
_Returns the address value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 


**Returns:** void* stored address in `a`. 



##  Function `vatomicptr_markable_get_pointer_rlx`

```c
static void* vatomicptr_markable_get_pointer_rlx(vatomicptr_markable_t *a)
``` 
_Returns the address value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 


**Returns:** void* stored address in `a`. 



##  Function `vatomicptr_markable_get_mark`

```c
static vbool_t vatomicptr_markable_get_mark(vatomicptr_markable_t *a)
``` 
_Returns the mark value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 


**Returns:** true if `a` is marked. 

**Returns:** true if `a` is not marked. 



##  Function `vatomicptr_markable_get_mark_acq`

```c
static vbool_t vatomicptr_markable_get_mark_acq(vatomicptr_markable_t *a)
``` 
_Returns the mark value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 


**Returns:** true if `a` is marked. 

**Returns:** true if `a` is not marked. 



##  Function `vatomicptr_markable_get_mark_rlx`

```c
static vbool_t vatomicptr_markable_get_mark_rlx(vatomicptr_markable_t *a)
``` 
_Returns the mark value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_markable_t object. 


**Returns:** true if `a` is marked. 

**Returns:** true if `a` is not marked. 




---
