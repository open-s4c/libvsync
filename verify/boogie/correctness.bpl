/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/***************************

    This file contains correctness conditions that need to be proven about the ISA.

*/

/* ensure SC accesses provide ppo between each other. */
procedure verify_sc() 
ensures (sc_impl is RCsc || sc_impl is Mixed) ==> (forall i, j : StateIndex ::
    is_effect(effects[i]) && is_effect(effects[j]) && is_sc(ordering[i]) && is_sc(ordering[j]) && i < j ==> ppo(i, j, ordering, effects));
{

}


/* Prove meta properties about execute, that are used in the proof */
procedure verify_execute(instr : Instruction) returns (r : bv64)
    #modifies;
    ensures {:msg "load return is correct"} (
            forall a, v: bv64, vis : bool :: 
                effects[step-1] == read(a,v,vis) && returning_load(instr)  ==> 
                    r == v
    );

    ensures {:msg "last_load tracked correctly"} (
            (is_read(effects[step-1])) ==
                (step-1 == last_load)
    );
    ensures ( // can define no_writes through last_store 
            (is_write(effects[step-1])) ==
                (step-1 == last_store)
    );
    ensures last_load < step;
    ensures last_store < step;

    ensures (forall i : StateIndex ::
            atomic[i, step-1] ==> i == last_load && step-1 == last_store
        );
    ensures (forall i, j : StateIndex ::
            atomic[i, j] ==> i <= j && j < step);
{
    var last_step : StateIndex;
    call assume_requires_execute(instr);
    
    assume last_load < step;
    assume last_store < step;
    assume (forall i, j : StateIndex :: atomic[i, j] ==> i <= j && j < step);
    last_step := step;
    call r := execute(instr);
    assert (step == last_step + 1);
}