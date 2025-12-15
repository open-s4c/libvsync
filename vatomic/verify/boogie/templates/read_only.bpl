/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
procedure read_only()
    modifies step, last_load, last_store, #state, #registers;
    ensures no_writes(old(step), step, last_store);
    ensures {:msg "produced read effects are correct"}
        old(step) <= last_load && last_load < step ==> effects[last_load] == read(old(#address), #output, true);
{
    #implementation
}