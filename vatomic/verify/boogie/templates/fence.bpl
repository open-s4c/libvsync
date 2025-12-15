/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/*
    fence_order - ordering of fence
*/
procedure fence(fence_order: OrderRelation)
    modifies step, effects, ordering, atomic, last_load, last_store, #state, #registers;
    ensures {:msg "no writes"} no_writes(old(step), step, last_store);
    ensures {:msg "fence ordering"}
        fence_order[0, old(step), step, ordering, effects];
{
    #implementation
}

