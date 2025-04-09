/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
type RMWOp = [int, int] int;

/*
    op          - operation to be performed
    load_order  - ordering of load
    store_order - ordering of store

    input       - second operand to op
    output      - first operand to op; loaded from memory
    addr        - address of load/store

    result      - temporary; holds op(output, input); stored to memory
    status      - temporary; used for stx
*/
procedure rmw (op: RMWOp, store_order: OrderRelation, addr, input : Register);
    modifies step, states, effects, ordering, global_monitor_exclusive, event_register;

    ensures {:msg "load happens within function bounds"} (
        var load := states[step]->last_load;
        old(step) <= load && load < step
    );

    // version 1
    ensures {:msg "store happens within function bounds; produces effect with correct order; no other stores"} (
        var load, store, gpr := states[step]->last_load, states[step]->last_store, old(states[step]->gpr); (
        var load_val := memory[load, gpr[addr]]; (
        var store_val := op[load_val, gpr[input]];
        (load_val == store_val && no_writes(old(step), step, effects)) ||
        (
            old(step) <= store && store < step
            && memory[load, gpr[addr]] == memory[store, gpr[addr]]
            && effects[store][write(gpr[addr], store_val)]
            && store_order[store, old(step), step, ordering]
            && (forall i: StateIndex, e: Effect ::
                old(step) <= i && i < step && i != store ==>
                    !(effects[i][e] && e is write)
            )
        )
    )));

    // version 2
    ensures {:msg "if no write happened, the value from memory is already the result of operation"}
        no_writes(old(step), step, effects) ==> (
            var load, gpr := states[step]->last_load, old(states[step]->gpr); (
            var load_val := memory[load, gpr[addr]]; (
            var store_val := op[load_val, gpr[input]];
            load_val == store_val
        )));
    ensures {:msg "store happens within function bounds"}
        !no_writes(old(step), step, effects) ==> (
            var store := states[step]->last_store;
            old(step) <= store && store < step
        );
    ensures {:msg "memory is preserved between load and store"}
        !no_writes(old(step), step, effects) ==> (
            var load, store, addr := states[step]->last_load, states[step]->last_store, old(states[step]->gpr[addr]); (
            memory[load, addr] == memory[store, addr]
        ));
    ensures {:msg "store produces write to correct address with correct value"}
        !no_writes(old(step), step, effects) ==> (
            var load, store, gpr := states[step]->last_load, states[step]->last_store, old(states[step]->gpr); (
            var load_val := memory[load, gpr[addr]]; (
            var store_val := op[load_val, gpr[input]];
            effects[store][write(gpr[addr], store_val)]
        )));
    ensures {:msg "no other write effects"}
        !no_writes(old(step), step, effects) ==> (
            var store := states[step]->last_store; (
            forall i: StateIndex, e: Effect ::
                old(step) <= i && i < step && i != store ==>
                    !(effects[i][e] && e is write)
        ));
    ensures {:msg "store ordering"}
        !no_writes(old(step), step, effects) ==> (
            var store := states[step]->last_store;
            store_order[store, old(step), step, ordering]
        );

procedure get_rmw(op: RMWOp, load_order, store_order: OrderRelation, addr, input: Register) returns (output: Register);
    modifies step, states, effects, ordering, global_monitor_exclusive, event_register;

    ensures {:msg "load happens within function bounds"} (
        var load := states[step]->last_load;
        old(step) <= load && load < step
    );
    ensures {:msg "load ordering"}
        load_order[states[step]->last_load, old(step), step, ordering];
    ensures {:msg "output register contains loaded value"}
        states[step]->gpr[output] == memory[states[step]->last_load, old(states[step]->gpr[addr])];
    ensures {:msg "if no write happened, the value from memory is already the result of operation"}
        no_writes(old(step), step, effects) ==> (
            var load, gpr := states[step]->last_load, old(states[step]->gpr); (
            var load_val := memory[load, gpr[addr]]; (
            var store_val := op[load_val, gpr[input]];
            load_val == store_val
        )));
    ensures {:msg "store happens within function bounds"}
        !no_writes(old(step), step, effects) ==> (
            var store := states[step]->last_store;
            old(step) <= store && store < step
        );
    ensures {:msg "memory is preserved between load and store"}
        !no_writes(old(step), step, effects) ==> (
            var load, store, addr := states[step]->last_load, states[step]->last_store, old(states[step]->gpr[addr]); (
            memory[load, addr] == memory[store, addr]
        ));
    ensures {:msg "store produces write to correct address with correct value"}
        !no_writes(old(step), step, effects) ==> (
            var load, store, gpr := states[step]->last_load, states[step]->last_store, old(states[step]->gpr); (
            var load_val := memory[load, gpr[addr]]; (
            var store_val := op[load_val, gpr[input]];
            effects[store][write(gpr[addr], store_val)]
        )));
    ensures {:msg "no other write effects"}
        !no_writes(old(step), step, effects) ==> (
            var store := states[step]->last_store; (
            forall i: StateIndex, e: Effect ::
                old(step) <= i && i < step && i != store ==>
                    !(effects[i][e] && e is write)
        ));
    ensures {:msg "store ordering"}
        !no_writes(old(step), step, effects) ==> (
            var store := states[step]->last_store;
            store_order[store, old(step), step, ordering]
        );


