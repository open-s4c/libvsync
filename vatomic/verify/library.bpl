/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
// This builtin boogie function returns const array
function {:builtin "MapConst"} ConstArray<T,U>(U): [T]U;

type Register;
type StateIndex = int;
// type Word = bv64;

// function {:bvbuiltin "(_ int2bv 64)"} int2bv_64(x: int): bv64;

// function {:bvbuiltin "bv2int"} bv2int_64(bv: bv64): int;

// function {:bvbuiltin "bvand"} bvand_64(x: bv64, y: bv64): bv64;

// function {:bvbuiltin "bvor"} bvor_64(x: bv64, y: bv64): bv64;

// function {:bvbuiltin "bvxor"} bvxor_64(x: bv64, y: bv64): bv64;

// No implementation needed
// The variant with implementation makes it run very slow and it gives no extra information
// The version without implementation creates a generic operation for each case
// NOTE: There is an issue on the boogie repo with regards to this 
function bit_and(x: int, y: int): int;
function bit_or(x: int, y: int): int;
function bit_xor(x: int, y: int): int;

function bit_not(x: int): int;

axiom (forall x: int :: bit_not(bit_not(x)) == x); // double negation on bits
axiom (forall x: int, y: int :: bit_and(x, y) == bit_and(y, x));
axiom (forall x: int, y: int :: bit_or(x, y) == bit_or(y, x));
axiom (forall x: int, y: int :: bit_xor(x, y) == bit_xor(y, x));

const max: [int, int] int;
axiom max == (lambda x, y: int ::
    if x > y then x else y
);

const add: [int, int] int;
axiom add == (lambda x, y: int :: x + y);

const sub: [int, int] int;
axiom sub == (lambda x, y: int :: x - y);

const and: [int, int] int;
axiom and == (lambda x, y: int :: bit_and(x, y));

const or: [int, int] int;
axiom or == (lambda x, y: int :: bit_or(x, y));

const xor: [int, int] int;
axiom xor == (lambda x, y: int :: bit_xor(x, y));

datatype Flags {
    Flags (
        N: bool,
        Z: bool,
        C: bool
    )
}

datatype Effect {
    read(addr: int),
    write(addr: int, value: int)
}

datatype Ordering {
    AcquirePC(),
    Acquire(),
    Release()
}

datatype Monitor {
    exclusive(addr: int, step: StateIndex),
    open()
}

datatype State {
    State(
        gpr: [Register] int,
        flags: Flags,
        local_monitor: Monitor,
        last_load, last_store: StateIndex
    )
}

var step: StateIndex;
var states: [StateIndex] State;
var global_monitor_exclusive: [StateIndex] bool;
var event_register: [StateIndex] bool;
var memory: [StateIndex, int] int;
var effects: [StateIndex][Effect] bool;
var ordering: [StateIndex][Ordering] bool;

var store_clears_local_monitor: bool;
var store_clears_global_monitor: bool;

datatype Instruction {
    ld(acq: bool, dest, addr: Register),
    ldx(acq: bool, dest, addr: Register),
    st(rel: bool, src, addr: Register),
    stx(rel: bool, status, src, addr: Register),

    mov(dest, src: Register),
    cmp(opnd1, opnd2: Register),
    add(dest, first, second: Register),
    sub(dest, first, second: Register),
    andd(dest, first, second: Register),
    orr(dest, first, second: Register),
    eor(dest, first, second: Register),
    wfe(),
    sevl(),

    //LSE instructions

    mvn(dest, src: Register), // complements the bits in result
    neg(dest, src: Register), // negates the bits in the result
    
    swp(acq, rel: bool, src, dest, addr: Register), // exchanges 
    cas(acq, rel: bool, dest, src, addr: Register), // compare and swap

    ldumax(acq, rel: bool, src, dest, addr: Register), // maximum between src register, and loaded value
    ldclr(acq, rel: bool, src, dest, addr: Register), // bitwise and between src and ~loaded value
    ldset(acq, rel: bool, src, dest, addr: Register), // bitwise or between  src and loaded value
    ldeor(acq, rel: bool, src, dest, addr: Register), // bitwise xor between src and loaded value
    ldadd(acq, rel: bool, src, dest, addr: Register), // sum of src and loaded value

    stumax(rel: bool, src, addr: Register), // store maximum between src and addr
    stclr(rel: bool, src, addr: Register), // store and between src and ~addr
    stset(rel: bool, src, addr: Register), // store or
    steor(rel: bool, src, addr: Register), // store xor
    stadd(rel: bool, src, addr: Register) // store sum
}


