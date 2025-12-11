/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/****************************************************************

        This file offers reusable code across architectures.

        It assumes the following be defined by each architecture / verification target:

            datatype Instruction
                instruction types

            datatype Ordering
                different ordering types, like acquire and release ordering or fences

            function is_sc(orders: Ordering) : bool
                for RCsc
            
            function ppo(i, j: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect): bool 
                defining whether i --ppo-> j with the ordering and effects


        and that assumptions be present in templated code:

            assume sc_impl is ...;
            telling which SC implementation is used (TrailingFence, LeadingFence, RCsc)

        as well as function parameters:

            assume load_order == ...;
            assume fence_order == ...;
            ...

****************************************************************/




// This builtin boogie function returns const array
function {:builtin "MapConst"} ConstArray<T,U>(U): [T]U;

type StateIndex = int;



datatype Effect {
    // read(a,v,vis) == read at a the value v. vis means whether this read is visible to barriers
    read(addr: bv64, read_value: bv64, read_visible: bool),
    write(addr: bv64, write_value, write_mask: bv64),
    update(addr: bv64, read_value: bv64, read_visible: bool, write_value, write_mask: bv64),
    no_effect()
}


function is_effect(effect: Effect) : bool {
    ! (effect is no_effect)
}
function is_read(effect: Effect) : bool {
    effect is read || effect is update
}

function is_write(effect: Effect) : bool {
    effect is write || effect is update
}

var last_load, last_store: StateIndex;
var step: StateIndex;
var atomic: [StateIndex,StateIndex] bool;
var effects: [StateIndex] Effect;
var ordering: [StateIndex] Ordering;





function {:bvbuiltin "bvshl"} shift_left(bv64,bv64) returns(bv64);
function {:bvbuiltin "bvlshr"} shift_right(bv64,bv64) returns(bv64);
function {:bvbuiltin "bvand"} bit_and(bv64,bv64) returns(bv64);
function {:bvbuiltin "bvor"} bit_or(bv64,bv64) returns(bv64);
function {:bvbuiltin "bvxor"} bit_xor(bv64,bv64) returns(bv64);
function {:bvbuiltin "bvnot"} bit_inv(bv64) returns(bv64);
function {:bvbuiltin "bvneg"} bin_neg(bv64) returns(bv64);

function {:bvbuiltin "bvsub"} bin_sub(bv64,bv64) returns(bv64);
function {:bvbuiltin "bvadd"} bin_add(bv64,bv64) returns(bv64);

function {:bvbuiltin "bvule"} ule(bv64,bv64) returns(bool);
function {:bvbuiltin "bvult"} ult(bv64,bv64) returns(bool);
function {:bvbuiltin "bvuge"} uge(bv64,bv64) returns(bool);
function {:bvbuiltin "bvugt"} ugt(bv64,bv64) returns(bool);

function {:bvbuiltin "bvsle"} sle(bv64,bv64) returns(bool);
function {:bvbuiltin "bvslt"} slt(bv64,bv64) returns(bool);
function {:bvbuiltin "bvsge"} sge(bv64,bv64) returns(bool);
function {:bvbuiltin "bvsgt"} sgt(bv64,bv64) returns(bool);



function align_value(address : bv64, value : bv64, old_value : bv64, value_mask: bv64) : bv64 {
    bit_or(
            shift_left(
                bit_and(value, value_mask), 
                shift_left(bit_and(address, 3bv64), 3bv64)),
        bit_and(old_value, bit_inv(shift_left(value_mask, shift_left(bit_and(address, 3bv64), 3bv64)))))
}

function extract_value(address : bv64, value : bv64) : bv64 {
    shift_right(value, 
        shift_left(bit_and(address, 3bv64), 3bv64))
}


const max: [bv64, bv64] bv64;
axiom max == (lambda x, y: bv64 ::
    if uge(x, y) then x else y
);


const min: [bv64, bv64] bv64;
axiom min == (lambda x, y: bv64 ::
    if ule(x, y) then x else y
);


const add: [bv64, bv64] bv64;
axiom add == (lambda x, y: bv64 :: bin_add(x, y));

const sub: [bv64, bv64] bv64;
axiom sub == (lambda x, y: bv64 :: bin_sub(x, y));

const and: [bv64, bv64] bv64;
axiom and == (lambda x, y: bv64 :: bit_and(x, y));

const or: [bv64, bv64] bv64;
axiom or == (lambda x, y: bv64 :: bit_or(x, y));

const xor: [bv64, bv64] bv64;
axiom xor == (lambda x, y: bv64 :: bit_xor(x, y));

function i2b(i:bv64) returns (bool) { i != 0bv64 }
function b2i(b:bool) returns (bv64) { if b then 1bv64 else 0bv64 }


type OrderRelation = [StateIndex, StateIndex, StateIndex, [StateIndex] Ordering, [StateIndex] Effect] bool;

const order_rlx: OrderRelation;
axiom order_rlx == (lambda i, entry, exit: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect ::
    true
);

const order_acq: OrderRelation;
axiom order_acq == (lambda load, entry, exit: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect ::
    (forall i: StateIndex ::
        (i >= exit) && is_effect(effects[i]) ==> ppo(load, i, ordering, effects)
    )
);

