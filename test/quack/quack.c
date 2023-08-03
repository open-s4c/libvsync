/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <pthread.h>
#include <vsync/common/assert.h>
#include <stdlib.h>
#include <vsync/stack/quack.h>
#include <test/thread_launcher.h>

#define NTHREADS	   2U
#define NODES		   3U
#define PRODUCER_COUNT (NTHREADS / 2)

quack_t quack;

typedef struct {
	quack_node_t node;
	vuint32_t v;
} node_t;

node_t g_nodes[NODES];
vatomic32_t g_cnt_in;
vatomic32_t g_cnt_out;

static void *
producer(void)
{
	vuint32_t i = 0;
	while ((i = vatomic32_get_inc_rlx(&g_cnt_in)) < NODES) {
		node_t *n = &g_nodes[i];
		n->v	  = i;
		quack_push(&quack, &n->node);
	}
	return 0;
}

static void *
consumer(void)
{
	vuint32_t count = 0;
	await_do {
		quack_node_t *n = quack_reverse(quack_popall(&quack));
		for (; n != NULL; n = n->next) {
			node_t *nn = container_of(n, node_t, node);
			nn->v	   = VUINT32_MAX;
			vatomic32_inc_rlx(&g_cnt_out);
		}
	}

	while_await(vatomic32_read_rlx(&g_cnt_out) < NODES);

	count = vatomic32_read_rlx(&g_cnt_out);
	ASSERT(count == NODES);
	V_UNUSED(count);
	return 0;
}

void *
run(void *arg)
{
	vsize_t tid = (vsize_t)(vuintptr_t)arg;
	if (tid < PRODUCER_COUNT) {
		return producer();
	} else {
		return consumer();
	}
}

int
main(void)
{
	vuint32_t count = 0;
	launch_threads(NTHREADS, run);

	for (vsize_t i = 0; i < NODES; i++) {
		ASSERT(g_nodes[i].v == VUINT32_MAX);
	}

	count = vatomic32_read_rlx(&g_cnt_in);
	ASSERT(count == (NODES + PRODUCER_COUNT));
	count = vatomic32_read_rlx(&g_cnt_out);
	ASSERT(count == NODES);
	V_UNUSED(count);
	return 0;
}
