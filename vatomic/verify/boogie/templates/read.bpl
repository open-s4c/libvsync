/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/*
    ret         - computation of return value (either ret_old, for normal read/rmw, or the same as op, for ..._get_... rmw)
    load_order  - ordering of load
*/

procedure read(ret : RMWOp, load_order: OrderRelation)
    modifies step, last_load, last_store, #state, #registers;
    ensures {:msg "load happens within function bounds"}
        old(step) <= last_load && last_load < step;
    ensures {:msg "load order"}
        load_order[last_load, old(step), step, ordering, effects];
    ensures {:msg "is visible"}
        effects[last_load]->read_visible;
    ensures {:msg "correct output"}
        (var extracted := bit_and(extract_value(bin_sub(old(#address), effects[last_load]->addr), effects[last_load]->read_value), #value_mask);
        (var returned := ret[extracted, old(#input1), old(#input2)];
            (bit_and(#output, #value_mask) == bit_and(returned, #value_mask))));

{
    #implementation
}