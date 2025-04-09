/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
procedure xchg(load_order, store_order: OrderRelation, addr, input: Register) returns (output: Register);
    modifies step, states, effects, ordering, global_monitor_exclusive, event_register;
    ensures {:msg "load happens within function bounds"} (
        var i := states[step]->last_load;
        old(step) <= i && i < step
    );
    ensures {:msg "store happens within function bounds"} (
        var i := states[step]->last_store;
        old(step) <= i && i < step
    );
    ensures {:msg "output register contains correct value"}
        states[step]->gpr[output] == memory[states[step]->last_load, old(states[step]->gpr[addr])];
    ensures {:msg "memory is preserved between load and store"} (
        var load, store, addr := states[step]->last_load, states[step]->last_store, old(states[step]->gpr[addr]);
        memory[load, addr] == memory[store, addr]
    );
    ensures {:msg "load produces read to correct address"}
        effects[states[step]->last_load][read(old(states[step]->gpr[addr]))];
    ensures {:msg "store produces write to correct address with correct value"}
        effects[states[step]->last_store][write(
            old(states[step]->gpr[addr]),
            old(states[step]->gpr[input]) // will be replaced depending on function
        )];
    ensures {:msg "no other write effects"}
        (forall i: StateIndex, e: Effect ::
            old(step) <= i && i < step && i != states[step]->last_store ==>
                !(effects[i][e] && e is write)
        );
        /* Above is equivalent to
            no_writes(old(step), states[step]->last_store, effects) &&
            no_writes(states[step]->last_store + 1, step, effects);
        */
    ensures {:msg "load ordering"}
        load_order[states[step]->last_load, old(step), step, ordering];
    ensures {:msg "store ordering"}
        store_order[states[step]->last_store, old(step), step, ordering];

procedure cmpxchg(load_order, store_order: OrderRelation, addr, exp, input: Register) returns (output: Register);
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
                load_val != states[step]->gpr[exp])));
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
            var store_val := gpr[input]; (
            effects[store][write(gpr[addr], store_val)] && load_val == states[step]->gpr[exp])
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


