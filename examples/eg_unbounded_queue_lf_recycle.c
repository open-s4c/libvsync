#include <vsync/queue/unbounded_queue_lf_recycle.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#define IT 100
#define N  12

/* this is just a proof of concept and not an optimal. Each thread shall have a
 * its own pool */
vqueue_ub_node_t *local_pools[N] = {0};

typedef struct data_s {
    vsize_t id;
} data_t;

vqueue_ub_t g_queue;

void
destroy_pools(void)
{
    vqueue_ub_node_t *curr = NULL;
    vqueue_ub_node_t *succ = NULL;

    for (vsize_t i = 0; i < N; i++) {
        curr = local_pools[i];
        while (curr) {
            succ = vatomicptr_stamped_get_pointer(&curr->next);
            free(curr);
            curr = succ;
        }
    }
}

void
destroy_cb(vqueue_ub_node_t *qnode, void *args)
{
    /* Nodes can be freed only after queue destruction */
    free(qnode);
    (void)args;
}

void
retire_cb(vqueue_ub_node_t *qnode, void *args)
{
    vsize_t tid = (vsize_t)args;
    /* @note: Users should not rely on the qnode internal fields, they should
     * have an independent pool. The following is just a proof of concept. */
    /* retired nodes are added to the local pool */
    vuint8_t current_stamp = 0;
    (void)vatomicptr_stamped_get(&qnode->next, &current_stamp);

    /* connect the retired node to the local pool */
    vatomicptr_stamped_set(&qnode->next, local_pools[tid], current_stamp + 1);
    local_pools[tid] = qnode;
    (void)args;
}

vqueue_ub_node_t *
alloc_qnode(vsize_t tid)
{
    vqueue_ub_node_t *qnode = NULL;

    if (local_pools[tid]) {
        qnode = local_pools[tid];
        /* if the local pool is not empty use a node from there */
        local_pools[tid] =
            vatomicptr_stamped_get_pointer(&local_pools[tid]->next);
        printf("Recycle allocation\n");
    } else {
        /* otherwise allocated an aligned node */
        qnode =
            memalign(RECYCLE_QUEUE_NODE_ALIGNMENT, sizeof(vqueue_ub_node_t));
        printf("New allocation\n");
    }
    assert(qnode);
    return qnode;
}

void *
run(void *args)
{
    data_t *data = NULL;
    vsize_t tid  = (vsize_t)args;

    for (vsize_t i = 0; i < IT; i++) {
        data = vqueue_ub_deq(&g_queue, retire_cb, (void *)tid);
        if (data == NULL) {
            data                    = malloc(sizeof(data_t));
            data->id                = i;
            vqueue_ub_node_t *qnode = alloc_qnode(tid);
            printf("[T%zu] enq %zu\n", tid, data->id);
            vqueue_ub_enq(&g_queue, qnode, data);
        } else {
            printf("[T%zu] deq %zu\n", tid, data->id);
            free(data);
        }
    }

    return NULL;
}

int
main(void)
{
    pthread_t threads[N];
    data_t *data = NULL;

    vqueue_ub_init(&g_queue);

    for (vsize_t i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, run, (void *)i);
    }

    for (vsize_t i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    /* dequeue all remaining nodes, to be able to destroy data */
    while (data = vqueue_ub_deq(&g_queue, destroy_cb, NULL), data) {
        free(data);
    }
    /* destroy the queue to destroy the remaining sentinel */
    vqueue_ub_destroy(&g_queue, destroy_cb, NULL);

    /* destroy local pools */
    destroy_pools();
    return 0;
}
