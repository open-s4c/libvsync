/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
datatype Ordering {
    Acquire(),
    Release(),
    AcquirePC(),
    ReleasePC(),
    AcqRel(),
    Fence(ra, wa, rb, wb: bool),
    NoOrd()
}

datatype AtomicType {
    AtomicAdd(),
    AtomicAnd(),
    AtomicMax(),
    AtomicMin(),
    AtomicOr(),
    AtomicXor(),
    AtomicSwap()
}

datatype Monitor {
    exclusive(addr: bv64),
    open()
}

var local_monitor: Monitor;
var monitor_exclusive: bool;

datatype Instruction {
    ld(addr: bv64, mask: bv64),
    ldu(addr: bv64, mask: bv64),
    sd(src, addr: bv64, mask: bv64),
    sb(src, addr: bv64, mask: bv64),
    lr(acq, rel: bool, addr: bv64, mask: bv64),
    sc(acq, rel: bool, src, addr: bv64, mask: bv64),
    mv(src: bv64),
    atomic(atom: AtomicType, acq, rel: bool, src, addr: bv64, mask: bv64),

    add(first, second: bv64),
    addi(first, second: bv64),
    sub(first, second: bv64),
    neg(src: bv64),

    andd(first, second: bv64),
    orr(first, second: bv64),
    and(first, second: bv64),
    or(first, second: bv64),
    xor(first, second: bv64),

    negw(src: bv64),

    andi(first, second: bv64),
    slli(first, second: bv64),
    sll(first, second: bv64),
    li(src: bv64),
    not(src: bv64),


    srli(first, second: bv64),
    srl(first, second: bv64),
    sra(first, second: bv64),

    sext(src: bv64),


    fence(ra, wa, rb, wb: bool)
}


function updated_value(instr: Instruction, read_value : bv64) : bv64 {
    if instr is sc then instr->src 
    else if instr->atom is AtomicAdd then bin_add(instr->src, read_value)
    else if instr->atom is AtomicAnd then and[instr->src, read_value]
    else if instr->atom is AtomicMax then max[instr->src, read_value]
    else if instr->atom is AtomicMin then min[instr->src, read_value]
    else if instr->atom is AtomicOr then or[instr->src, read_value]
    else if instr->atom is AtomicXor then xor[instr->src, read_value]
    else if instr->atom is AtomicSwap then instr->src
    else 0bv64
}

function rmw(instr: Instruction) : bool {
    instr is atomic
}

function reads(instr: Instruction) : bool {
    rmw(instr) || instr is ld || instr is ldu || instr is lr
}


function returning_load(instr : Instruction) : bool { reads(instr) }

function instruction_mask(instr: Instruction) : bv64 {
    instr->mask
}

function writes(instr: Instruction) : bool {
    rmw(instr) || instr is sd || instr is sb
}


procedure execute_local(instr: Instruction) returns (r : bv64);
    ensures
        (r == if instr is mv || instr is sext || instr is li then instr->src
            else if instr is add || instr is addi then bin_add(instr->first, instr->second)
            else if instr is sub then bin_sub(instr->first, instr->second)
            else if instr is neg || instr is negw then bin_neg(instr->src)
            else if instr is slli || instr is sll then shift_left(instr->first, instr->second)

            /* realistically, sra and srl behave differently - srl on unsigned, sra on signed */
            else if instr is srli || instr is srl || instr is sra then shift_right(instr->first, instr->second)

            else if instr is not then bit_inv(instr->src)
            else if instr is andd || instr is and || instr is andi then bit_and(instr->first, instr->second)
            else if instr is orr || instr is or then  bit_or(instr->first, instr->second)
            else if instr is xor then  bit_xor(instr->first, instr->second)
            else bit_and(r, instruction_mask(instr)));


procedure assume_requires_execute(instr: Instruction);
    modifies step, local_monitor, monitor_exclusive, last_store, last_load;
    ensures (instr is sc ==> local_monitor is exclusive && local_monitor->addr == instr->addr);

