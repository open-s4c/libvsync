/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
datatype Flags {
    Flags (
        N: bool,
        Z: bool,
        C: bool
    )
}

datatype Ordering {
    AcquirePC(),
    Acquire(),
    Release(),
    AcquireRelease(),
    Fence(mode : FenceType),
    NoOrd()
}

datatype Monitor {
    exclusive(addr: bv64),
    open()
}

var local_monitor: Monitor;
var flags: Flags;
var monitor_exclusive: bool;
var event_register: bool;

datatype FenceType {
    SY(),
    LD()
}

datatype Instruction {
    ld(acq: bool, addr: bv64, mask: bv64),
    ldx(acq: bool, addr: bv64, mask: bv64),
    st(rel: bool, src, write_mask, addr: bv64),
    stx(rel: bool, src, write_mask, addr: bv64),

    csel(src1, src2: bv64, cond: bool),
    mov(src: bv64),
    cmp(opnd1, opnd2: bv64),
    add(first, second: bv64),
    sub(first, second: bv64),
    andd(first, second: bv64),
    orr(first, second: bv64),
    eor(first, second: bv64),
    wfe(),
    sevl(),

    dmb(mode : FenceType),
    //LSE instructions

    mvn(src: bv64), // complements the bits in result
    neg(src: bv64), // negates the bits in the result
    
    swp(acq, rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64), // exchanges 
    cas(acq, rel: bool, exp, src, addr: bv64, mask: bv64, write_mask: bv64), // compare and swap

    ldumax(acq, rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64), // maximum between src register, and loaded value
    ldclr(acq, rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64), // bitwise and between src and ~loaded value
    ldset(acq, rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64), // bitwise or between  src and loaded value
    ldeor(acq, rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64), // bitwise xor between src and loaded value
    ldadd(acq, rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64), // sum of src and loaded value

    stumax(rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64), // store maximum between src and addr
    stclr(rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64), // store and between src and ~addr
    stset(rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64), // store or
    steor(rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64), // store xor
    stadd(rel: bool, src, addr: bv64, mask: bv64, write_mask: bv64) // store sum
}

function returning_load(instr : Instruction) : bool {
    instr is ld
    || instr is ldx
    || instr is swp
    || instr is cas 
    || instr is ldumax
    || instr is ldclr
    || instr is ldset
    || instr is ldeor
    || instr is ldadd
    || instr is stumax
}



function visible(instr : Instruction) : bool {
    ! (instr is stumax
    || instr is stclr
    || instr is stset
    || instr is steor
    || instr is stadd)
}

function updated_value(instr: Instruction, read_value : bv64) : bv64 {
    if instr is cas || instr is swp
    then instr->src
    else if instr is ldclr || instr is stclr
    then and[bit_inv(instr->src), read_value]
    else if instr is ldset || instr is stset
    then or[instr->src, read_value]
    else if instr is ldeor || instr is steor
    then xor[instr->src, read_value] 
    else if instr is ldumax || instr is stumax
    then max[instr->src, read_value]
    else if instr is ldadd || instr is stadd
    then add[instr->src, read_value]
    else 0bv64
}

function rmw(instr: Instruction) : bool {
    instr is swp
    || instr is ldumax
    || instr is stumax
    || instr is ldclr
    || instr is stclr
    || instr is ldset
    || instr is stset
    || instr is ldeor
    || instr is steor
    || instr is ldadd
    || instr is stadd
}

function reads(instr: Instruction) : bool {
    rmw(instr) || instr is ld || instr is ldx || instr is cas
}

function writes(instr: Instruction) : bool {
    rmw(instr) || instr is st
}

procedure assume_requires_execute(instr: Instruction);
    modifies step, local_monitor, monitor_exclusive, event_register, last_load, last_store;
    ensures (instr is stx ==> local_monitor is exclusive && local_monitor->addr == instr->addr);
    ensures instr is wfe ==> event_register || monitor_exclusive;


procedure execute_local(instr: Instruction) returns (r : bv64);
    modifies flags;
    ensures
        (r == if instr is mov then instr->src
            else if instr is add then bin_add(instr->first, instr->second)
            else if instr is sub then bin_sub(instr->first, instr->second)
            else if instr is andd then bit_and(instr->first, instr->second)
            else if instr is orr  then bit_or (instr->first, instr->second)
            else if instr is eor  then bit_xor(instr->first, instr->second)
            else if instr is mvn  then bit_inv(instr->src)
            else if instr is neg  then bin_neg(instr->src)
            else if instr is csel then if instr->cond then instr->src1 else instr->src2
            else r)
        &&
        (flags == if instr is cmp
                then (
                    Flags(ult(instr->opnd1, instr->opnd2), instr->opnd1 == instr->opnd2, uge(instr->opnd1, instr->opnd2))
                )
                else
                    old(flags)
                );