procedure execute(instr: Instruction)
    modifies states, effects, ordering, step, global_monitor_exclusive, event_register;
    ensures step == old(step + 1);
    ensures {:msg "state"}
        states == old(states[
            step + 1 := (
                var gpr, local_monitor := states[step]->gpr, states[step]->local_monitor; (
                var stx_success, cas_success :=
                    local_monitor is exclusive
                    && local_monitor->addr == gpr[instr->addr]
                    && global_monitor_exclusive[step],
                    memory[step, gpr[instr->addr]] == gpr[instr->dest];
                states[step]
                ->(gpr :=
                    if instr is ld
                    || instr is ldx
                    || instr is swp
                    || instr is cas
                    || instr is ldumax
                    || instr is ldclr
                    || instr is ldset
                    || instr is ldeor
                    || instr is ldadd
                    then
                        gpr[instr->dest := memory[step, gpr[instr->addr]]]
                    else if instr is stx then
                        gpr[instr->status := if stx_success then 0 else 1]
                    else if instr is mov then
                        gpr[instr->dest := gpr[instr->src]]
                    else if instr is add then 
                        gpr[instr->dest := gpr[instr->first] + gpr[instr->second]]
                    else if instr is sub then
                        gpr[instr->dest := gpr[instr->first] - gpr[instr->second]]
                    else if instr is andd then
                        gpr[instr->dest := bit_and(gpr[instr->first], gpr[instr->second])]
                    else if instr is orr then 
                        gpr[instr->dest := bit_or(gpr[instr->first], gpr[instr->second])]
                    else if instr is eor then
                        gpr[instr->dest := bit_xor(gpr[instr->first], gpr[instr->second])]
                    else if instr is mvn then
                        gpr[instr->dest := bit_not(gpr[instr->src])]
                    else if instr is neg then 
                        gpr[instr->dest := 0 - gpr[instr->src]]
                    else
                        gpr
                )
                ->(last_load :=
                    if instr is ld
                    || instr is ldx
                    || instr is swp
                    || instr is cas 
                    || instr is ldumax
                    || instr is ldclr
                    || instr is ldset
                    || instr is ldeor
                    || instr is ldadd
                    || instr is stumax
                    || instr is stclr
                    || instr is stset
                    || instr is steor
                    || instr is stadd
                    then
                        step
                    else
                        states[step]->last_load
                )
                ->(last_store :=
                    if instr is st
                    || instr is swp
                    || instr is ldclr
                    || instr is ldset
                    || instr is ldeor
                    || instr is ldadd
                    || (instr is cas && cas_success)
                    || (instr is stx && stx_success)
                    || (instr is ldumax || instr is stumax)
                    || instr is stclr
                    || instr is stset
                    || instr is steor
                    || instr is stadd
                    then
                        step
                    else
                        states[step]->last_store
                )
                ->(local_monitor :=
                    if instr is ldx then
                        exclusive(gpr[instr->addr], step)
                    else
                        if instr is stx || (
                            store_clears_local_monitor && (
                                instr is st
                                || instr is swp
                                || instr is ldset
                                || instr is ldclr
                                || instr is ldeor
                                || instr is ldadd
                                || instr is stclr
                                || instr is stset
                                || instr is steor
                                || instr is stadd
                                || (instr is cas && cas_success)
                                || (instr is ldumax || instr is stumax) 

                            )
                        )
                        then
                            open()
                        else
                            local_monitor
                )
                ->(flags :=
                    if instr is cmp
                    then (
                        var diff := gpr[instr->opnd1] - gpr[instr->opnd2];
                        Flags(diff < 0, diff == 0, diff >= 0)
                    )
                    else
                        states[step]->flags
                )
            ))
        ]);
    ensures {:msg "effects"}
        (!(instr is ldset || instr is stset
         || instr is ldeor || instr is steor
         || instr is ldadd || instr is stadd
         || instr is ldclr || instr is stclr
         || instr is ldumax || instr is stumax)) ==> effects == old(effects[
            step := (
                var gpr, local_monitor := states[step]->gpr, states[step]->local_monitor;
                ConstArray(false)
                    [read(gpr[instr->addr]) :=
                        instr is ld
                        || instr is ldx 
                        || instr is swp 
                        || instr is cas 


                    ]
                    // [ write(gpr[instr->addr], or[gpr[instr->src], memory[step, gpr[instr->addr]]]) :=
                    //         instr is ldset]
                    [write(gpr[instr->addr], gpr[instr->src]) :=
                        instr is st ||
                        instr is swp ||
                        (instr is cas &&
                            memory[step, gpr[instr->addr]] == gpr[instr->dest]
                        )
                          || (
                            instr is stx
                            && local_monitor is exclusive
                            && local_monitor->addr == gpr[instr->addr]
                            && global_monitor_exclusive[step]
                        )
                    ]

            )
        ]);
    ensures {:msg "ld/stset effects"}
            (instr is ldset || instr is stset) ==> effects == old(effects[
            step := (
                var gpr, local_monitor := states[step]->gpr, states[step]->local_monitor;
                ConstArray(false)
                    [read(gpr[instr->addr]) := true]
                    [write(gpr[instr->addr], or[memory[step, gpr[instr->addr]], gpr[instr->src]]) := true]
            )
        ]);
    ensures {:msg "ld/steor effects"}
            (instr is ldeor || instr is steor) ==> effects == old(effects[
            step := (
                var gpr, local_monitor := states[step]->gpr, states[step]->local_monitor;
                ConstArray(false)
                    [read(gpr[instr->addr]) := true]
                    [write(gpr[instr->addr], xor[memory[step, gpr[instr->addr]], gpr[instr->src]]) := true]
            )
        ]);
    ensures {:msg "ld/stumax effects"}
            (instr is ldumax || instr is stumax) ==> effects == old(effects[
            step := (
                var gpr, local_monitor := states[step]->gpr, states[step]->local_monitor;
                ConstArray(false)
                    [read(gpr[instr->addr]) := true]
                    [write(gpr[instr->addr], max[memory[step, gpr[instr->addr]], gpr[instr->src]]) := true]
            )
        ]);
    ensures {:msg "ld/stadd effects"}
            (instr is ldadd || instr is stadd) ==> effects == old(effects[
            step := (
                var gpr, local_monitor := states[step]->gpr, states[step]->local_monitor;
                ConstArray(false)
                    [read(gpr[instr->addr]) := true]
                    [write(gpr[instr->addr], memory[step, gpr[instr->addr]] + gpr[instr->src]) := true]
            )
        ]);
    ensures {:msg "ld/stclr effects"}
            (instr is ldclr || instr is stclr) ==> effects == old(effects[
            step := (
                var gpr, local_monitor := states[step]->gpr, states[step]->local_monitor;
                ConstArray(false)
                    [read(gpr[instr->addr]) := true]
                    [write(gpr[instr->addr], and[memory[step, gpr[instr->addr]], bit_not(gpr[instr->src])]) := true]
            )
        ]);
    ensures {:msg "ordering"}
        ordering == old(ordering[
            step := ConstArray(false)
            [Acquire() :=
                 instr->acq && (
                    instr is ld 
                    || instr is ldx 
                    || instr is swp 
                    || instr is cas 
                    || instr is ldumax
                    || instr is ldclr
                    || instr is ldset
                    || instr is ldeor
                    || instr is ldadd
                )
            ]
            [Release() :=
                instr->rel && (
                    instr is st ||
                    instr is swp ||
                    instr is ldclr ||
                    instr is ldset ||
                    instr is ldadd ||
                    instr is ldeor ||
                    instr is stclr ||
                    instr is stset ||
                    instr is steor ||
                    instr is stadd ||
                    (  instr is cas && (var gpr := states[step]->gpr;
                        memory[step, gpr[instr->addr]] == gpr[instr->dest]
                    )) || (instr is ldumax || instr is stumax) 
                       ||
                    (instr is stx && (
                        var gpr, local_monitor := states[step]->gpr, states[step]->local_monitor;
                        local_monitor is exclusive
                        && local_monitor->addr == gpr[instr->addr]
                        && global_monitor_exclusive[step]
                    ))
                )
            ]
        ]);
    ensures {:msg "global monitor"}
        (
            global_monitor_exclusive == old(global_monitor_exclusive[step + 1 := false])  // external write can clear monitor at any moment
            && event_register == old(event_register[step + 1 := true]) // it has to set event register
        )
        || global_monitor_exclusive == old(global_monitor_exclusive[step + 1 :=
            if instr is ldx then
                true
            else
                if store_clears_global_monitor && (
                    instr is st
                    || instr is stx 
                    || instr is swp 
                    || instr is ldclr
                    || instr is ldset
                    || instr is ldeor
                    || instr is ldadd
                    || instr is stclr
                    || instr is stset
                    || instr is steor
                    || instr is stadd
                    || (instr is cas && (
                        var gpr := states[step]->gpr;
                        memory[step, gpr[instr->addr]] == gpr[instr->dest]
                    )) 
                    || (instr is ldumax || instr is stumax)
                )
                then
                    false
                else
                    global_monitor_exclusive[step]
        ]);
    ensures {:msg "memory"}
        old(global_monitor_exclusive[step] ==>
            memory[step, states[step]->local_monitor->addr] == memory[step - 1, states[step]->local_monitor->addr]
        );
    /* D1.6.1 The Event Register
        The Event Register for a PE is set by any of the following:
        • A Send Event instruction, SEV, executed by any PE in the system.
        • A Send Event Local instruction, SEVL, executed by the PE.
        • An exception return.
        • The clearing of the global monitor for the PE.
        • An event from a Generic Timer event stream, see Event streams on page D11-5991.
        • An event sent by some IMPLEMENTATION DEFINED mechanism.
    */
    ensures {:msg "event register"}
        event_register == old(event_register[step + 1 := true]) || // extrenal factors can set the event register
        event_register == old(event_register)[step :=
            if instr is wfe then
                false
            else
                instr is sevl ||
                old(event_register[step]) // preserve event register
        ];
    requires {:msg "either event register or global monitor is set for WFE"}
        instr is wfe ==>
            event_register[step] || global_monitor_exclusive[step];
    requires {:msg "stx is paired to ldx"}
        instr is stx ==> (
            var state := states[step];
            state->local_monitor == exclusive(state->gpr[instr->addr], state->last_load)
        );
{
    var stx_succeeds, cas_succeeds, clears_global_monitor, sets_event_register: bool;

    var cmp_diff: int;

     cas_succeeds :=
        memory[step, states[step]->gpr[instr->addr]] == states[step]->gpr[instr->dest];

    states[step + 1] := states[step];
    global_monitor_exclusive[step + 1] := global_monitor_exclusive[step];
    event_register[step + 1] := event_register[step];
    effects[step] := ConstArray(false);
    ordering[step] := ConstArray(false);

    if (instr is ld 
        || instr is ldx 
        || instr is swp 
        || instr is cas 
        || instr is ldclr
        || instr is ldumax
        || instr is ldset
        || instr is ldeor
        || instr is ldadd) {
        states[step + 1]->gpr[instr->dest] := memory[step, states[step]->gpr[instr->addr]];
        states[step + 1]->last_load := step;
        effects[step][read(states[step]->gpr[instr->addr])] := true;
        if (instr is ldx) {
            states[step + 1]->local_monitor := exclusive(states[step]->gpr[instr->addr], step);
            global_monitor_exclusive[step + 1] := true;
        }
        if (instr is ld 
            || instr is ldx 
            || instr is swp 
            || instr is cas 
            || instr is ldumax
            || instr is ldclr
            || instr is ldset
            || instr is ldeor
            || instr is ldadd) {
            ordering[step][Acquire()] := instr->acq;
        }
    }
    if (instr is stclr
        || instr is stset
        || instr is stumax
        || instr is steor
        || instr is stadd) {
        states[step + 1]->last_load := step;
        effects[step][read(states[step]->gpr[instr->addr])] := true;
    }
    if (instr is st || 
        instr is stx || 
        instr is swp || 
        (instr is cas && cas_succeeds) ||
        instr is ldumax || instr is stumax ||
        instr is ldclr || instr is stclr ||
        instr is ldset || instr is stset ||
        instr is ldeor || instr is steor ||
        instr is ldadd || instr is stadd) {
        stx_succeeds := global_monitor_exclusive[step];
        // stx always clears local monitor
        if (instr is stx || store_clears_local_monitor) {
            states[step + 1]->local_monitor := open();
        }
        if (store_clears_global_monitor) {
            global_monitor_exclusive[step + 1] := false;
        }
        if (instr is stx) {
            if (stx_succeeds) {
                states[step + 1]->gpr[instr->status] := 0;

                states[step + 1]->last_store := step;
                effects[step][write(states[step]->gpr[instr->addr], states[step]->gpr[instr->src])] := true;
                ordering[step][Release()] := instr->rel;
            } else {
                states[step + 1]->gpr[instr->status] := 1;
            }
        }  else if (instr is ldclr || instr is stclr) {
            states[step + 1]->last_store := step;
            effects[step][write(states[step]->gpr[instr->addr], and[memory[step,states[step]->gpr[instr->addr]], bit_not(states[step]->gpr[instr->src])])] := true;
            ordering[step][Release()] := instr->rel;
        } else if (instr is ldumax || instr is stumax) {
            states[step + 1]->last_store := step;
            effects[step][write(states[step]->gpr[instr->addr], max[memory[step,states[step]->gpr[instr->addr]], states[step]->gpr[instr->src]])] := true;
            ordering[step][Release()] := instr->rel;
        } else if (instr is ldset || instr is stset) {
            states[step + 1]->last_store := step;
            effects[step][write(states[step]->gpr[instr->addr], or[memory[step,states[step]->gpr[instr->addr]], states[step]->gpr[instr->src]])] := true;
            ordering[step][Release()] := instr->rel;
        } else if (instr is ldeor || instr is steor) {
            states[step + 1]->last_store := step;
            effects[step][write(states[step]->gpr[instr->addr], xor[memory[step,states[step]->gpr[instr->addr]], states[step]->gpr[instr->src]])] := true;
            ordering[step][Release()] := instr->rel;
        }  else if (instr is ldadd || instr is stadd) {
            states[step + 1]->last_store := step;
            effects[step][write(states[step]->gpr[instr->addr], memory[step,states[step]->gpr[instr->addr]] + states[step]->gpr[instr->src])] := true;
            ordering[step][Release()] := instr->rel;
        }
        else {
            states[step + 1]->last_store := step;
            effects[step][write(states[step]->gpr[instr->addr], states[step]->gpr[instr->src])] := true;
            ordering[step][Release()] := instr->rel;
        }
        
    }

    if (instr is mov) {
        states[step + 1]->gpr[instr->dest] := states[step]->gpr[instr->src];
    }
    if (instr is add) {
        states[step + 1]->gpr[instr->dest] := states[step]->gpr[instr->first] + states[step]->gpr[instr->second]; 
    }
    if (instr is sub) {
        states[step + 1]->gpr[instr->dest] := states[step]->gpr[instr->first] - states[step]->gpr[instr->second]; 
    }
    if (instr is andd) {
        states[step + 1]->gpr[instr->dest] := bit_and(states[step]->gpr[instr->first], states[step]->gpr[instr->second]); 
    }
    if (instr is orr) {
        states[step + 1]->gpr[instr->dest] := bit_or(states[step]->gpr[instr->first], states[step]->gpr[instr->second]); 
    }
    if (instr is eor) {
        states[step + 1]->gpr[instr->dest] := bit_xor(states[step]->gpr[instr->first], states[step]->gpr[instr->second]); 
    }
    if (instr is mvn) {
        states[step + 1]->gpr[instr->dest] := bit_not(states[step]->gpr[instr->src]);
    }
    if (instr is neg) {
        states[step + 1]->gpr[instr->dest] := 0 - states[step]->gpr[instr->src];
    }
    if (instr is cmp) {
        cmp_diff := states[step]->gpr[instr->opnd1] - states[step]->gpr[instr->opnd2];
        states[step + 1]->flags->N := cmp_diff < 0;
        states[step + 1]->flags->Z := cmp_diff == 0;
        states[step + 1]->flags->C := cmp_diff >= 0;
    }

    if (global_monitor_exclusive[step]) {
        assume(memory[step, states[step]->local_monitor->addr] == memory[step - 1, states[step]->local_monitor->addr]);
    }

    if (instr is sevl) {
        event_register[step + 1] := true;
    }
    if (instr is wfe) {
        event_register[step + 1] := false;
    }
    if (clears_global_monitor) {
        global_monitor_exclusive[step + 1] := false;
    }
    if (sets_event_register || clears_global_monitor) {
        event_register[step + 1] := true;
    }

    step := step + 1;
}

