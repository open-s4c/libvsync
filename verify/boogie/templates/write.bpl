/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/*
    store_order - ordering of store
*/
procedure write(store_order: OrderRelation)
    modifies step, last_load, last_store, #state, #registers;
    ensures {:msg "no other writes"}
        (forall i : StateIndex ::
            old(step) <= i && i < step && (exists e : Effect :: effects[i] == e && (is_write(e)))
                ==> i == last_store);
    ensures {:msg "store ordering"}
        !no_writes(old(step), step, last_store)
            ==> store_order[last_store, old(step), step, ordering, effects];
{
    #implementation
}

