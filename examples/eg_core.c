#include <vsync/atomic.h>
#include <assert.h>
#include <stdio.h>

vatomic32_t var;
vatomicptr_t ptr;
int x;

void
foo(void)
{
    vatomic32_write(&var, 1000);
    vatomic32_add(&var, 10);
    vuint32_t val = vatomic32_read(&var);
    assert(val == 1010);
    assert(vatomic32_cmpxchg(&var, val, 0) == val);

    x = 123;
    vatomicptr_write(&ptr, &x);
    int *y = vatomicptr_read(&ptr);
    (*y)++;
    assert(*y == x);

    printf("passed\n");
}
int
main(void)
{
    foo();
}
