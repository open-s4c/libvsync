#  [vsync](../../README.md) / [thread](../README.md) / [mutex](README.md) / tristate.h
_3-state mutex_ 


### Example:



```c
   #include <vsync/thread/mutex/tristate.h>
vmutex_t mutex;
void run(void) {
   vmutex_acquire(&mutex);
   ...
   vmutex_release(&mutex);
}
```




### References:
 Ulrich Drepper  --  Futexes Are Tricky. 2004 


---