procedure execute(instr: Instruction) returns (r : bv64);
    modifies step, local_monitor, monitor_exclusive, event_register, last_load, last_store;
    ensures step == old(step + 1);
    ensures {:msg "state"} (
        var stx_success, cas_success :=
            old(local_monitor == exclusive(instr->addr)
            && monitor_exclusive),
            r == instr->exp;
        (r == if instr is stx then b2i(! stx_success)
            else if returning_load(instr) then bit_and(r, instr->mask)
            else r)
        &&
        (last_load == if reads(instr)
                    then
                        old(step)
                    else
                        old(last_load))
        &&
        (last_store == if writes(instr) || rmw(instr)
                        || (instr is cas && cas_success)
                        || (instr is stx && stx_success)
                    then
                        old(step)
                    else
                        old(last_store))
        &&
        (local_monitor == if instr is ldx then exclusive(instr->addr)
                        else if instr is stx 
                            || instr is cas
                            || reads(instr) || writes(instr)
                            || instr is wfe
                        then open()
                        else old(local_monitor))
        &&
        (effects[old(step)] == 
                        if rmw(instr)
                        || (instr is cas && cas_success)
                        then update(instr->addr, r, visible(instr), updated_value(instr, r), instr->write_mask)
                        else if writes(instr) || (instr is stx && stx_success)
                        then write(instr->addr, instr->src, instr->write_mask)
                        else if reads(instr)
                        then read(instr->addr, r, visible(instr))
                        else no_effect()
            )
        &&
        (ordering[old(step)] == if instr->acq && reads(instr) && (instr->rel && (writes(instr)
                            || (instr is stx && stx_success)
                            || (instr is cas && cas_success)))
                            then AcquireRelease()
                    else if instr->acq && reads(instr)
                    then Acquire()
                    else if instr->rel && (writes(instr)
                            || (instr is stx && stx_success)
                            || (instr is cas && cas_success))
                    then Release()
                    else if instr is dmb
                    then Fence(instr->mode)
                    else NoOrd())
        &&
        (atomic[last_load, old(step)] == (rmw(instr) || (instr is stx && stx_success) || (instr is cas && cas_success)))
        &&
        (   // external write can clear monitor at any moment. has to set event register.
            (monitor_exclusive == false && event_register == old(monitor_exclusive || event_register))
            || monitor_exclusive == if instr is ldx then true
                else if writes(instr)
                    || instr is stx 
                    || (instr is cas && cas_success)
                then false
                else old(monitor_exclusive))
        &&
        /* D1.6.1 The Event Register
            The Event Register for a PE is set by any of the following:
            • A Send Event instruction, SEV, executed by any PE in the system.
            • A Send Event Local instruction, SEVL, executed by the PE.
            • An exception return.
            • The clearing of the global monitor for the PE.
            • An event from a Generic Timer event stream, see Event streams on page D11-5991.
            • An event sent by some IMPLEMENTATION DEFINED mechanism.
            [ Arm Architecture Reference Manual, version K.a ]
            
            NOTE: since we only care about proving that the event register is set upon reaching wfe, we just allow it to become set non-deterministically.
            But it can be cleared only by wfe.
        */
        (old(event_register) ==> (event_register || instr is wfe))
        &&
        (step == old(step) + 1)
    );
    requires {:msg "either event register or global monitor is set for WFE"}
        instr is wfe ==> event_register || monitor_exclusive;
    requires {:msg "stx is paired to ldx"}
        instr is stx ==> local_monitor == exclusive(instr->addr);


function cbnz(test: bv64): bool {
    test != 0bv64
}

function cbz(test: bv64): bool {
    test == 0bv64
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

function branch(cond: ConditionCode, flags: Flags): bool {(
    var N, Z, C := flags->N, flags->Z, flags->C;
         if cond is EQ then Z
    else if cond is NE then !Z
    else if cond is HS then C
    else if cond is LO then !C
    else if cond is HI then C && !Z
    else if cond is LS then !(C && !Z)
    else false // Should never be reached
)}


function ppo(step1, step2: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect): bool {
    step1 < step2 && (
        // Barrier-ordered-before
        ordering[step1] is Acquire || ordering[step1] is AcquireRelease ||
        ordering[step1] is AcquirePC ||
        ordering[step2] is Release || ordering[step2] is AcquireRelease ||
        (ordering[step1] is Release && ordering[step2] is Acquire) ||
        (exists f : StateIndex :: step1 < f && f < step2 && ordering[f] == Fence(SY())) ||
        (exists f : StateIndex :: step1 < f && f < step2 && ordering[f] == Fence(LD())
            && is_read(effects[step1]))
    )
}


function is_sc(order: Ordering): bool {
    order is Acquire || order is Release || order is AcquireRelease
}