const order_rel: OrderRelation;
axiom order_rel == (lambda store, entry, exit: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect ::
    (forall i: StateIndex ::
        (i < entry) && is_effect(effects[i]) ==> ppo(i, store, ordering, effects)
    )
);

datatype SCImplementation { LeadingFence(), TrailingFence(), Mixed(), RCsc() }
const sc_impl: SCImplementation;

const order_acq_sc: OrderRelation;
axiom order_acq_sc == (lambda load, entry, exit: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect ::
    order_acq[load, entry, exit, ordering, effects] &&
    if sc_impl is LeadingFence then
        // ordered with all previous operations
        (forall i: StateIndex ::
            (i < entry) && is_effect(effects[i]) ==> ppo(i, load, ordering, effects)
        )
    else if sc_impl is TrailingFence then
        true
    else if sc_impl is RCsc then
        // ordered with all previous SC operations
        is_sc(ordering[load])
    else if sc_impl is Mixed then
        is_sc(ordering[load]) 
        || (forall i: StateIndex ::
            (i < entry) && is_effect(effects[i]) ==> ppo(i, load, ordering, effects)
        )
    else false
);

const order_rel_sc: OrderRelation;
axiom order_rel_sc == (lambda store, entry, exit: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect ::
    order_rel[store, entry, exit, ordering, effects] &&
    if sc_impl is LeadingFence then
        true
    else if sc_impl is TrailingFence then
        // ordered with all later operations
        (forall i: StateIndex ::
            (i >= exit) && is_effect(effects[i]) ==> ppo(store, i, ordering, effects)
        )
    else if sc_impl is RCsc then
        // ordered with all later SC operations
        is_sc(ordering[store])
    else if sc_impl is Mixed then
        is_sc(ordering[store]) 
        || (forall i: StateIndex ::
            (i >= exit) && is_effect(effects[i]) ==> ppo(store, i, ordering, effects)
        )
    else false
);


const order_fence_acq: OrderRelation;
axiom order_fence_acq == (lambda fence, entry, exit: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect ::
        (forall i, j: StateIndex ::
            (i < entry) && (j >= exit) && (is_read(effects[i]) && effects[i]->read_visible) && is_effect(effects[j])
                ==> ppo(i, j, ordering, effects))
);

const order_fence_rel: OrderRelation;
axiom order_fence_rel == (lambda fence, entry, exit: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect ::
        (forall i, j: StateIndex ::
            (i < entry) && (j >= exit) && is_effect(effects[i]) && is_write(effects[j])
                ==> ppo(i, j, ordering, effects))
);
const order_fence_sc: OrderRelation;
axiom order_fence_sc == (lambda fence, entry, exit: StateIndex, ordering: [StateIndex] Ordering, effects: [StateIndex] Effect ::
        (forall i, j: StateIndex ::
            (i < entry) && (j >= exit) && is_effect(effects[i]) && is_effect(effects[j])
                ==> ppo(i, j, ordering, effects))
);



function no_writes(from, to, write: StateIndex): bool {
    (write < from || to <= write)
}

function valid_mask(val, mask : bv64) : bool {
    val == bit_and(val, mask)
}


type RMWOp = [bv64, bv64, bv64] bv64;

const cmpset, add_op, sub_op, set_op, min_op, max_op, dec_op, inc_op, ret_old, xor_op, and_op, or_op: RMWOp;

axiom cmpset == (lambda x, y1, y2 : bv64 :: if x == y1 then y2 else x);
axiom add_op == (lambda x, y, _: bv64 :: bin_add(x, y));
axiom sub_op == (lambda x, y, _: bv64 :: bin_sub(x, y));
axiom set_op == (lambda x, y, _: bv64 :: y);
axiom min_op == (lambda x, y, _: bv64 :: min[x, y]);
axiom max_op == (lambda x, y, _: bv64 :: max[x, y]);
axiom dec_op == (lambda x, _1, _2: bv64 :: bin_sub(x, 1bv64));
axiom inc_op == (lambda x, _1, _2: bv64 :: bin_add(x, 1bv64));
axiom and_op == (lambda x, y, _ : bv64 :: bit_and(x, y));
axiom or_op == (lambda x, y, _ : bv64 :: bit_or(x, y));
axiom xor_op == (lambda x, y, _ : bv64 :: bit_xor(x, y));

axiom ret_old == (lambda x, _1, _2 : bv64 :: x);

const bit8, bit16 : [RMWOp] RMWOp;
axiom bit8 == (lambda op : RMWOp :: (lambda x, y1, y2 : bv64 :: op[x, bit_and(y1,255bv64), bit_and(y2, 255bv64)]));
axiom bit16 == (lambda op : RMWOp :: (lambda x, y1, y2 : bv64 :: op[x, bit_and(y1,65535bv64), bit_and(y2, 65535bv64)]));

type AwaitOp = [bv64, bv64] bool;

const eq, neq, lt, le, gt, ge: AwaitOp;

axiom eq  == (lambda x, y: bv64 :: x == y);
axiom neq == (lambda x, y: bv64 :: x != y);
axiom lt  == (lambda x, y: bv64 :: ult(x, y));
axiom le  == (lambda x, y: bv64 :: ule(x, y));
axiom gt  == (lambda x, y: bv64 :: ugt(x, y));
axiom ge  == (lambda x, y: bv64 :: uge(x, y));