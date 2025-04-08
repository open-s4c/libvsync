/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
type AwaitOp = [int, int] bool;

const eq, neq, lt, le, gt, ge: AwaitOp;

axiom eq  == (lambda x, y: int :: x == y);
axiom neq == (lambda x, y: int :: x != y);
axiom lt  == (lambda x, y: int :: x <  y);
axiom le  == (lambda x, y: int :: x <= y);
axiom gt  == (lambda x, y: int :: x >  y);
axiom ge  == (lambda x, y: int :: x >= y);

procedure await(op : AwaitOp, load_order : OrderRelation, addr, input : Register) returns (val : Register);
    modifies step, states, effects, ordering, global_monitor_exclusive, event_register; 
    ensures {:msg "load happens within function bounds"} (
        var i := states[step]->last_load;
        old(step) <= i && i < step
    );
    ensures {:msg "val register contains correct value"}
        states[step]->gpr[val] == memory[states[step]->last_load, old(states[step]->gpr[addr])];
    ensures {:msg "respect await operation"}
        // different for different awaits
        op[states[step]->gpr[val], old(states[step]->gpr[input])] == true;
    ensures {:msg "load produces read effect to correct address"}
        effects[states[step]->last_load][read(old(states[step]->gpr[addr]))];
    ensures {:msg "no write effects"}
        no_writes(old(step), step, effects);
    ensures {:msg "load ordering"}
        load_order[states[step]->last_load, old(step), step, ordering];