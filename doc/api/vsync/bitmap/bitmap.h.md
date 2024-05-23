#  [vsync](../README.md) / [bitmap](README.md) / bitmap.h
_A bitmap implementation._ 

Bitmap with basic functionality.


### Example:



```c

#include <vsync/bitmap/bitmap.h>
#include <vsync/common/assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define WRITER_COUNT 11U
#define N            (WRITER_COUNT + 1)
#define BIT_COUNT    (VUINT64_WIDTH * WRITER_COUNT)

vbitmap_t *g_bitmap;

void
reader(void)
{
    vsize_t idx          = 0;
    vbitmap_iter_t iter  = {0};
    vsize_t signal_count = 0;
    vsize_t expected     = BIT_COUNT - WRITER_COUNT;

    while (signal_count < expected) {
        signal_count = 0;
        vbitmap_iter_init(g_bitmap, &iter);

        while (vbitmap_iter_next(&iter, &idx)) {
            signal_count++;
        }
        printf("%zu bits were raised, expected %zu\n", signal_count, expected);
    }
}

void
writer(vsize_t tid)
{
    vbool_t set = false;

    // each writer sets 64 bits
    vsize_t from = tid * VUINT64_WIDTH;
    vsize_t to   = from + VUINT64_WIDTH - 1;

    vbitmap_set_range(g_bitmap, from, to);

    set = vbitmap_get(g_bitmap, from);
    ASSERT(set == true);

    vbitmap_clr_bit(g_bitmap, from);
    set = vbitmap_get(g_bitmap, from);
    ASSERT(set == false);

    vbitmap_set_bit(g_bitmap, from);
    set = vbitmap_get(g_bitmap, from);
    ASSERT(set == true);
}

void *
run(void *args)
{
    vsize_t tid = (vsize_t)(vuintptr_t)args;
    if (tid == WRITER_COUNT) {
        reader();
    } else {
        writer(tid);
    }
    return NULL;
}

int
main(void)
{
    pthread_t threads[N];

    vsize_t size = vbitmap_size(BIT_COUNT);
    g_bitmap     = malloc(size);
    vbitmap_init(g_bitmap, BIT_COUNT, false);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    free(g_bitmap);

    return 0;
}
```




### References:

The interface is inspired by and similar to [ck_bitmap](https://github.com/concurrencykit/ck/blob/master/include/ck_bitmap.h) 

---
# Functions 

| Function | Description |
|---|---|
| [vbitmap_size](bitmap.h.md#function-vbitmap_size) | Calculates the size of vbitmap_t object based on the given number of bits.  |
| [vbitmap_init](bitmap.h.md#function-vbitmap_init) | Initializes the given `bitmap` object.  |
| [vbitmap_clear](bitmap.h.md#function-vbitmap_clear) | Clears all bits in the given bitmap.  |
| [vbitmap_get](bitmap.h.md#function-vbitmap_get) | Returns the value of the bit at index `bit_idx`.  |
| [vbitmap_set_bit](bitmap.h.md#function-vbitmap_set_bit) | Sets the bit at index `bit_idx`.  |
| [vbitmap_set_range](bitmap.h.md#function-vbitmap_set_range) | Sets all bits with indexes that are in range `[from:to]`.  |
| [vbitmap_clr_range](bitmap.h.md#function-vbitmap_clr_range) | Clears all bits with indexes that are in range `[from:to]`.  |
| [vbitmap_clr_bit](bitmap.h.md#function-vbitmap_clr_bit) | Clears the bit at index `bit_idx`.  |
| [vbitmap_iter_init](bitmap.h.md#function-vbitmap_iter_init) | Initializes the given vbitmap_iter_t object.  |
| [vbitmap_iter_next](bitmap.h.md#function-vbitmap_iter_next) | Iterates to the next set bit.  |

##  Function `vbitmap_size`

```c
static vsize_t vbitmap_size(vsize_t bit_count)
``` 
_Calculates the size of vbitmap_t object based on the given number of bits._ 




**Parameters:**

- `bit_count`: number of bits. 


**Returns:** vsize_t size of vbitmap_t object needed to accommodate `bit_count` of bits. 



##  Function `vbitmap_init`

```c
static void vbitmap_init(vbitmap_t *bitmap, vsize_t bit_count, vbool_t set)
``` 
_Initializes the given_ `bitmap` _object._ 


All bits are unset initially.



**Parameters:**

- `bitmap`: address of vbitmap_t object. 
- `bit_count`: number of bits. 
- `set`: true if all bits should be set, false otherwise. 




##  Function `vbitmap_clear`

```c
static void vbitmap_clear(vbitmap_t *bitmap)
``` 
_Clears all bits in the given bitmap._ 




**Parameters:**

- `bitmap`: address of vbitmap_t object. 




##  Function `vbitmap_get`

```c
static vbool_t vbitmap_get(vbitmap_t *bitmap, vsize_t bit_idx)
``` 
_Returns the value of the bit at index_ `bit_idx`_._ 




**Parameters:**

- `bitmap`: address of vbitmap_t object. 
- `bit_idx`: index of the bit to set. 


**Returns:** true the bit is set. 

**Returns:** false the bit is unset. 



##  Function `vbitmap_set_bit`

```c
static void vbitmap_set_bit(vbitmap_t *bitmap, vsize_t bit_idx)
``` 
_Sets the bit at index_ `bit_idx`_._ 




**Parameters:**

- `bitmap`: address of vbitmap_t object. 
- `bit_idx`: index of the bit to set. 




##  Function `vbitmap_set_range`

```c
static void vbitmap_set_range(vbitmap_t *bitmap, vsize_t from, vsize_t to)
``` 
_Sets all bits with indexes that are in range_ `[from:to]`_._ 


> **Note:** the range is inclusive both bits at index `from` and index `to` will be set.



**Parameters:**

- `bitmap`: address of vbitmap_t object. 
- `from`: index of the LSB in the range. 
- `to`: index of the MSB in the range.


> **Note:** This function is not atomic. Bits inside the range might not be raised at once. Raising bits occur in multiple steps. 


##  Function `vbitmap_clr_range`

```c
static void vbitmap_clr_range(vbitmap_t *bitmap, vsize_t from, vsize_t to)
``` 
_Clears all bits with indexes that are in range_ `[from:to]`_._ 


> **Note:** the range is inclusive both bits at index `from` and index `to` will be set.



**Parameters:**

- `bitmap`: address of vbitmap_t object. 
- `from`: index of the LSB in the range. 
- `to`: index of the MSB in the range.


> **Note:** This function is not atomic. Bits inside the range might not be cleared at once. Clearing bits occur in multiple steps. 


##  Function `vbitmap_clr_bit`

```c
static void vbitmap_clr_bit(vbitmap_t *bitmap, vsize_t bit_idx)
``` 
_Clears the bit at index_ `bit_idx`_._ 




**Parameters:**

- `bitmap`: address of vbitmap_t object. 
- `bit_idx`: index of the bit to unset. 




##  Function `vbitmap_iter_init`

```c
static void vbitmap_iter_init(vbitmap_t *bitmap, vbitmap_iter_t *iter)
``` 
_Initializes the given vbitmap_iter_t object._ 




**Parameters:**

- `bitmap`: address of vbitmap_t to iterate over. 
- `iter`: address of vbitmap_iter_t object. 




##  Function `vbitmap_iter_next`

```c
static vbool_t vbitmap_iter_next(vbitmap_iter_t *iter, vsize_t *out_bit_idx)
``` 
_Iterates to the next set bit._ 




**Parameters:**

- `iter`: address of vbitmap_iter_t object. 
- `out_bit_idx`: index of the next set bit. 


**Returns:** true next set bit is found. 

**Returns:** false no more set bits are found.

**Precondition:** call `vbitmap_iter_init` to initialize `iter` before first use. 




---