function cbnz(test: Register, state: State): bool {
    state->gpr[test] != 0
}

function cbz(test: Register, state: State): bool {
    state->gpr[test] == 0
}

// C1.2.4 Condition code
datatype ConditionCode {
    EQ(), // Equal
    NE(), // Not equal
    HS(), // Unsigned higher or same
    LO(), // Unsigned lower
    HI(), // Unsigned higher
    LS()  // Unsigned lower or same
}

function branch(cond: ConditionCode, state: State): bool {(
    var N, Z, C := state->flags->N, state->flags->Z, state->flags->C;
         if cond is EQ then Z
    else if cond is NE then !Z
    else if cond is HS then C
    else if cond is LO then !C
    else if cond is HI then C && !Z
    else if cond is LS then !(C && !Z)
    else false // Should never be reached
)}



function no_writes(from, to: StateIndex, effects: [StateIndex][Effect] bool): bool {
    (forall i: StateIndex, e: Effect ::
        from <= i && i < to ==>
            !(effects[i][e] && e is write)
    )
}

function ppo(step1, step2: StateIndex, ordering: [StateIndex][Ordering] bool): bool {
    step1 < step2 && (
        // Barrier-ordered-before
        ordering[step1][Acquire()] ||
        ordering[step1][AcquirePC()] ||
        ordering[step2][Release()] ||
        (ordering[step1][Release()] && ordering[step2][Acquire()])
    )
}

