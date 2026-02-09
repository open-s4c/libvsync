/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef TEST_CASE_H
#define TEST_CASE_H
/**
 * This test case targets an edge case in level queue.
 * It is relevant when block capacity is 2.
 *
 * The edge case happens when the block is not full yet,
 * but the consumer has consumed all produced elements in that block so far.
 * The consumer then decides to reattach the block to the end of the queue, and
 * continue to consume from a newly dequeued block. Between these two steps the
 * producer of that block might already fill the block and attach a new block.
 * If the current block is abandoned by the consumer then that will lead to
 * violating FIFO. Hence, the consumer must recheck if the state of the
 * block has changed after it moved it to the end of the queue:
 * - if it has not changed it means the producer will use it for future
 * enqueues, and all of its future blocks will be enqueued in order.
 * - if it has changed and there are new elements to consume then the consumer
 * will make sure to consume them before moving to another block.
 *
 * Test case description:0
 *
 * Producers:
 *  P1[tid=1]: enq 1, 2, 3
 *  P2[tid=2]: enq 4
 *
 * consumer: consumes 4 elements
 *
 * P1 enqueues 1, P2 enqueues 4:
 *
 * blk_1 [1, X] -> blk_2 [5, X]
 *
 * consumer consumes 1, found blk_1 has no data
 *
 * P1 enqueues 2, 3 and 4:
 * blk_1 [X, 2] -> blk_2 [5, X] -> blk_3 [3, X]
 *
 * consumer move blk_1 to the end:
 * blk_2 [4, X] -> blk_3 [3, X] -> blk_1 [X, 2]
 *
 * If the consumer does not recheck the state of blk_1
 * it will consume 1, 4, 3, 2 // FIFO order violated 3 is consumed before 2
 * If it does it will consume: 1, 2, 4, 3 // FIFO order maintained 2 is consumed
 * before 3
 *
 */

#define MAX_DEQUEUE    4U
#define VALID_COMB_LEN 4U

vuintptr_t vals[MAX_DEQUEUE] = {0};

/* acceptable dequeue order maintaining FIFO */
vuintptr_t valid_com[VALID_COMB_LEN][MAX_DEQUEUE] = {{4, 1, 2, 3},
                                                     {1, 4, 2, 3},
                                                     {1, 2, 4, 3},
                                                     {1, 2, 3, 4}};

vbool_t
is_valid(void)
{
    for (vsize_t i = 0; i < VALID_COMB_LEN; i++) {
        vbool_t match = true;
        for (vsize_t j = 0; j < MAX_DEQUEUE; j++) {
            if (valid_com[i][j] != vals[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return true;
        }
    }
    return false;
}

void
pre(void)
{
    // pre-enqueue for thread 1, to speed up the test case
    enq(1, 1);
#if LEVEL_QUEUE_BLOCK_NUM_ENTRIES == 1 || defined(MPSC)
    // speed up for block capacity 2 or when targeting the other queue
    enq(1, 2);
#endif
}

void
consumer(void)
{
    vuintptr_t v = 0;
    for (vsize_t i = 0; i < MAX_DEQUEUE; i++) {
        do {
            v = deq();
            verification_assume(v != 0);
            vals[i] = v;
        } while (v == 0);
    }
}

void
p1(void)
{
    vsize_t tid = 1;
#if LEVEL_QUEUE_BLOCK_NUM_ENTRIES == 2 && !defined(MPSC)
    enq(tid, 2);
#endif
    enq(tid, 3);
}

void
p2(void)
{
    vsize_t tid = 2;
    enq(tid, 4);
}

void
post(void)
{
    vbool_t fifo_valid = is_valid();
    if (!fifo_valid) {
        printf("{");
        for (vsize_t i = 0; i < MAX_DEQUEUE; i++) {
            printf("%lu, ", vals[i]);
        }
        printf("}\n");
        ASSERT(0 && "FIFO violated!");
    }
}
#endif
