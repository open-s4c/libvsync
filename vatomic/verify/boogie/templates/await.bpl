/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
procedure await(cond : AwaitOp)
    modifies step, last_load, last_store, #state, #registers;

    ensures {:msg "satisfy await condition"}
        cond[effects[last_load]->read_value, old(#input1)];
{
    #implementation
}
