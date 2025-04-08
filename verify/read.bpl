/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
procedure read(load_order: OrderRelation, addr: Register) returns (v : Register);
    modifies step, states, effects, ordering, global_monitor_exclusive, event_register;
    ensures {:msg "load happens within function bounds"} (
        var i := states[step]->last_load;
        old(step) <= i && i < step
    );
    ensures {:msg "no write effects"} (
        forall i: StateIndex, e: Effect ::
            old(step) <= i && i < step ==>
                !(effects[i][e] && e is write)
    );
    ensures {:msg "read effect produced and correct"}
        effects[states[step]->last_load][read(old(states[step]->gpr[addr]))];
    ensures {:msg "order"}
        load_order[states[step]->last_load, old(step), step, ordering];
    ensures {:msg "output register contains loaded value"}
        states[step]->gpr[v] == memory[states[step]->last_load, old(states[step]->gpr[addr])];


