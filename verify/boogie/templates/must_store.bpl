/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
procedure must_store()
    modifies step, last_load, last_store, #state, #registers;
    ensures {:msg "store happens within function bounds"} (
        old(step) <= last_store && last_store < step
    );
    ensures {:msg "produces a write effect"}
        is_write(effects[last_store]);
    ensures {:msg "basic write or RMW"}
        ((effects[last_store]->write_mask == #value_mask && effects[last_store]->addr == old(#address))
            || atomic[last_load, last_store])
        && bit_and(effects[last_store]->write_value, effects[last_store]->write_mask) == 
                bit_and(align_value(
                    bin_sub(old(#address), effects[last_store]->addr), 
                    old(#input1), 
                    effects[last_load]->read_value, #value_mask), 
                    effects[last_store]->write_mask);    
{
    #implementation
}

