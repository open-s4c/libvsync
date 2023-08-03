/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <vsync/common/assert.h>
#include <stdlib.h>
#include <vsync/stack/quack.h>
#include <vsync/common/compiler.h>

#define NODES 6

quack_t quack;

typedef struct node_s {
	quack_node_t n;
	vsize_t v;
} node_t;

node_t g_nodes[NODES];

void
test1(void)
{
	for (vsize_t i = 0; i < NODES; i++) {
		node_t *n = &g_nodes[i];
		n->v	  = i;
		quack_push(&quack, &n->n);
	}

	quack_node_t *node = quack_reverse(quack_popall(&quack));
	for (; node != NULL; node = node->next) {
		node_t *nn = container_of(node, node_t, n);
		nn->v	   = VUINT32_MAX;
	}

	for (vsize_t i = 0; i < NODES; i++) {
		ASSERT(g_nodes[i].v == VUINT32_MAX);
	}
}

void
test2(void)
{
	node_t *n1 = (node_t *)malloc(sizeof(node_t));
	if (n1) {
		n1->v = 123U;

		// push once
		quack_push(&quack, &n1->n);
		quack_node_t *qn = quack_reverse(quack_popall(&quack));
		node_t *n2		 = container_of(qn, node_t, n);
		if (n1 == n2) {
			ASSERT(n2->v == 123U);
		} else {
			ASSERT(0 && "n1 != n2");
		}

		// pop empty
		qn = quack_reverse(quack_popall(&quack));
		ASSERT(qn == NULL);

		// push again
		for (vsize_t i = 0; i < NODES; i++) {
			n1->v = i;
			quack_push(&quack, &n1->n);
			qn = quack_reverse(quack_popall(&quack));
			ASSERT(n1 == n2);
			ASSERT(n2->v == i);
		}

		free(n1);
		V_UNUSED(n2, qn);
	}
}

int
main(void)
{
	test1();
	test2();
	return 0;
}
