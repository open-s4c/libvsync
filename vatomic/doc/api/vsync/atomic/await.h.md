#  [vsync](../README.md) / [atomic](README.md) / await.h
_Atomic await functions._ 

Await functions are used to politely await a condition on an atomic variable. Moreover, some await functions also perform write to memroy at the end of the spinning. Functions of the form `await_COND` only await a condition and functions of the form `await_COND_OP` await and perform a write operation.

### Supported conditions:



| COND   |Condition   |Atomic types    |
| --- | --- | --- |
| lt   |(*a < v)   |vatomic32_t, vatomic64_t    |
| le   |(*a <= v)   |vatomic32_t, vatomic64_t    |
| gt   |(*a > v)   |vatomic32_t, vatomic64_t    |
| ge   |(*a >= v)   |vatomic32_t, vatomic64_t    |
| eq   |(*a == v)   |vatomic32_t, vatomic64_t, vatomicptr_t    |
| neq   |(*a != v)   |vatomic32_t, vatomic64_t, vatomicptr_t   |



The following example waits for the pointer me->next to be different than NULL. The variable next contains the value that satisfied the condition. The operation has an acquire barrier.



```c
node_t *next = vatomicptr_await_neq_acq(me->next, NULL);
```



### Supported operations:



| OP   |Operation   |Atomic types    |
| --- | --- | --- |
| add   |*a += v   |vatomic32_t, vatomic64_t    |
| sub   |*a -= v   |vatomic32_t, vatomic64_t    |
| set   |*a = v   |vatomic32_t, vatomic64_t, vatomicptr_t   |



The following example waits for the pointer me->next to be equal to pred. Once the condition is met, write NULL in me->next. The variable next contains the value that satisfied the condition. The operation has a release barrier.



```c
node_t *next = vatomicptr_await_eq_set_rel(me->next, pred, NULL);
```



### Return value

The return value is (typically) the first read value that satisfies the condition. Exception are functions with `eq` condition. Those return the last previously read value different than the expected value (if any). This twist can be used to identify whether the operation had to spin at all.

### Detailed documentation:

The declaration and documentation of all await operations is split into files based on the atomic type operated by the function as follows:



| File   |Type    |
| --- | --- |
| [await_u32.h](await_u32.h.md)   |vatomic32_t and vuint32_t    |
| [await_u64.h](await_u64.h.md)   |vatomic64_t and vuint64_t    |
| [await_ptr.h](await_ptr.h.md)   |vatomicptr_t and void *   |




---
