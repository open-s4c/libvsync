#define TREESET_LOCK_RW
#include <vsync/map/treeset_bst_coarse.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N       4
#define MIN_KEY 0
#define MAX_KEY 3

typedef vuintptr_t value_t;

treeset_t tree;

void *
run(void *args)
{
    vsize_t tid = (vsize_t)args;

    for (treeset_key_t key = MIN_KEY; key <= MAX_KEY; key++) {
        value_t value = tid;
        value_t old_value;

        // insert
        vbool_t res =
            treeset_add(&tree, key, (void *)value, (void *)&old_value);

        if (res) {
            // insert succeeded
            printf("[%lu] key %lu inserted\n", tid, key);
        } else {
            printf(
                "[%lu] key %lu not inserted, already in tree with value %lu\n",
                tid, key, old_value);
        }

        // search
        res = treeset_contains(&tree, key, (void *)&old_value);

        if (res) {
            // search successful
            printf("[%lu] key %lu in tree with value %lu\n", tid, key,
                   old_value);
        } else {
            printf("[%lu] key %lu not in tree\n", tid, key);
        }

        // remove
        res = treeset_remove(&tree, key, (void *)&old_value);

        if (res) {
            // remove succeeded
            printf("[%lu] key %lu removed, old value was %lu\n", tid, key,
                   old_value);
        } else {
            printf("[%lu] key %lu not removed\n", tid, key);
        }
    }

    return NULL;
}

void *
malloc_cb(vsize_t sz, void *arg)
{
    (void)arg;
    return malloc(sz);
}

void
free_cb(void *ptr, void *arg)
{
    (void)arg;
    free(ptr);
}

void
free_visitor(treeset_key_t key, void *value, void *arg)
{
    (void)key;
    (void)arg;
    free((value_t *)value);
}

int
main(void)
{
    pthread_t threads[N];

    vmem_lib_t mem_lib = {.free_fun   = free_cb,
                          .malloc_fun = malloc_cb,
                          .arg        = NULL};

    treeset_init(&tree, mem_lib);

    for (vsize_t i = 0; i < N; ++i) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    treeset_visit(&tree, free_visitor, NULL);
    treeset_destroy(&tree);

    return 0;
}
