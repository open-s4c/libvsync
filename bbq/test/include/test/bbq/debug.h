/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2026. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VSYNC_BBQ_DEBUG_H
#define VSYNC_BBQ_DEBUG_H

/* debug function */
static inline void
bbq_spsc_debug(struct bbq_spsc_s *rb)
{
    (void)rb;
#if 0
    vuint16_t block_number   = 1U << BBQ_BLOCK_NUM_LOG;
    vuint16_t block_size     = rb->config.blk_size;
    vuint16_t data_space     = block_size - BBQ_BLOCK_SPSC_INIT_VALUE;
    vuint64_t write_index    = BBQ_GLOBAL_IDX(rb->widx);
    vuint64_t write_version  = BBQ_GLOBAL_VSN(rb->widx);
    vuint64_t read_index     = BBQ_GLOBAL_IDX(rb->ridx);
    vuint64_t read_version   = BBQ_GLOBAL_VSN(rb->ridx);

    printf("------------------------------------------------------------------------\n");
    printf("Drop old mode:      %d\n", BBQ_MODE == BBQ_DROP_OLD);
    printf("Block number:       %d\n", block_number);
    printf("Block init value:   %ld\n", BBQ_BLOCK_SPSC_INIT_VALUE);
    printf("Block size:         %d\n", block_size);
    printf("Entry size:         %ld\n", BBQ_ENTRY_SIZE);
    printf("Data space:         %d\n", data_space);
    printf("widx:               (%ld,\t%ld)\n", write_version, write_index);
    printf("ridx:               (%ld,\t%ld)\n", read_version, read_index);

    for (int i = 0; i < block_number; i++) {
        printf("------------------------------------------------\n");

        struct bbq_spsc_block_s *blk = (struct bbq_spsc_block_s *) (rb->blk + i * block_size);
        printf("[Block %d] %p \n", i, (void *) blk);

        vuint64_t committed_space    = BBQ_LOCAL_IDX(vatomic64_read(&blk->committed));
        vuint64_t committed_version  = BBQ_LOCAL_VSN(vatomic64_read(&blk->committed));

        vuint64_t consumed_space     = BBQ_LOCAL_IDX(vatomic64_read(&blk->consumed));
        vuint64_t consumed_version   = BBQ_LOCAL_VSN(vatomic64_read(&blk->consumed));

        printf("committed:      (%" VUINT64_FORMAT ",\t%" VUINT64_FORMAT ")\n", committed_version, committed_space);
        printf("consumed:       (%" VUINT64_FORMAT ",\t%" VUINT64_FORMAT ")\n", consumed_version, consumed_space);
    }
#endif
}

static inline void
bbq_mpsc_debug(struct bbq_mpsc_s *rb)
{
    (void)rb;
#if 0
    vuint16_t block_number   = 1U << BBQ_BLOCK_NUM_LOG;
    vuint16_t block_size     = rb->config.blk_size;
    vuint16_t data_space     = block_size - BBQ_BLOCK_MPSC_INIT_VALUE;
    vuint64_t write_index    = BBQ_GLOBAL_IDX(vatomic64_read(&rb->widx));
    vuint64_t write_version  = BBQ_GLOBAL_VSN(vatomic64_read(&rb->widx));
    vuint64_t read_index     = BBQ_GLOBAL_IDX(rb->ridx);
    vuint64_t read_version   = BBQ_GLOBAL_VSN(rb->ridx);

    DBG("------------------------------------------------------------------------\n");
    DBG("Drop old mode:      %d\n", BBQ_MODE == BBQ_DROP_OLD);
    DBG("Block number:       %" VUINT16_FORMAT "\n", block_number);
    DBG("Block init value:   %" VUINT16_FORMAT "\n", BBQ_BLOCK_MPSC_INIT_VALUE);
    DBG("Block size:         %" VUINT16_FORMAT "\n", block_size);
    DBG("Entry size:         %" VUINT64_FORMAT "\n", (vuint64_t) BBQ_ENTRY_SIZE);
    DBG("Data space:         %" VUINT16_FORMAT "\n", data_space);
    DBG("widx:               (%" VUINT64_FORMAT ",\t%" VUINT64_FORMAT ")\n", write_version, write_index);
    DBG("ridx:               (%" VUINT64_FORMAT ",\t%" VUINT64_FORMAT ")\n", read_version, read_index);

    for (int i = 0; i < block_number; i++) {
        printf("------------------------------------------------\n");

        struct bbq_mpsc_block_s *blk = (struct bbq_mpsc_block_s *) (rb->blk + i * block_size);
        printf("[Block %d] %p \n", i, (void *) blk);

        vuint64_t allocated_space    = BBQ_LOCAL_IDX(vatomic64_read(&blk->allocated));
        vuint64_t allocated_version  = BBQ_LOCAL_VSN(vatomic64_read(&blk->allocated));

        vuint64_t committed_space    = BBQ_LOCAL_IDX(vatomic64_read(&blk->committed));
        vuint64_t committed_version  = BBQ_LOCAL_VSN(vatomic64_read(&blk->committed));

        vuint64_t consumed_space     = BBQ_LOCAL_IDX(vatomic64_read(&blk->consumed));
        vuint64_t consumed_version   = BBQ_LOCAL_VSN(vatomic64_read(&blk->consumed));

        printf("allocated:      (%ld,\t%ld)\n", allocated_version, allocated_space);
        printf("committed:      (%ld,\t%ld)\n", committed_version, committed_space);
        printf("consumed:       (%ld,\t%ld)\n", consumed_version, consumed_space);
    }
#endif
}