procedure execute(instr: Instruction) returns (r : bv64);
    modifies step, local_monitor, monitor_exclusive, last_store, last_load;
    requires (instr is sc ==> local_monitor is exclusive && local_monitor->addr == instr->addr);
    ensures step == old(step + 1);
    ensures (
        var sc_success :=
                    old(local_monitor is exclusive
                        && (local_monitor->addr == instr->addr)
                        && monitor_exclusive);
        (r == if instr is mv || instr is sext || instr is li then instr->src
            else if instr is sc then b2i(!sc_success)
            else bit_and(r, instruction_mask(instr)))
        &&
        (last_load ==
                    if reads(instr)
                    then
                        old(step)
                    else
                        old(last_load))
        && 
        (last_store == 
                    if writes(instr) || (instr is sc && sc_success)
                    then
                        old(step)
                    else
                        old(last_store))
        &&
        (local_monitor == 
                    if instr is lr then
                        exclusive(instr->addr)
                    else if writes(instr) || reads(instr) || instr is sc then
                        open()
                    else
                        old(local_monitor))
        &&
        (effects[old(step)] == if rmw(instr) || (instr is sc && sc_success)
            then update(instr->addr, r, true, updated_value(instr, r), instruction_mask(instr))
            else if reads(instr)
            then read(instr->addr, r, true)
            else if writes(instr) 
            then write(instr->addr, instr->src, instruction_mask(instr))
            else no_effect())
        &&
        (ordering[old(step)] ==
            if instr->acq && instr->rel
                && (instr is lr
                    || (instr is sc && sc_success)
                    || rmw(instr))
            then AcqRel()
            else if instr->acq && reads(instr)
            then Acquire()
            else if (instr->rel && writes(instr))
                    || (instr is sc && sc_success)
            then Release()
            else if instr is fence
            then Fence(instr->ra, instr->wa, instr->rb, instr->wb) 
            else NoOrd())
        &&
        ((
            monitor_exclusive == false  // external write can clear monitor at any moment
        )
        || monitor_exclusive == old(
            if instr is lr then
                true
            else if writes(instr) || instr is sc then
                false
            else
                monitor_exclusive
        ))
        &&
        (atomic[last_load, old(step)] == (rmw(instr) || (instr is sc && sc_success)))
    );

function beq(r1: bv64, r2:bv64): bool {
    r1 == r2
}

function bne(r1: bv64, r2:bv64): bool {
    r1 != r2
}

function bnez(r: bv64): bool {
    r != 0bv64
}


function bgt(r1, r2: bv64): bool {
    sgt(r1, r2)
}

function bgtu(r1, r2: bv64): bool {
    ugt(r1, r2)
}

function ble(r1, r2: bv64): bool {
    sle(r1, r2)
}

function bleu(r1, r2: bv64): bool {
    ule(r1, r2)
}


function blt(r1, r2: bv64): bool {
    slt(r1, r2)
}

function bltu(r1, r2: bv64): bool {
    ult(r1, r2)
}

function bge(r1, r2: bv64): bool {
    sge(r1, r2)
}

function bgeu(r1, r2: bv64): bool {
    uge(r1, r2)
}


function is_acq(order: Ordering) : bool {
    order is Acquire || order is AcqRel || order is AcquirePC
}

function is_rel(order: Ordering) : bool {
    order is Release || order is AcqRel || order is ReleasePC
}


function ppo(step1, step2: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect): bool {
    step1 < step2 && (
        // Barrier-ordered-before
        is_acq(ordering[step1]) ||
        is_rel(ordering[step2]) ||
        (ordering[step1] == Release() && ordering[step2] == Acquire()) ||

        (exists fenceId: StateIndex, fence: Ordering, e1, e2: Effect :: 
            fence is Fence && ordering[fenceId] == fence && effects[step1] == e1 && effects[step2] == e2 &&
            (step1 < fenceId && fenceId < step2) &&
            ((fence->ra && is_read(e1)) ||
             (fence->wa && is_write(e1))
            ) && 
            ((fence->rb && is_read(e2)) ||
             (fence->wb && is_write(e2))
            )
        )
    )
}

function is_sc(order: Ordering): bool {
    order is Acquire || order is Release
}