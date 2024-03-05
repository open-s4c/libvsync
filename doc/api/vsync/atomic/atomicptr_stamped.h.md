#  [vsync](../README.md) / [atomic](README.md) / atomicptr_stamped.h
_C implementation of AtomicstampedReference in Java._ 


**Precondition:** Addresses must be 128-byte aligned in order to use vatomicptr_stamped_t.

Extends `vatomicptr_t` to `vatomicptr_stamped_t`. The 7 LSBs of the address are used as a stamp.


### References:
 [AtomicstampedReference](https://bit.ly/2TWl11a) 

---
# Macros 

| Macro | Description |
|---|---|
| [V_ATOMICPTR_STAMPED_IS_ALIGNED](atomicptr_stamped.h.md#macro-v_atomicptr_stamped_is_aligned) | Evaluates to `true`, if the given pointer `_ptr_` is 128-byte-aligned.  |

##  Macro `V_ATOMICPTR_STAMPED_IS_ALIGNED`

```c
V_ATOMICPTR_STAMPED_IS_ALIGNED(_ptr_)
```

 
_Evaluates to_ `true`_, if the given pointer_ `_ptr_` _is 128-byte-aligned._ 



---
# Functions 

| Function | Description |
|---|---|
| [vatomicptr_stamped_cmpxchg](atomicptr_stamped.h.md#function-vatomicptr_stamped_cmpxchg) | Compares and exchanges the address and stamp value in `a`.  |
| [vatomicptr_stamped_cmpxchg_acq](atomicptr_stamped.h.md#function-vatomicptr_stamped_cmpxchg_acq) | Compares and exchanges the address and stamp value in `a`.  |
| [vatomicptr_stamped_cmpxchg_rel](atomicptr_stamped.h.md#function-vatomicptr_stamped_cmpxchg_rel) | Compares and exchanges the address and stamp value in `a`.  |
| [vatomicptr_stamped_cmpxchg_rlx](atomicptr_stamped.h.md#function-vatomicptr_stamped_cmpxchg_rlx) | Compares and exchanges the address and stamp value in `a`.  |
| [vatomicptr_stamped_attempt_stamp](atomicptr_stamped.h.md#function-vatomicptr_stamped_attempt_stamp) | Attemps to update the stamp in `a`.  |
| [vatomicptr_stamped_attempt_stamp_acq](atomicptr_stamped.h.md#function-vatomicptr_stamped_attempt_stamp_acq) | Attemps to update the stamp in `a`.  |
| [vatomicptr_stamped_attempt_stamp_rel](atomicptr_stamped.h.md#function-vatomicptr_stamped_attempt_stamp_rel) | Attemps to update the stamp in `a`.  |
| [vatomicptr_stamped_attempt_stamp_rlx](atomicptr_stamped.h.md#function-vatomicptr_stamped_attempt_stamp_rlx) | Attemps to update the stamp in `a`.  |
| [vatomicptr_stamped_set](atomicptr_stamped.h.md#function-vatomicptr_stamped_set) | Sets the address and stamp of a to the given `v` and `stamp`.  |
| [vatomicptr_stamped_set_rel](atomicptr_stamped.h.md#function-vatomicptr_stamped_set_rel) | Sets the address and stamp of a to the given `v` and `stamp`.  |
| [vatomicptr_stamped_set_rlx](atomicptr_stamped.h.md#function-vatomicptr_stamped_set_rlx) | Sets the address and stamp of a to the given `v` and `stamp`.  |
| [vatomicptr_stamped_get](atomicptr_stamped.h.md#function-vatomicptr_stamped_get) | Returns the address value of a.  |
| [vatomicptr_stamped_get_acq](atomicptr_stamped.h.md#function-vatomicptr_stamped_get_acq) | Returns the address value of a.  |
| [vatomicptr_stamped_get_rlx](atomicptr_stamped.h.md#function-vatomicptr_stamped_get_rlx) | Returns the address value of a.  |
| [vatomicptr_stamped_get_pointer](atomicptr_stamped.h.md#function-vatomicptr_stamped_get_pointer) | Returns the address value of `a`.  |
| [vatomicptr_stamped_get_pointer_acq](atomicptr_stamped.h.md#function-vatomicptr_stamped_get_pointer_acq) | Returns the address value of `a`.  |
| [vatomicptr_stamped_get_pointer_rlx](atomicptr_stamped.h.md#function-vatomicptr_stamped_get_pointer_rlx) | Returns the address value of `a`.  |
| [vatomicptr_stamped_get_stamp](atomicptr_stamped.h.md#function-vatomicptr_stamped_get_stamp) | Returns the stamp value of `a`.  |
| [vatomicptr_stamped_get_stamp_acq](atomicptr_stamped.h.md#function-vatomicptr_stamped_get_stamp_acq) | Returns the stamp value of `a`.  |
| [vatomicptr_stamped_get_stamp_rlx](atomicptr_stamped.h.md#function-vatomicptr_stamped_get_stamp_rlx) | Returns the stamp value of `a`.  |

##  Function `vatomicptr_stamped_cmpxchg`

```c
static vbool_t vatomicptr_stamped_cmpxchg(vatomicptr_stamped_t *a, void *e_v, vuint8_t e_stamp, void *v, vuint8_t stamp)
``` 
_Compares and exchanges the address and stamp value in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `e_v`: expected address value. 
- `e_stamp`: expected stamp value. 
- `v`: new address value to set. 
- `stamp`: new stamp value to set. 


**Returns:** true address and stamp were set. 

**Returns:** false address and stamp were not updated, because either the current address value in `a` != `e_v`, or the current stamp value in `a` != `e_stamp`. 



##  Function `vatomicptr_stamped_cmpxchg_acq`

```c
static vbool_t vatomicptr_stamped_cmpxchg_acq(vatomicptr_stamped_t *a, void *e_v, vuint8_t e_stamp, void *v, vuint8_t stamp)
``` 
_Compares and exchanges the address and stamp value in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `e_v`: expected address value. 
- `e_stamp`: expected stamp value. 
- `v`: new address value to set. 
- `stamp`: new stamp value to set. 


**Returns:** true address and stamp were set. 

**Returns:** false address and stamp were not updated, because either the current address value in `a` != `e_v`, or the current stamp value in `a` != `e_stamp`. 



##  Function `vatomicptr_stamped_cmpxchg_rel`

```c
static vbool_t vatomicptr_stamped_cmpxchg_rel(vatomicptr_stamped_t *a, void *e_v, vuint8_t e_stamp, void *v, vuint8_t stamp)
``` 
_Compares and exchanges the address and stamp value in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `e_v`: expected address value. 
- `e_stamp`: expected stamp value. 
- `v`: new address value to set. 
- `stamp`: new stamp value to set. 


**Returns:** true address and stamp were set. 

**Returns:** false address and stamp were not updated, because either the current address value in `a` != `e_v`, or the current stamp value in `a` != `e_stamp`. 



##  Function `vatomicptr_stamped_cmpxchg_rlx`

```c
static vbool_t vatomicptr_stamped_cmpxchg_rlx(vatomicptr_stamped_t *a, void *e_v, vuint8_t e_stamp, void *v, vuint8_t stamp)
``` 
_Compares and exchanges the address and stamp value in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `e_v`: expected address value. 
- `e_stamp`: expected stamp value. 
- `v`: new address value to set. 
- `stamp`: new stamp value to set. 


**Returns:** true address and stamp were set. 

**Returns:** false address and stamp were not updated, because either the current address value in `a` != `e_v`, or the current stamp value in `a` != `e_stamp`. 



##  Function `vatomicptr_stamped_attempt_stamp`

```c
static vbool_t vatomicptr_stamped_attempt_stamp(vatomicptr_stamped_t *a, void *e_v, vuint8_t stamp)
``` 
_Attemps to update the stamp in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `e_v`: expected address value. 
- `stamp`: new stamp value. 


**Returns:** true stamp was set to `stamp`. 

**Returns:** false stamp is not set, because current address value in `a` != `e_v`. 



##  Function `vatomicptr_stamped_attempt_stamp_acq`

```c
static vbool_t vatomicptr_stamped_attempt_stamp_acq(vatomicptr_stamped_t *a, void *e_v, vuint8_t stamp)
``` 
_Attemps to update the stamp in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `e_v`: expected address value. 
- `stamp`: new stamp value. 


**Returns:** true stamp was set to `stamp`. 

**Returns:** false stamp is not set, because current address value in `a` != `e_v`. 



##  Function `vatomicptr_stamped_attempt_stamp_rel`

```c
static vbool_t vatomicptr_stamped_attempt_stamp_rel(vatomicptr_stamped_t *a, void *e_v, vuint8_t stamp)
``` 
_Attemps to update the stamp in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `e_v`: expected address value. 
- `stamp`: new stamp value. 


**Returns:** true stamp was set to `stamp`. 

**Returns:** false stamp is not set, because current address value in `a` != `e_v`. 



##  Function `vatomicptr_stamped_attempt_stamp_rlx`

```c
static vbool_t vatomicptr_stamped_attempt_stamp_rlx(vatomicptr_stamped_t *a, void *e_v, vuint8_t stamp)
``` 
_Attemps to update the stamp in_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `e_v`: expected address value. 
- `stamp`: new stamp value. 


**Returns:** true stamp was set to `stamp`. 

**Returns:** false stamp is not set, because current address value in `a` != `e_v`. 



##  Function `vatomicptr_stamped_set`

```c
static void vatomicptr_stamped_set(vatomicptr_stamped_t *a, void *v, vuint8_t stamp)
``` 
_Sets the address and stamp of a to the given_ `v` _and_ `stamp`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `v`: address value to set. 
- `stamp`: stamp value to set. 




##  Function `vatomicptr_stamped_set_rel`

```c
static void vatomicptr_stamped_set_rel(vatomicptr_stamped_t *a, void *v, vuint8_t stamp)
``` 
_Sets the address and stamp of a to the given_ `v` _and_ `stamp`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `v`: address value to set. 
- `stamp`: stamp value to set. 




##  Function `vatomicptr_stamped_set_rlx`

```c
static void vatomicptr_stamped_set_rlx(vatomicptr_stamped_t *a, void *v, vuint8_t stamp)
``` 
_Sets the address and stamp of a to the given_ `v` _and_ `stamp`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `v`: address value to set. 
- `stamp`: stamp value to set. 




##  Function `vatomicptr_stamped_get`

```c
static void* vatomicptr_stamped_get(vatomicptr_stamped_t *a, vuint8_t *out_stamp)
``` 
_Returns the address value of a._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `out_stamp`: output param of type vuint8_t. The stamp value in `a`. 


**Returns:** void* address value in `a`. 



##  Function `vatomicptr_stamped_get_acq`

```c
static void* vatomicptr_stamped_get_acq(vatomicptr_stamped_t *a, vuint8_t *out_stamp)
``` 
_Returns the address value of a._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `out_stamp`: output param of type vuint8_t. The stamp value in `a`. 


**Returns:** void* address value in `a`. 



##  Function `vatomicptr_stamped_get_rlx`

```c
static void* vatomicptr_stamped_get_rlx(vatomicptr_stamped_t *a, vuint8_t *out_stamp)
``` 
_Returns the address value of a._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 
- `out_stamp`: output param of type vuint8_t. The stamp value in `a`. 


**Returns:** void* address value in `a`. 



##  Function `vatomicptr_stamped_get_pointer`

```c
static void* vatomicptr_stamped_get_pointer(vatomicptr_stamped_t *a)
``` 
_Returns the address value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 


**Returns:** void* stored address in `a`. 



##  Function `vatomicptr_stamped_get_pointer_acq`

```c
static void* vatomicptr_stamped_get_pointer_acq(vatomicptr_stamped_t *a)
``` 
_Returns the address value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 


**Returns:** void* stored address in `a`. 



##  Function `vatomicptr_stamped_get_pointer_rlx`

```c
static void* vatomicptr_stamped_get_pointer_rlx(vatomicptr_stamped_t *a)
``` 
_Returns the address value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 


**Returns:** void* stored address in `a`. 



##  Function `vatomicptr_stamped_get_stamp`

```c
static vuint8_t vatomicptr_stamped_get_stamp(vatomicptr_stamped_t *a)
``` 
_Returns the stamp value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 


**Returns:** stamp value in `a`. 



##  Function `vatomicptr_stamped_get_stamp_acq`

```c
static vuint8_t vatomicptr_stamped_get_stamp_acq(vatomicptr_stamped_t *a)
``` 
_Returns the stamp value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 


**Returns:** stamp value in `a`. 



##  Function `vatomicptr_stamped_get_stamp_rlx`

```c
static vuint8_t vatomicptr_stamped_get_stamp_rlx(vatomicptr_stamped_t *a)
``` 
_Returns the stamp value of_ `a`_._ 




**Parameters:**

- `a`: address of vatomicptr_stamped_t object. 


**Returns:** stamp value in `a`. 




---
