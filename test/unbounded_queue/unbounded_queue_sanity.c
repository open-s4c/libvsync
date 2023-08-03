/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <vsync/common/assert.h>
#include <vsync/common/dbg.h>
#include <test/queue/ub/queue_interface.h>

queue_t g_queue;
#define SANITY_TID 0

void
test_sanity_fill_empty(vsize_t items_count)
{
	vuint64_t i	  = 0;
	char lbl	  = 'A';
	data_t *node  = NULL;
	vsize_t len	  = 0;
	vbool_t empty = false;

	queue_init(&g_queue);
	queue_register(SANITY_TID, &g_queue);

	empty = queue_empty(SANITY_TID, &g_queue);
	ASSERT(empty);
	len = queue_get_length(SANITY_TID, &g_queue);
	ASSERT(len == 0);
	node = queue_deq(SANITY_TID, &g_queue);
	ASSERT(node == NULL);

	for (i = 0; i < items_count; i++, lbl++) {
		queue_enq(SANITY_TID, &g_queue, i, lbl);
	}

	empty = queue_empty(SANITY_TID, &g_queue);
	ASSERT(!empty);
	len = queue_get_length(SANITY_TID, &g_queue);
	ASSERT(len == items_count);

	DBG_YELLOW("Printing Queue after filling %zu", items_count);
	queue_print(&g_queue, queue_print_callback);

	for (i = 0, lbl = 'A'; i < items_count; i++, lbl++) {
		node = queue_deq(SANITY_TID, &g_queue);
		ASSERT(node->key == i);
		ASSERT(node->lbl == lbl);
		free(node);
	}

	empty = queue_empty(SANITY_TID, &g_queue);
	ASSERT(empty);
	len = queue_get_length(SANITY_TID, &g_queue);
	ASSERT(len == 0);
	node = queue_deq(SANITY_TID, &g_queue);
	ASSERT(node == NULL);

	queue_deregister(SANITY_TID, &g_queue);
	queue_destroy(&g_queue);
	V_UNUSED(len, empty, node);
}

void
test_sanity_fill_destroy(vsize_t items_count)
{
	vuint64_t i	  = 0;
	char lbl	  = 'A';
	vsize_t len	  = 0;
	vbool_t empty = false;
	data_t *node  = NULL;

	queue_init(&g_queue);
	queue_register(SANITY_TID, &g_queue);

	empty = queue_empty(SANITY_TID, &g_queue);
	ASSERT(empty);
	len = queue_get_length(SANITY_TID, &g_queue);
	ASSERT(len == 0);
	node = queue_deq(SANITY_TID, &g_queue);
	ASSERT(node == NULL);

	for (i = 0; i < items_count; i++, lbl++) {
		queue_enq(SANITY_TID, &g_queue, i, lbl);
	}

	empty = queue_empty(SANITY_TID, &g_queue);
	ASSERT(!empty);
	len = queue_get_length(SANITY_TID, &g_queue);
	ASSERT(len == items_count);

	queue_deregister(SANITY_TID, &g_queue);
	queue_destroy(&g_queue);
	V_UNUSED(len, empty, node);
}

void
test_regular_case(void)
{
	vsize_t i		  = 0;
	const vsize_t len = 3;
	data_t *dequeued[len];

	queue_init(&g_queue);
	queue_register(SANITY_TID, &g_queue);

	for (i = 0; i < len; i++) {
		queue_enq(SANITY_TID, &g_queue, i + 1, 'x');
	}

	for (i = 0; i < len; i++) {
		dequeued[i] = queue_deq(SANITY_TID, &g_queue);
		ASSERT(dequeued[i]->key == i + 1);
		ASSERT(dequeued[i]);
		free(dequeued[i]);
	}

	queue_deregister(SANITY_TID, &g_queue);
	queue_destroy(&g_queue);
}

int
main(void)
{
#define TC_COUNT 4
	vsize_t items_count[TC_COUNT] = {1, 2, 3, 10};
	vsize_t i					  = 0;

	test_regular_case();
	for (i = 0; i < TC_COUNT; i++) {
		DBG_YELLOW("Filling %zu items\n", items_count[i]);
		test_sanity_fill_empty(items_count[i]);
		test_sanity_fill_destroy(items_count[i]);
	}
	return 0;
}
