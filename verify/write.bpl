/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
procedure write(store_order: OrderRelation, addr, src: Register);
    modifies step, states, effects, ordering, global_monitor_exclusive, event_register;
    ensures {:msg "store happens within function bounds"} (
        var i := states[step]->last_store;
        old(step) <= i && i < step
    );
    ensures {:msg "order"}
        store_order[states[step]->last_store, old(step), step, ordering];
    ensures {:msg "produces write effect"} (
        var gpr := old(states[step]->gpr);
        effects[states[step]->last_store][write(gpr[addr], gpr[src])]
    );
    ensures {:msg "no other writes"} (
        forall i : StateIndex, e : Effect ::
            old(step) <= i && i < step && i != states[step]->last_store ==>
                !(effects[i][e] && (e is write))
    );