static inline void
bbq_spmc_debug(struct bbq_spmc_s *rb)
{
    (void)rb;
#if 0
    vuint16_t block_number   = 1U << BBQ_BLOCK_NUM_LOG;
    vuint16_t block_size     = rb->config.blk_size;
    vuint16_t data_space     = block_size - BBQ_BLOCK_SPMC_INIT_VALUE;
    vuint64_t write_index    = BBQ_GLOBAL_IDX(rb->widx);
    vuint64_t write_version  = BBQ_GLOBAL_VSN(rb->widx);
    vuint64_t read_index     = BBQ_GLOBAL_IDX(vatomic64_read(&rb->ridx));
    vuint64_t read_version   = BBQ_GLOBAL_VSN(vatomic64_read(&rb->ridx));

    printf("------------------------------------------------------------------------\n");
    printf("Drop old mode:      %d\n", BBQ_MODE == BBQ_DROP_OLD);
    printf("Block number:       %d\n", block_number);
    printf("Block init value:   %ld\n", BBQ_BLOCK_SPMC_INIT_VALUE);
    printf("Block size:         %d\n", block_size);
    printf("Entry size:         %ld\n", BBQ_ENTRY_SIZE);
    printf("Data space:         %d\n", data_space);
    printf("widx:               (%ld,\t%ld)\n", write_version, write_index);
    printf("ridx:               (%ld,\t%ld)\n", read_version, read_index);

    for (int i = 0; i < block_number; i++) {
        printf("------------------------------------------------\n");

        struct bbq_spmc_block_s *blk = (struct bbq_spmc_block_s *) (rb->blk + i * block_size);
        printf("[Block %d] %p \n", i, (void *) blk);

        vuint64_t committed_space    = BBQ_LOCAL_IDX(vatomic64_read(&blk->committed));
        vuint64_t committed_version  = BBQ_LOCAL_VSN(vatomic64_read(&blk->committed));

        vuint64_t reserved_space     = BBQ_LOCAL_IDX(vatomic64_read(&blk->reserved));
        vuint64_t reserved_version   = BBQ_LOCAL_VSN(vatomic64_read(&blk->reserved));

        vuint64_t consumed_space     = BBQ_LOCAL_IDX(vatomic64_read(&blk->consumed));
        vuint64_t consumed_version   = BBQ_LOCAL_VSN(vatomic64_read(&blk->consumed));

        printf("committed:      (%ld,\t%ld)\n", committed_version, committed_space);
        printf("reserved:       (%ld,\t%ld)\n", reserved_version, reserved_space);
        printf("consumed:       (%ld,\t%ld)\n", consumed_version, consumed_space);
    }
#endif
}

static inline void
bbq_mpmc_debug(struct bbq_mpmc_s *rb)
{
    (void)rb;
#if 0
    vuint16_t block_number   = 1U << BBQ_BLOCK_NUM_LOG;
    vuint16_t block_size     = rb->config.blk_size;
    vuint16_t data_space     = block_size - BBQ_BLOCK_MPMC_INIT_VALUE;
    vuint64_t write_index    = BBQ_GLOBAL_IDX(vatomic64_read(&rb->widx));
    vuint64_t write_version  = BBQ_GLOBAL_VSN(vatomic64_read(&rb->widx));
    vuint64_t read_index     = BBQ_GLOBAL_IDX(vatomic64_read(&rb->ridx));
    vuint64_t read_version   = BBQ_GLOBAL_VSN(vatomic64_read(&rb->ridx));

    printf("------------------------------------------------------------------------\n");
    printf("Drop old mode:      %d\n", BBQ_MODE == BBQ_DROP_OLD);
    printf("Block number:       %d\n", block_number);
    printf("Block init value:   %ld\n", BBQ_BLOCK_MPMC_INIT_VALUE);
    printf("Block size:         %d\n", block_size);
    printf("Entry size:         %ld\n", BBQ_ENTRY_SIZE);
    printf("Data space:         %d\n", data_space);
    printf("widx:               (%ld,\t%ld)\n", write_version, write_index);
    printf("ridx:               (%ld,\t%ld)\n", read_version, read_index);

    for (int i = 0; i < block_number; i++) {
        printf("------------------------------------------------\n");

        struct bbq_mpmc_block_s *blk = (struct bbq_mpmc_block_s *) (rb->blk + i * block_size);
        printf("[Block %d] %p \n", i, (void *) blk);

        vuint64_t allocated_space    = BBQ_LOCAL_IDX(vatomic64_read(&blk->allocated));
        vuint64_t allocated_version  = BBQ_LOCAL_VSN(vatomic64_read(&blk->allocated));

        vuint64_t committed_space    = BBQ_LOCAL_IDX(vatomic64_read(&blk->committed));
        vuint64_t committed_version  = BBQ_LOCAL_VSN(vatomic64_read(&blk->committed));

        vuint64_t reserved_space     = BBQ_LOCAL_IDX(vatomic64_read(&blk->reserved));
        vuint64_t reserved_version   = BBQ_LOCAL_VSN(vatomic64_read(&blk->reserved));

        vuint64_t consumed_space     = BBQ_LOCAL_IDX(vatomic64_read(&blk->consumed));
        vuint64_t consumed_version   = BBQ_LOCAL_VSN(vatomic64_read(&blk->consumed));

        DBG("allocated:      (%ld,\t%ld)\n", allocated_version, allocated_space);
        DBG("committed:      (%ld,\t%ld)\n", committed_version, committed_space);
        DBG("reserved:       (%ld,\t%ld)\n", reserved_version, reserved_space);
        DBG("consumed:       (%ld,\t%ld)\n", consumed_version, consumed_space);
    }
#endif
}

#endif
