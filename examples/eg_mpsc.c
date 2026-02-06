#include <vsync/queue/mpsc.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

vmpsc_t g_queue;

#define NUM_PRODUCERS  3U
#define NUM_PROD_ITEMS 10U
#define NUM_CONS_ITEMS (NUM_PRODUCERS * NUM_PROD_ITEMS)

typedef struct data_s {
    vuint32_t x;
    vuint32_t y;
} data_t;

void
destroy_cb(vmpsc_node_t *node, void *args)
{
    free(node->data);
    free(node);
    V_UNUSED(args);
}

void *
consume(void *args)
{
    vmpsc_node_t *n = NULL;
    data_t *d       = NULL;
    for (vuint32_t i = 0; i < NUM_CONS_ITEMS; i++) {
        while (d = vmpsc_deq(&g_queue, &n), d == NULL) {
            /* busy wait till an item is consumed. Repeat on empty! */
        }
        ASSERT(d);
        ASSERT(d->x == d->y);
        printf("consumed item: {%u, %u}\n", d->x, d->y);
        free(d);
        /* n can be NULL even if d is not. */
        free(n);
    }
    V_UNUSED(args);
    return NULL;
}

void *
produce(void *args)
{
    vuint32_t tid = (vuint32_t)(vuintptr_t)args;
    for (vuint32_t i = 0; i < NUM_PROD_ITEMS; i++) {
        data_t *d = malloc(sizeof(*d));
        ASSERT(d);
        d->x = d->y     = (tid * NUM_PRODUCERS) + i;
        vmpsc_node_t *n = malloc(sizeof(*n));
        ASSERT(n);
        vmpsc_enq(&g_queue, n, d);
    }
    V_UNUSED(args);
    return NULL;
}

int
main(void)
{
    vmpsc_init(&g_queue);
    pthread_t consumer;
    pthread_t producer[NUM_PRODUCERS];

    pthread_create(&consumer, NULL, consume, NULL);
    for (vsize_t i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producer[i], NULL, produce, (void *)(vuintptr_t)i);
    }
    for (vsize_t i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producer[i], NULL);
    }
    pthread_join(consumer, NULL);
    vmpsc_destroy(&g_queue, destroy_cb, NULL);
    return 0;
}