type OrderRelation = [StateIndex, StateIndex, StateIndex, [StateIndex][Ordering] bool] bool;

const order_rlx: OrderRelation;
axiom order_rlx == (lambda step, entry, exit: StateIndex, ordering: [StateIndex][Ordering] bool ::
    true
);

const order_acq: OrderRelation;
axiom order_acq == (lambda load, entry, exit: StateIndex, ordering: [StateIndex][Ordering] bool ::
    (forall step: StateIndex ::
        step >= exit ==> ppo(load, step, ordering)
    )
);

const order_rel: OrderRelation;
axiom order_rel == (lambda store, entry, exit: StateIndex, ordering: [StateIndex][Ordering] bool ::
    (forall step: StateIndex ::
        step < entry ==> ppo(step, store, ordering)
    )
);

function is_sc(order: [Ordering] bool): bool {
    order[Acquire()] || order[Release()]
}

datatype SCImplementation { LeadingFence(), TrailingFence(), AcqRel() }
const sc_impl: SCImplementation;
axiom sc_impl is AcqRel; // <- IMPORTANT: put required implemetation here

const order_acq_sc: OrderRelation;
axiom order_acq_sc == (lambda load, entry, exit: StateIndex, ordering: [StateIndex][Ordering] bool ::
    order_acq[load, entry, exit, ordering] &&
    if sc_impl is LeadingFence then
        // ordered with all previous operations
        (forall step: StateIndex ::
            step < entry ==> ppo(step, load, ordering)
        )
    else if sc_impl is TrailingFence then
        true
    else if sc_impl is AcqRel then
        // ordered with all previous SC operations
        is_sc(ordering[load]) &&
        (forall step: StateIndex ::
            step < entry ==> is_sc(ordering[step]) ==> ppo(step, load, ordering)
        )
    else false
);

const order_rel_sc: OrderRelation;
axiom order_rel_sc == (lambda store, entry, exit: StateIndex, ordering: [StateIndex][Ordering] bool ::
    order_rel[store, entry, exit, ordering] &&
    if sc_impl is LeadingFence then
        true
    else if sc_impl is TrailingFence then
        // ordered with all later operations
        (forall step: StateIndex ::
            step >= exit ==> ppo(store, step, ordering)
        )
    else if sc_impl is AcqRel then
        // ordered with all later SC operations
        is_sc(ordering[store]) &&
        (forall step: StateIndex ::
            step >= exit ==> is_sc(ordering[step]) ==> ppo(store, step, ordering)
        )
    else false
);
