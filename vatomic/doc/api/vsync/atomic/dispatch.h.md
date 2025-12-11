#  [vsync](../README.md) / [atomic](README.md) / dispatch.h
_Set of macros to dispatch atomic functions._ 

These macros select the atomic function variant based on the argument type, for example, [vatomic_read(a)](dispatch.h.md#macro-vatomic_read) calls vatomic32_read(a) if `a` is a vatomic32_t and calls vatomic64_read(a) if `a` is a vatomic64_t. vatomicptr_t is mapped to either 32 or 64 bits functions depending on the architecture. 

---
# Macros 

| Macro | Description |
|---|---|
| [vatomic_init](dispatch.h.md#macro-vatomic_init) | Dispatches init calls with configurable memory order.  |
| [vatomic_read](dispatch.h.md#macro-vatomic_read) | Dispatches read calls with seq_cst memory order.  |
| [vatomic_read_acq](dispatch.h.md#macro-vatomic_read_acq) | Dispatches read calls with acquire memory order.  |
| [vatomic_read_rlx](dispatch.h.md#macro-vatomic_read_rlx) | Dispatches read calls with relaxed memory order.  |
| [vatomic_write](dispatch.h.md#macro-vatomic_write) | Dispatches write calls with seq_cst memory order.  |
| [vatomic_write_rel](dispatch.h.md#macro-vatomic_write_rel) | Dispatches write calls with release memory order.  |
| [vatomic_write_rlx](dispatch.h.md#macro-vatomic_write_rlx) | Dispatches write calls with relaxed memory order.  |
| [vatomic_xchg](dispatch.h.md#macro-vatomic_xchg) | Dispatches xchg calls with seq_cst memory order.  |
| [vatomic_xchg_acq](dispatch.h.md#macro-vatomic_xchg_acq) | Dispatches xchg calls with acquire memory order.  |
| [vatomic_xchg_rel](dispatch.h.md#macro-vatomic_xchg_rel) | Dispatches xchg calls with release memory order.  |
| [vatomic_xchg_rlx](dispatch.h.md#macro-vatomic_xchg_rlx) | Dispatches xchg calls with relaxed memory order.  |
| [vatomic_cmpxchg](dispatch.h.md#macro-vatomic_cmpxchg) | Dispatches cmpxchg calls with seq_cst memory order.  |
| [vatomic_cmpxchg_acq](dispatch.h.md#macro-vatomic_cmpxchg_acq) | Dispatches cmpxchg calls with acquire memory order.  |
| [vatomic_cmpxchg_rel](dispatch.h.md#macro-vatomic_cmpxchg_rel) | Dispatches cmpxchg calls with release memory order.  |
| [vatomic_cmpxchg_rlx](dispatch.h.md#macro-vatomic_cmpxchg_rlx) | Dispatches cmpxchg calls with relaxed memory order.  |
| [vatomic_inc](dispatch.h.md#macro-vatomic_inc) | Dispatches inc calls with seq_cst memory order.  |
| [vatomic_inc_rel](dispatch.h.md#macro-vatomic_inc_rel) | Dispatches inc calls with release memory order.  |
| [vatomic_inc_rlx](dispatch.h.md#macro-vatomic_inc_rlx) | Dispatches inc calls with relaxed memory order.  |
| [vatomic_dec](dispatch.h.md#macro-vatomic_dec) | Dispatches dec calls with seq_cst memory order.  |
| [vatomic_dec_rel](dispatch.h.md#macro-vatomic_dec_rel) | Dispatches dec calls with release memory order.  |
| [vatomic_dec_rlx](dispatch.h.md#macro-vatomic_dec_rlx) | Dispatches dec calls with relaxed memory order.  |
| [vatomic_inc_get](dispatch.h.md#macro-vatomic_inc_get) | Dispatches inc_get calls with seq_cst memory order.  |
| [vatomic_inc_get_acq](dispatch.h.md#macro-vatomic_inc_get_acq) | Dispatches inc_get calls with acquire memory order.  |
| [vatomic_inc_get_rel](dispatch.h.md#macro-vatomic_inc_get_rel) | Dispatches inc_get calls with release memory order.  |
| [vatomic_inc_get_rlx](dispatch.h.md#macro-vatomic_inc_get_rlx) | Dispatches inc_get calls with relaxed memory order.  |
| [vatomic_dec_get](dispatch.h.md#macro-vatomic_dec_get) | Dispatches dec_get calls with seq_cst memory order.  |
| [vatomic_dec_get_acq](dispatch.h.md#macro-vatomic_dec_get_acq) | Dispatches dec_get calls with acquire memory order.  |
| [vatomic_dec_get_rel](dispatch.h.md#macro-vatomic_dec_get_rel) | Dispatches dec_get calls with release memory order.  |
| [vatomic_dec_get_rlx](dispatch.h.md#macro-vatomic_dec_get_rlx) | Dispatches dec_get calls with relaxed memory order.  |
| [vatomic_get_inc](dispatch.h.md#macro-vatomic_get_inc) | Dispatches get_inc calls with seq_cst memory order.  |
| [vatomic_get_inc_acq](dispatch.h.md#macro-vatomic_get_inc_acq) | Dispatches get_inc calls with acquire memory order.  |
| [vatomic_get_inc_rel](dispatch.h.md#macro-vatomic_get_inc_rel) | Dispatches get_inc calls with release memory order.  |
| [vatomic_get_inc_rlx](dispatch.h.md#macro-vatomic_get_inc_rlx) | Dispatches get_inc calls with relaxed memory order.  |
| [vatomic_get_dec](dispatch.h.md#macro-vatomic_get_dec) | Dispatches get_dec calls with seq_cst memory order.  |
| [vatomic_get_dec_acq](dispatch.h.md#macro-vatomic_get_dec_acq) | Dispatches get_dec calls with acquire memory order.  |
| [vatomic_get_dec_rel](dispatch.h.md#macro-vatomic_get_dec_rel) | Dispatches get_dec calls with release memory order.  |
| [vatomic_get_dec_rlx](dispatch.h.md#macro-vatomic_get_dec_rlx) | Dispatches get_dec calls with relaxed memory order.  |
| [vatomic_add](dispatch.h.md#macro-vatomic_add) | Dispatches add calls with seq_cst memory order.  |
| [vatomic_add_rel](dispatch.h.md#macro-vatomic_add_rel) | Dispatches add calls with release memory order.  |
| [vatomic_add_rlx](dispatch.h.md#macro-vatomic_add_rlx) | Dispatches add calls with relaxed memory order.  |
| [vatomic_sub](dispatch.h.md#macro-vatomic_sub) | Dispatches sub calls with seq_cst memory order.  |
| [vatomic_sub_rel](dispatch.h.md#macro-vatomic_sub_rel) | Dispatches sub calls with release memory order.  |
| [vatomic_sub_rlx](dispatch.h.md#macro-vatomic_sub_rlx) | Dispatches sub calls with relaxed memory order.  |
| [vatomic_and](dispatch.h.md#macro-vatomic_and) | Dispatches and calls with seq_cst memory order.  |
| [vatomic_and_rel](dispatch.h.md#macro-vatomic_and_rel) | Dispatches and calls with release memory order.  |
| [vatomic_and_rlx](dispatch.h.md#macro-vatomic_and_rlx) | Dispatches and calls with relaxed memory order.  |
| [vatomic_xor](dispatch.h.md#macro-vatomic_xor) | Dispatches xor calls with seq_cst memory order.  |
| [vatomic_xor_rel](dispatch.h.md#macro-vatomic_xor_rel) | Dispatches xor calls with release memory order.  |
| [vatomic_xor_rlx](dispatch.h.md#macro-vatomic_xor_rlx) | Dispatches xor calls with relaxed memory order.  |
| [vatomic_or](dispatch.h.md#macro-vatomic_or) | Dispatches or calls with seq_cst memory order.  |
| [vatomic_or_rel](dispatch.h.md#macro-vatomic_or_rel) | Dispatches or calls with release memory order.  |
| [vatomic_or_rlx](dispatch.h.md#macro-vatomic_or_rlx) | Dispatches or calls with relaxed memory order.  |
| [vatomic_max](dispatch.h.md#macro-vatomic_max) | Dispatches max calls with seq_cst memory order.  |
| [vatomic_max_rel](dispatch.h.md#macro-vatomic_max_rel) | Dispatches max calls with release memory order.  |
| [vatomic_max_rlx](dispatch.h.md#macro-vatomic_max_rlx) | Dispatches max calls with relaxed memory order.  |
| [vatomic_add_get](dispatch.h.md#macro-vatomic_add_get) | Dispatches add_get calls with seq_cst memory order.  |
| [vatomic_add_get_acq](dispatch.h.md#macro-vatomic_add_get_acq) | Dispatches add_get calls with acquire memory order.  |
| [vatomic_add_get_rel](dispatch.h.md#macro-vatomic_add_get_rel) | Dispatches add_get calls with release memory order.  |
| [vatomic_add_get_rlx](dispatch.h.md#macro-vatomic_add_get_rlx) | Dispatches add_get calls with relaxed memory order.  |
| [vatomic_sub_get](dispatch.h.md#macro-vatomic_sub_get) | Dispatches sub_get calls with seq_cst memory order.  |
| [vatomic_sub_get_acq](dispatch.h.md#macro-vatomic_sub_get_acq) | Dispatches sub_get calls with acquire memory order.  |
| [vatomic_sub_get_rel](dispatch.h.md#macro-vatomic_sub_get_rel) | Dispatches sub_get calls with release memory order.  |
| [vatomic_sub_get_rlx](dispatch.h.md#macro-vatomic_sub_get_rlx) | Dispatches sub_get calls with relaxed memory order.  |
| [vatomic_and_get](dispatch.h.md#macro-vatomic_and_get) | Dispatches and_get calls with seq_cst memory order.  |
| [vatomic_and_get_acq](dispatch.h.md#macro-vatomic_and_get_acq) | Dispatches and_get calls with acquire memory order.  |
| [vatomic_and_get_rel](dispatch.h.md#macro-vatomic_and_get_rel) | Dispatches and_get calls with release memory order.  |
| [vatomic_and_get_rlx](dispatch.h.md#macro-vatomic_and_get_rlx) | Dispatches and_get calls with relaxed memory order.  |
| [vatomic_xor_get](dispatch.h.md#macro-vatomic_xor_get) | Dispatches xor_get calls with seq_cst memory order.  |
| [vatomic_xor_get_acq](dispatch.h.md#macro-vatomic_xor_get_acq) | Dispatches xor_get calls with acquire memory order.  |
| [vatomic_xor_get_rel](dispatch.h.md#macro-vatomic_xor_get_rel) | Dispatches xor_get calls with release memory order.  |
| [vatomic_xor_get_rlx](dispatch.h.md#macro-vatomic_xor_get_rlx) | Dispatches xor_get calls with relaxed memory order.  |
| [vatomic_or_get](dispatch.h.md#macro-vatomic_or_get) | Dispatches or_get calls with seq_cst memory order.  |
| [vatomic_or_get_acq](dispatch.h.md#macro-vatomic_or_get_acq) | Dispatches or_get calls with acquire memory order.  |
| [vatomic_or_get_rel](dispatch.h.md#macro-vatomic_or_get_rel) | Dispatches or_get calls with release memory order.  |
| [vatomic_or_get_rlx](dispatch.h.md#macro-vatomic_or_get_rlx) | Dispatches or_get calls with relaxed memory order.  |
| [vatomic_max_get](dispatch.h.md#macro-vatomic_max_get) | Dispatches max_get calls with seq_cst memory order.  |
| [vatomic_max_get_acq](dispatch.h.md#macro-vatomic_max_get_acq) | Dispatches max_get calls with acquire memory order.  |
| [vatomic_max_get_rel](dispatch.h.md#macro-vatomic_max_get_rel) | Dispatches max_get calls with release memory order.  |
| [vatomic_max_get_rlx](dispatch.h.md#macro-vatomic_max_get_rlx) | Dispatches max_get calls with relaxed memory order.  |
| [vatomic_get_add](dispatch.h.md#macro-vatomic_get_add) | Dispatches get_add calls with seq_cst memory order.  |
| [vatomic_get_sub](dispatch.h.md#macro-vatomic_get_sub) | Dispatches get_sub calls with seq_cst memory order.  |
| [vatomic_get_and](dispatch.h.md#macro-vatomic_get_and) | Dispatches get_and calls with seq_cst memory order.  |
| [vatomic_get_xor](dispatch.h.md#macro-vatomic_get_xor) | Dispatches get_xor calls with seq_cst memory order.  |
| [vatomic_get_or](dispatch.h.md#macro-vatomic_get_or) | Dispatches get_or calls with seq_cst memory order.  |
| [vatomic_get_max](dispatch.h.md#macro-vatomic_get_max) | Dispatches get_max calls with seq_cst memory order.  |
| [vatomic_await_eq](dispatch.h.md#macro-vatomic_await_eq) | Dispatches await_eq calls with seq_cst memory order.  |
| [vatomic_await_eq_acq](dispatch.h.md#macro-vatomic_await_eq_acq) | Dispatches await_eq calls with acquire memory order.  |
| [vatomic_await_eq_rlx](dispatch.h.md#macro-vatomic_await_eq_rlx) | Dispatches await_eq calls with relaxed memory order.  |
| [vatomic_await_neq](dispatch.h.md#macro-vatomic_await_neq) | Dispatches await_neq calls with seq_cst memory order.  |
| [vatomic_await_neq_acq](dispatch.h.md#macro-vatomic_await_neq_acq) | Dispatches await_neq calls with acquire memory order.  |
| [vatomic_await_neq_rlx](dispatch.h.md#macro-vatomic_await_neq_rlx) | Dispatches await_neq calls with relaxed memory order.  |
| [vatomic_await_le](dispatch.h.md#macro-vatomic_await_le) | Dispatches await_le calls with seq_cst memory order.  |
| [vatomic_await_le_acq](dispatch.h.md#macro-vatomic_await_le_acq) | Dispatches await_le calls with acquire memory order.  |
| [vatomic_await_le_rlx](dispatch.h.md#macro-vatomic_await_le_rlx) | Dispatches await_le calls with relaxed memory order.  |
| [vatomic_await_lt](dispatch.h.md#macro-vatomic_await_lt) | Dispatches await_lt calls with seq_cst memory order.  |
| [vatomic_await_lt_acq](dispatch.h.md#macro-vatomic_await_lt_acq) | Dispatches await_lt calls with acquire memory order.  |
| [vatomic_await_lt_rlx](dispatch.h.md#macro-vatomic_await_lt_rlx) | Dispatches await_lt calls with relaxed memory order.  |
| [vatomic_await_gt](dispatch.h.md#macro-vatomic_await_gt) | Dispatches await_gt calls with seq_cst memory order.  |
| [vatomic_await_gt_acq](dispatch.h.md#macro-vatomic_await_gt_acq) | Dispatches await_gt calls with acquire memory order.  |
| [vatomic_await_gt_rlx](dispatch.h.md#macro-vatomic_await_gt_rlx) | Dispatches await_gt calls with relaxed memory order.  |
| [vatomic_await_ge](dispatch.h.md#macro-vatomic_await_ge) | Dispatches await_ge calls with seq_cst memory order.  |
| [vatomic_await_ge_acq](dispatch.h.md#macro-vatomic_await_ge_acq) | Dispatches await_ge calls with acquire memory order.  |
| [vatomic_await_ge_rlx](dispatch.h.md#macro-vatomic_await_ge_rlx) | Dispatches await_ge calls with relaxed memory order.  |
| [vatomic_await_eq_set](dispatch.h.md#macro-vatomic_await_eq_set) | Dispatches await_eq_set calls with seq_cst memory order.  |
| [vatomic_await_eq_set_acq](dispatch.h.md#macro-vatomic_await_eq_set_acq) | Dispatches await_eq_set calls with acquire memory order.  |
| [vatomic_await_eq_set_rel](dispatch.h.md#macro-vatomic_await_eq_set_rel) | Dispatches await_eq_set calls with release memory order.  |
| [vatomic_await_eq_set_rlx](dispatch.h.md#macro-vatomic_await_eq_set_rlx) | Dispatches await_eq_set calls with relaxed memory order.  |
| [vatomic_await_eq_sub](dispatch.h.md#macro-vatomic_await_eq_sub) | Dispatches await_eq_sub calls with seq_cst memory order.  |
| [vatomic_await_eq_sub_acq](dispatch.h.md#macro-vatomic_await_eq_sub_acq) | Dispatches await_eq_sub calls with acquire memory order.  |
| [vatomic_await_eq_sub_rel](dispatch.h.md#macro-vatomic_await_eq_sub_rel) | Dispatches await_eq_sub calls with release memory order.  |
| [vatomic_await_eq_sub_rlx](dispatch.h.md#macro-vatomic_await_eq_sub_rlx) | Dispatches await_eq_sub calls with relaxed memory order.  |
| [vatomic_await_eq_add](dispatch.h.md#macro-vatomic_await_eq_add) | Dispatches await_eq_add calls with seq_cst memory order.  |
| [vatomic_await_eq_add_acq](dispatch.h.md#macro-vatomic_await_eq_add_acq) | Dispatches await_eq_add calls with acquire memory order.  |
| [vatomic_await_eq_add_rel](dispatch.h.md#macro-vatomic_await_eq_add_rel) | Dispatches await_eq_add calls with release memory order.  |
| [vatomic_await_eq_add_rlx](dispatch.h.md#macro-vatomic_await_eq_add_rlx) | Dispatches await_eq_add calls with relaxed memory order.  |
| [vatomic_await_neq_set](dispatch.h.md#macro-vatomic_await_neq_set) | Dispatches await_neq_set calls with seq_cst memory order.  |
| [vatomic_await_neq_set_acq](dispatch.h.md#macro-vatomic_await_neq_set_acq) | Dispatches await_neq_set calls with acquire memory order.  |
| [vatomic_await_neq_set_rel](dispatch.h.md#macro-vatomic_await_neq_set_rel) | Dispatches await_neq_set calls with release memory order.  |
| [vatomic_await_neq_set_rlx](dispatch.h.md#macro-vatomic_await_neq_set_rlx) | Dispatches await_neq_set calls with relaxed memory order.  |
| [vatomic_await_neq_sub](dispatch.h.md#macro-vatomic_await_neq_sub) | Dispatches await_neq_sub calls with seq_cst memory order.  |
| [vatomic_await_neq_sub_acq](dispatch.h.md#macro-vatomic_await_neq_sub_acq) | Dispatches await_neq_sub calls with acquire memory order.  |
| [vatomic_await_neq_sub_rel](dispatch.h.md#macro-vatomic_await_neq_sub_rel) | Dispatches await_neq_sub calls with release memory order.  |
| [vatomic_await_neq_sub_rlx](dispatch.h.md#macro-vatomic_await_neq_sub_rlx) | Dispatches await_neq_sub calls with relaxed memory order.  |
| [vatomic_await_neq_add](dispatch.h.md#macro-vatomic_await_neq_add) | Dispatches await_neq_add calls with seq_cst memory order.  |
| [vatomic_await_neq_add_acq](dispatch.h.md#macro-vatomic_await_neq_add_acq) | Dispatches await_neq_add calls with acquire memory order.  |
| [vatomic_await_neq_add_rel](dispatch.h.md#macro-vatomic_await_neq_add_rel) | Dispatches await_neq_add calls with release memory order.  |
| [vatomic_await_neq_add_rlx](dispatch.h.md#macro-vatomic_await_neq_add_rlx) | Dispatches await_neq_add calls with relaxed memory order.  |
| [vatomic_await_le_set](dispatch.h.md#macro-vatomic_await_le_set) | Dispatches await_le_set calls with seq_cst memory order.  |
| [vatomic_await_le_set_acq](dispatch.h.md#macro-vatomic_await_le_set_acq) | Dispatches await_le_set calls with acquire memory order.  |
| [vatomic_await_le_set_rel](dispatch.h.md#macro-vatomic_await_le_set_rel) | Dispatches await_le_set calls with release memory order.  |
| [vatomic_await_le_set_rlx](dispatch.h.md#macro-vatomic_await_le_set_rlx) | Dispatches await_le_set calls with relaxed memory order.  |
| [vatomic_await_le_sub](dispatch.h.md#macro-vatomic_await_le_sub) | Dispatches await_le_sub calls with seq_cst memory order.  |
| [vatomic_await_le_sub_acq](dispatch.h.md#macro-vatomic_await_le_sub_acq) | Dispatches await_le_sub calls with acquire memory order.  |
| [vatomic_await_le_sub_rel](dispatch.h.md#macro-vatomic_await_le_sub_rel) | Dispatches await_le_sub calls with release memory order.  |
| [vatomic_await_le_sub_rlx](dispatch.h.md#macro-vatomic_await_le_sub_rlx) | Dispatches await_le_sub calls with relaxed memory order.  |
| [vatomic_await_le_add](dispatch.h.md#macro-vatomic_await_le_add) | Dispatches await_le_add calls with seq_cst memory order.  |
| [vatomic_await_le_add_acq](dispatch.h.md#macro-vatomic_await_le_add_acq) | Dispatches await_le_add calls with acquire memory order.  |
| [vatomic_await_le_add_rel](dispatch.h.md#macro-vatomic_await_le_add_rel) | Dispatches await_le_add calls with release memory order.  |
| [vatomic_await_le_add_rlx](dispatch.h.md#macro-vatomic_await_le_add_rlx) | Dispatches await_le_add calls with relaxed memory order.  |
| [vatomic_await_lt_set](dispatch.h.md#macro-vatomic_await_lt_set) | Dispatches await_lt_set calls with seq_cst memory order.  |
| [vatomic_await_lt_set_acq](dispatch.h.md#macro-vatomic_await_lt_set_acq) | Dispatches await_lt_set calls with acquire memory order.  |
| [vatomic_await_lt_set_rel](dispatch.h.md#macro-vatomic_await_lt_set_rel) | Dispatches await_lt_set calls with release memory order.  |
| [vatomic_await_lt_set_rlx](dispatch.h.md#macro-vatomic_await_lt_set_rlx) | Dispatches await_lt_set calls with relaxed memory order.  |
| [vatomic_await_lt_sub](dispatch.h.md#macro-vatomic_await_lt_sub) | Dispatches await_lt_sub calls with seq_cst memory order.  |
| [vatomic_await_lt_sub_acq](dispatch.h.md#macro-vatomic_await_lt_sub_acq) | Dispatches await_lt_sub calls with acquire memory order.  |
| [vatomic_await_lt_sub_rel](dispatch.h.md#macro-vatomic_await_lt_sub_rel) | Dispatches await_lt_sub calls with release memory order.  |
| [vatomic_await_lt_sub_rlx](dispatch.h.md#macro-vatomic_await_lt_sub_rlx) | Dispatches await_lt_sub calls with relaxed memory order.  |
| [vatomic_await_lt_add](dispatch.h.md#macro-vatomic_await_lt_add) | Dispatches await_lt_add calls with seq_cst memory order.  |
| [vatomic_await_lt_add_acq](dispatch.h.md#macro-vatomic_await_lt_add_acq) | Dispatches await_lt_add calls with acquire memory order.  |
| [vatomic_await_lt_add_rel](dispatch.h.md#macro-vatomic_await_lt_add_rel) | Dispatches await_lt_add calls with release memory order.  |
| [vatomic_await_lt_add_rlx](dispatch.h.md#macro-vatomic_await_lt_add_rlx) | Dispatches await_lt_add calls with relaxed memory order.  |
| [vatomic_await_gt_set](dispatch.h.md#macro-vatomic_await_gt_set) | Dispatches await_gt_set calls with seq_cst memory order.  |
| [vatomic_await_gt_set_acq](dispatch.h.md#macro-vatomic_await_gt_set_acq) | Dispatches await_gt_set calls with acquire memory order.  |
| [vatomic_await_gt_set_rel](dispatch.h.md#macro-vatomic_await_gt_set_rel) | Dispatches await_gt_set calls with release memory order.  |
| [vatomic_await_gt_set_rlx](dispatch.h.md#macro-vatomic_await_gt_set_rlx) | Dispatches await_gt_set calls with relaxed memory order.  |
| [vatomic_await_gt_sub](dispatch.h.md#macro-vatomic_await_gt_sub) | Dispatches await_gt_sub calls with seq_cst memory order.  |
| [vatomic_await_gt_sub_acq](dispatch.h.md#macro-vatomic_await_gt_sub_acq) | Dispatches await_gt_sub calls with acquire memory order.  |
| [vatomic_await_gt_sub_rel](dispatch.h.md#macro-vatomic_await_gt_sub_rel) | Dispatches await_gt_sub calls with release memory order.  |
| [vatomic_await_gt_sub_rlx](dispatch.h.md#macro-vatomic_await_gt_sub_rlx) | Dispatches await_gt_sub calls with relaxed memory order.  |
| [vatomic_await_gt_add](dispatch.h.md#macro-vatomic_await_gt_add) | Dispatches await_gt_add calls with seq_cst memory order.  |
| [vatomic_await_gt_add_acq](dispatch.h.md#macro-vatomic_await_gt_add_acq) | Dispatches await_gt_add calls with acquire memory order.  |
| [vatomic_await_gt_add_rel](dispatch.h.md#macro-vatomic_await_gt_add_rel) | Dispatches await_gt_add calls with release memory order.  |
| [vatomic_await_gt_add_rlx](dispatch.h.md#macro-vatomic_await_gt_add_rlx) | Dispatches await_gt_add calls with relaxed memory order.  |
| [vatomic_await_ge_set](dispatch.h.md#macro-vatomic_await_ge_set) | Dispatches await_ge_set calls with seq_cst memory order.  |
| [vatomic_await_ge_set_acq](dispatch.h.md#macro-vatomic_await_ge_set_acq) | Dispatches await_ge_set calls with acquire memory order.  |
| [vatomic_await_ge_set_rel](dispatch.h.md#macro-vatomic_await_ge_set_rel) | Dispatches await_ge_set calls with release memory order.  |
| [vatomic_await_ge_set_rlx](dispatch.h.md#macro-vatomic_await_ge_set_rlx) | Dispatches await_ge_set calls with relaxed memory order.  |
| [vatomic_await_ge_sub](dispatch.h.md#macro-vatomic_await_ge_sub) | Dispatches await_ge_sub calls with seq_cst memory order.  |
| [vatomic_await_ge_sub_acq](dispatch.h.md#macro-vatomic_await_ge_sub_acq) | Dispatches await_ge_sub calls with acquire memory order.  |
| [vatomic_await_ge_sub_rel](dispatch.h.md#macro-vatomic_await_ge_sub_rel) | Dispatches await_ge_sub calls with release memory order.  |
| [vatomic_await_ge_sub_rlx](dispatch.h.md#macro-vatomic_await_ge_sub_rlx) | Dispatches await_ge_sub calls with relaxed memory order.  |
| [vatomic_await_ge_add](dispatch.h.md#macro-vatomic_await_ge_add) | Dispatches await_ge_add calls with seq_cst memory order.  |
| [vatomic_await_ge_add_acq](dispatch.h.md#macro-vatomic_await_ge_add_acq) | Dispatches await_ge_add calls with acquire memory order.  |
| [vatomic_await_ge_add_rel](dispatch.h.md#macro-vatomic_await_ge_add_rel) | Dispatches await_ge_add calls with release memory order.  |
| [vatomic_await_ge_add_rlx](dispatch.h.md#macro-vatomic_await_ge_add_rlx) | Dispatches await_ge_add calls with relaxed memory order.  |

##  Macro `vatomic_init`

```c
vatomic_init(a,v)
```

 
_Dispatches init calls with configurable memory order._ 


See [vatomic32_init](core_u32.h.md#function-vatomic32_init) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: initial value 




##  Macro `vatomic_read`

```c
vatomic_read(a)
```

 
_Dispatches read calls with seq_cst memory order._ 


See [vatomic32_read](core_u32.h.md#function-vatomic32_read) for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: seq_cst 


##  Macro `vatomic_read_acq`

```c
vatomic_read_acq(a)
```

 
_Dispatches read calls with acquire memory order._ 


See [vatomic32_read_acq](core_u32.h.md#function-vatomic32_read_acq) for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: acquire 


##  Macro `vatomic_read_rlx`

```c
vatomic_read_rlx(a)
```

 
_Dispatches read calls with relaxed memory order._ 


See [vatomic32_read_rlx](core_u32.h.md#function-vatomic32_read_rlx) for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** current value

**Memory ordering**: relaxed 


##  Macro `vatomic_write`

```c
vatomic_write(a,v)
```

 
_Dispatches write calls with seq_cst memory order._ 


See [vatomic32_write](core_u32.h.md#function-vatomic32_write) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: seq_cst 


##  Macro `vatomic_write_rel`

```c
vatomic_write_rel(a,v)
```

 
_Dispatches write calls with release memory order._ 


See [vatomic32_write_rel](core_u32.h.md#function-vatomic32_write_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: release 


##  Macro `vatomic_write_rlx`

```c
vatomic_write_rlx(a,v)
```

 
_Dispatches write calls with relaxed memory order._ 


See [vatomic32_write_rlx](core_u32.h.md#function-vatomic32_write_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: new value


**Memory ordering**: relaxed 


##  Macro `vatomic_xchg`

```c
vatomic_xchg(a,v)
```

 
_Dispatches xchg calls with seq_cst memory order._ 


See [vatomic32_xchg](core_u32.h.md#function-vatomic32_xchg) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Macro `vatomic_xchg_acq`

```c
vatomic_xchg_acq(a,v)
```

 
_Dispatches xchg calls with acquire memory order._ 


See [vatomic32_xchg_acq](core_u32.h.md#function-vatomic32_xchg_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: acquire 


##  Macro `vatomic_xchg_rel`

```c
vatomic_xchg_rel(a,v)
```

 
_Dispatches xchg calls with release memory order._ 


See [vatomic32_xchg_rel](core_u32.h.md#function-vatomic32_xchg_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: release 


##  Macro `vatomic_xchg_rlx`

```c
vatomic_xchg_rlx(a,v)
```

 
_Dispatches xchg calls with relaxed memory order._ 


See [vatomic32_xchg_rlx](core_u32.h.md#function-vatomic32_xchg_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: new value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Macro `vatomic_cmpxchg`

```c
vatomic_cmpxchg(a,c,v)
```

 
_Dispatches cmpxchg calls with seq_cst memory order._ 


See [vatomic32_cmpxchg](core_u32.h.md#function-vatomic32_cmpxchg) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: expected value 
- `v`: new value 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Macro `vatomic_cmpxchg_acq`

```c
vatomic_cmpxchg_acq(a,c,v)
```

 
_Dispatches cmpxchg calls with acquire memory order._ 


See [vatomic32_cmpxchg_acq](core_u32.h.md#function-vatomic32_cmpxchg_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: expected value 
- `v`: new value 


**Returns:** old value

**Memory ordering**: acquire 


##  Macro `vatomic_cmpxchg_rel`

```c
vatomic_cmpxchg_rel(a,c,v)
```

 
_Dispatches cmpxchg calls with release memory order._ 


See [vatomic32_cmpxchg_rel](core_u32.h.md#function-vatomic32_cmpxchg_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: expected value 
- `v`: new value 


**Returns:** old value

**Memory ordering**: release 


##  Macro `vatomic_cmpxchg_rlx`

```c
vatomic_cmpxchg_rlx(a,c,v)
```

 
_Dispatches cmpxchg calls with relaxed memory order._ 


See [vatomic32_cmpxchg_rlx](core_u32.h.md#function-vatomic32_cmpxchg_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: expected value 
- `v`: new value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Macro `vatomic_inc`

```c
vatomic_inc(a)
```

 
_Dispatches inc calls with seq_cst memory order._ 


See [vatomic32_inc](core_u32.h.md#function-vatomic32_inc) for details.



**Parameters:**

- `a`: atomic variable


**Memory ordering**: seq_cst 


##  Macro `vatomic_inc_rel`

```c
vatomic_inc_rel(a)
```

 
_Dispatches inc calls with release memory order._ 


See [vatomic32_inc_rel](core_u32.h.md#function-vatomic32_inc_rel) for details.



**Parameters:**

- `a`: atomic variable


**Memory ordering**: release 


##  Macro `vatomic_inc_rlx`

```c
vatomic_inc_rlx(a)
```

 
_Dispatches inc calls with relaxed memory order._ 


See [vatomic32_inc_rlx](core_u32.h.md#function-vatomic32_inc_rlx) for details.



**Parameters:**

- `a`: atomic variable


**Memory ordering**: relaxed 


##  Macro `vatomic_dec`

```c
vatomic_dec(a)
```

 
_Dispatches dec calls with seq_cst memory order._ 


See [vatomic32_dec](core_u32.h.md#function-vatomic32_dec) for details.



**Parameters:**

- `a`: atomic variable


**Memory ordering**: seq_cst 


##  Macro `vatomic_dec_rel`

```c
vatomic_dec_rel(a)
```

 
_Dispatches dec calls with release memory order._ 


See [vatomic32_dec_rel](core_u32.h.md#function-vatomic32_dec_rel) for details.



**Parameters:**

- `a`: atomic variable


**Memory ordering**: release 


##  Macro `vatomic_dec_rlx`

```c
vatomic_dec_rlx(a)
```

 
_Dispatches dec calls with relaxed memory order._ 


See [vatomic32_dec_rlx](core_u32.h.md#function-vatomic32_dec_rlx) for details.



**Parameters:**

- `a`: atomic variable


**Memory ordering**: relaxed 


##  Macro `vatomic_inc_get`

```c
vatomic_inc_get(a)
```

 
_Dispatches inc_get calls with seq_cst memory order._ 


See vatomic32_inc_get for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Macro `vatomic_inc_get_acq`

```c
vatomic_inc_get_acq(a)
```

 
_Dispatches inc_get calls with acquire memory order._ 


See vatomic32_inc_get_acq for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: acquire 


##  Macro `vatomic_inc_get_rel`

```c
vatomic_inc_get_rel(a)
```

 
_Dispatches inc_get calls with release memory order._ 


See vatomic32_inc_get_rel for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: release 


##  Macro `vatomic_inc_get_rlx`

```c
vatomic_inc_get_rlx(a)
```

 
_Dispatches inc_get calls with relaxed memory order._ 


See vatomic32_inc_get_rlx for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: relaxed 


##  Macro `vatomic_dec_get`

```c
vatomic_dec_get(a)
```

 
_Dispatches dec_get calls with seq_cst memory order._ 


See vatomic32_dec_get for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Macro `vatomic_dec_get_acq`

```c
vatomic_dec_get_acq(a)
```

 
_Dispatches dec_get calls with acquire memory order._ 


See vatomic32_dec_get_acq for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: acquire 


##  Macro `vatomic_dec_get_rel`

```c
vatomic_dec_get_rel(a)
```

 
_Dispatches dec_get calls with release memory order._ 


See vatomic32_dec_get_rel for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: release 


##  Macro `vatomic_dec_get_rlx`

```c
vatomic_dec_get_rlx(a)
```

 
_Dispatches dec_get calls with relaxed memory order._ 


See vatomic32_dec_get_rlx for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** new value

**Memory ordering**: relaxed 


##  Macro `vatomic_get_inc`

```c
vatomic_get_inc(a)
```

 
_Dispatches get_inc calls with seq_cst memory order._ 


See vatomic32_get_inc for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Macro `vatomic_get_inc_acq`

```c
vatomic_get_inc_acq(a)
```

 
_Dispatches get_inc calls with acquire memory order._ 


See vatomic32_get_inc_acq for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: acquire 


##  Macro `vatomic_get_inc_rel`

```c
vatomic_get_inc_rel(a)
```

 
_Dispatches get_inc calls with release memory order._ 


See vatomic32_get_inc_rel for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: release 


##  Macro `vatomic_get_inc_rlx`

```c
vatomic_get_inc_rlx(a)
```

 
_Dispatches get_inc calls with relaxed memory order._ 


See vatomic32_get_inc_rlx for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: relaxed 


##  Macro `vatomic_get_dec`

```c
vatomic_get_dec(a)
```

 
_Dispatches get_dec calls with seq_cst memory order._ 


See vatomic32_get_dec for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: seq_cst 


##  Macro `vatomic_get_dec_acq`

```c
vatomic_get_dec_acq(a)
```

 
_Dispatches get_dec calls with acquire memory order._ 


See vatomic32_get_dec_acq for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: acquire 


##  Macro `vatomic_get_dec_rel`

```c
vatomic_get_dec_rel(a)
```

 
_Dispatches get_dec calls with release memory order._ 


See vatomic32_get_dec_rel for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: release 


##  Macro `vatomic_get_dec_rlx`

```c
vatomic_get_dec_rlx(a)
```

 
_Dispatches get_dec calls with relaxed memory order._ 


See vatomic32_get_dec_rlx for details.



**Parameters:**

- `a`: atomic variable 


**Returns:** old value

**Memory ordering**: relaxed 


##  Macro `vatomic_add`

```c
vatomic_add(a,v)
```

 
_Dispatches add calls with seq_cst memory order._ 


See [vatomic32_add](core_u32.h.md#function-vatomic32_add) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: seq_cst 


##  Macro `vatomic_add_rel`

```c
vatomic_add_rel(a,v)
```

 
_Dispatches add calls with release memory order._ 


See [vatomic32_add_rel](core_u32.h.md#function-vatomic32_add_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: release 


##  Macro `vatomic_add_rlx`

```c
vatomic_add_rlx(a,v)
```

 
_Dispatches add calls with relaxed memory order._ 


See [vatomic32_add_rlx](core_u32.h.md#function-vatomic32_add_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: relaxed 


##  Macro `vatomic_sub`

```c
vatomic_sub(a,v)
```

 
_Dispatches sub calls with seq_cst memory order._ 


See [vatomic32_sub](core_u32.h.md#function-vatomic32_sub) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: seq_cst 


##  Macro `vatomic_sub_rel`

```c
vatomic_sub_rel(a,v)
```

 
_Dispatches sub calls with release memory order._ 


See [vatomic32_sub_rel](core_u32.h.md#function-vatomic32_sub_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: release 


##  Macro `vatomic_sub_rlx`

```c
vatomic_sub_rlx(a,v)
```

 
_Dispatches sub calls with relaxed memory order._ 


See [vatomic32_sub_rlx](core_u32.h.md#function-vatomic32_sub_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: relaxed 


##  Macro `vatomic_and`

```c
vatomic_and(a,v)
```

 
_Dispatches and calls with seq_cst memory order._ 


See [vatomic32_and](core_u32.h.md#function-vatomic32_and) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: seq_cst 


##  Macro `vatomic_and_rel`

```c
vatomic_and_rel(a,v)
```

 
_Dispatches and calls with release memory order._ 


See [vatomic32_and_rel](core_u32.h.md#function-vatomic32_and_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: release 


##  Macro `vatomic_and_rlx`

```c
vatomic_and_rlx(a,v)
```

 
_Dispatches and calls with relaxed memory order._ 


See [vatomic32_and_rlx](core_u32.h.md#function-vatomic32_and_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: relaxed 


##  Macro `vatomic_xor`

```c
vatomic_xor(a,v)
```

 
_Dispatches xor calls with seq_cst memory order._ 


See [vatomic32_xor](core_u32.h.md#function-vatomic32_xor) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: seq_cst 


##  Macro `vatomic_xor_rel`

```c
vatomic_xor_rel(a,v)
```

 
_Dispatches xor calls with release memory order._ 


See [vatomic32_xor_rel](core_u32.h.md#function-vatomic32_xor_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: release 


##  Macro `vatomic_xor_rlx`

```c
vatomic_xor_rlx(a,v)
```

 
_Dispatches xor calls with relaxed memory order._ 


See [vatomic32_xor_rlx](core_u32.h.md#function-vatomic32_xor_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: relaxed 


##  Macro `vatomic_or`

```c
vatomic_or(a,v)
```

 
_Dispatches or calls with seq_cst memory order._ 


See [vatomic32_or](core_u32.h.md#function-vatomic32_or) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: seq_cst 


##  Macro `vatomic_or_rel`

```c
vatomic_or_rel(a,v)
```

 
_Dispatches or calls with release memory order._ 


See [vatomic32_or_rel](core_u32.h.md#function-vatomic32_or_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: release 


##  Macro `vatomic_or_rlx`

```c
vatomic_or_rlx(a,v)
```

 
_Dispatches or calls with relaxed memory order._ 


See [vatomic32_or_rlx](core_u32.h.md#function-vatomic32_or_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: relaxed 


##  Macro `vatomic_max`

```c
vatomic_max(a,v)
```

 
_Dispatches max calls with seq_cst memory order._ 


See [vatomic32_max](core_u32.h.md#function-vatomic32_max) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: seq_cst 


##  Macro `vatomic_max_rel`

```c
vatomic_max_rel(a,v)
```

 
_Dispatches max calls with release memory order._ 


See [vatomic32_max_rel](core_u32.h.md#function-vatomic32_max_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: release 


##  Macro `vatomic_max_rlx`

```c
vatomic_max_rlx(a,v)
```

 
_Dispatches max calls with relaxed memory order._ 


See [vatomic32_max_rlx](core_u32.h.md#function-vatomic32_max_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value


**Memory ordering**: relaxed 


##  Macro `vatomic_add_get`

```c
vatomic_add_get(a,v)
```

 
_Dispatches add_get calls with seq_cst memory order._ 


See [vatomic32_add_get](core_u32.h.md#function-vatomic32_add_get) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Macro `vatomic_add_get_acq`

```c
vatomic_add_get_acq(a,v)
```

 
_Dispatches add_get calls with acquire memory order._ 


See [vatomic32_add_get_acq](core_u32.h.md#function-vatomic32_add_get_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: acquire 


##  Macro `vatomic_add_get_rel`

```c
vatomic_add_get_rel(a,v)
```

 
_Dispatches add_get calls with release memory order._ 


See [vatomic32_add_get_rel](core_u32.h.md#function-vatomic32_add_get_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: release 


##  Macro `vatomic_add_get_rlx`

```c
vatomic_add_get_rlx(a,v)
```

 
_Dispatches add_get calls with relaxed memory order._ 


See [vatomic32_add_get_rlx](core_u32.h.md#function-vatomic32_add_get_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Macro `vatomic_sub_get`

```c
vatomic_sub_get(a,v)
```

 
_Dispatches sub_get calls with seq_cst memory order._ 


See [vatomic32_sub_get](core_u32.h.md#function-vatomic32_sub_get) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Macro `vatomic_sub_get_acq`

```c
vatomic_sub_get_acq(a,v)
```

 
_Dispatches sub_get calls with acquire memory order._ 


See [vatomic32_sub_get_acq](core_u32.h.md#function-vatomic32_sub_get_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: acquire 


##  Macro `vatomic_sub_get_rel`

```c
vatomic_sub_get_rel(a,v)
```

 
_Dispatches sub_get calls with release memory order._ 


See [vatomic32_sub_get_rel](core_u32.h.md#function-vatomic32_sub_get_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: release 


##  Macro `vatomic_sub_get_rlx`

```c
vatomic_sub_get_rlx(a,v)
```

 
_Dispatches sub_get calls with relaxed memory order._ 


See [vatomic32_sub_get_rlx](core_u32.h.md#function-vatomic32_sub_get_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Macro `vatomic_and_get`

```c
vatomic_and_get(a,v)
```

 
_Dispatches and_get calls with seq_cst memory order._ 


See [vatomic32_and_get](core_u32.h.md#function-vatomic32_and_get) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Macro `vatomic_and_get_acq`

```c
vatomic_and_get_acq(a,v)
```

 
_Dispatches and_get calls with acquire memory order._ 


See [vatomic32_and_get_acq](core_u32.h.md#function-vatomic32_and_get_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: acquire 


##  Macro `vatomic_and_get_rel`

```c
vatomic_and_get_rel(a,v)
```

 
_Dispatches and_get calls with release memory order._ 


See [vatomic32_and_get_rel](core_u32.h.md#function-vatomic32_and_get_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: release 


##  Macro `vatomic_and_get_rlx`

```c
vatomic_and_get_rlx(a,v)
```

 
_Dispatches and_get calls with relaxed memory order._ 


See [vatomic32_and_get_rlx](core_u32.h.md#function-vatomic32_and_get_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Macro `vatomic_xor_get`

```c
vatomic_xor_get(a,v)
```

 
_Dispatches xor_get calls with seq_cst memory order._ 


See [vatomic32_xor_get](core_u32.h.md#function-vatomic32_xor_get) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Macro `vatomic_xor_get_acq`

```c
vatomic_xor_get_acq(a,v)
```

 
_Dispatches xor_get calls with acquire memory order._ 


See [vatomic32_xor_get_acq](core_u32.h.md#function-vatomic32_xor_get_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: acquire 


##  Macro `vatomic_xor_get_rel`

```c
vatomic_xor_get_rel(a,v)
```

 
_Dispatches xor_get calls with release memory order._ 


See [vatomic32_xor_get_rel](core_u32.h.md#function-vatomic32_xor_get_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: release 


##  Macro `vatomic_xor_get_rlx`

```c
vatomic_xor_get_rlx(a,v)
```

 
_Dispatches xor_get calls with relaxed memory order._ 


See [vatomic32_xor_get_rlx](core_u32.h.md#function-vatomic32_xor_get_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Macro `vatomic_or_get`

```c
vatomic_or_get(a,v)
```

 
_Dispatches or_get calls with seq_cst memory order._ 


See [vatomic32_or_get](core_u32.h.md#function-vatomic32_or_get) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Macro `vatomic_or_get_acq`

```c
vatomic_or_get_acq(a,v)
```

 
_Dispatches or_get calls with acquire memory order._ 


See [vatomic32_or_get_acq](core_u32.h.md#function-vatomic32_or_get_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: acquire 


##  Macro `vatomic_or_get_rel`

```c
vatomic_or_get_rel(a,v)
```

 
_Dispatches or_get calls with release memory order._ 


See [vatomic32_or_get_rel](core_u32.h.md#function-vatomic32_or_get_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: release 


##  Macro `vatomic_or_get_rlx`

```c
vatomic_or_get_rlx(a,v)
```

 
_Dispatches or_get calls with relaxed memory order._ 


See [vatomic32_or_get_rlx](core_u32.h.md#function-vatomic32_or_get_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Macro `vatomic_max_get`

```c
vatomic_max_get(a,v)
```

 
_Dispatches max_get calls with seq_cst memory order._ 


See [vatomic32_max_get](core_u32.h.md#function-vatomic32_max_get) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: seq_cst 


##  Macro `vatomic_max_get_acq`

```c
vatomic_max_get_acq(a,v)
```

 
_Dispatches max_get calls with acquire memory order._ 


See [vatomic32_max_get_acq](core_u32.h.md#function-vatomic32_max_get_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: acquire 


##  Macro `vatomic_max_get_rel`

```c
vatomic_max_get_rel(a,v)
```

 
_Dispatches max_get calls with release memory order._ 


See [vatomic32_max_get_rel](core_u32.h.md#function-vatomic32_max_get_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: release 


##  Macro `vatomic_max_get_rlx`

```c
vatomic_max_get_rlx(a,v)
```

 
_Dispatches max_get calls with relaxed memory order._ 


See [vatomic32_max_get_rlx](core_u32.h.md#function-vatomic32_max_get_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** new value

**Memory ordering**: relaxed 


##  Macro `vatomic_get_add`

```c
vatomic_get_add(a,v)
```

 
_Dispatches get_add calls with seq_cst memory order._ 


Dispatches get_add calls with relaxed memory order.

Dispatches get_add calls with release memory order.

Dispatches get_add calls with acquire memory order.

See [vatomic32_get_add](core_u32.h.md#function-vatomic32_get_add) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: seq_cst

See [vatomic32_get_add_acq](core_u32.h.md#function-vatomic32_get_add_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: acquire

See [vatomic32_get_add_rel](core_u32.h.md#function-vatomic32_get_add_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: release

See [vatomic32_get_add_rlx](core_u32.h.md#function-vatomic32_get_add_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Macro `vatomic_get_sub`

```c
vatomic_get_sub(a,v)
```

 
_Dispatches get_sub calls with seq_cst memory order._ 


Dispatches get_sub calls with relaxed memory order.

Dispatches get_sub calls with release memory order.

Dispatches get_sub calls with acquire memory order.

See [vatomic32_get_sub](core_u32.h.md#function-vatomic32_get_sub) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: seq_cst

See [vatomic32_get_sub_acq](core_u32.h.md#function-vatomic32_get_sub_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: acquire

See [vatomic32_get_sub_rel](core_u32.h.md#function-vatomic32_get_sub_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: release

See [vatomic32_get_sub_rlx](core_u32.h.md#function-vatomic32_get_sub_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Macro `vatomic_get_and`

```c
vatomic_get_and(a,v)
```

 
_Dispatches get_and calls with seq_cst memory order._ 


Dispatches get_and calls with relaxed memory order.

Dispatches get_and calls with release memory order.

Dispatches get_and calls with acquire memory order.

See [vatomic32_get_and](core_u32.h.md#function-vatomic32_get_and) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: seq_cst

See [vatomic32_get_and_acq](core_u32.h.md#function-vatomic32_get_and_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: acquire

See [vatomic32_get_and_rel](core_u32.h.md#function-vatomic32_get_and_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: release

See [vatomic32_get_and_rlx](core_u32.h.md#function-vatomic32_get_and_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Macro `vatomic_get_xor`

```c
vatomic_get_xor(a,v)
```

 
_Dispatches get_xor calls with seq_cst memory order._ 


Dispatches get_xor calls with relaxed memory order.

Dispatches get_xor calls with release memory order.

Dispatches get_xor calls with acquire memory order.

See [vatomic32_get_xor](core_u32.h.md#function-vatomic32_get_xor) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: seq_cst

See [vatomic32_get_xor_acq](core_u32.h.md#function-vatomic32_get_xor_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: acquire

See [vatomic32_get_xor_rel](core_u32.h.md#function-vatomic32_get_xor_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: release

See [vatomic32_get_xor_rlx](core_u32.h.md#function-vatomic32_get_xor_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Macro `vatomic_get_or`

```c
vatomic_get_or(a,v)
```

 
_Dispatches get_or calls with seq_cst memory order._ 


Dispatches get_or calls with relaxed memory order.

Dispatches get_or calls with release memory order.

Dispatches get_or calls with acquire memory order.

See [vatomic32_get_or](core_u32.h.md#function-vatomic32_get_or) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: seq_cst

See [vatomic32_get_or_acq](core_u32.h.md#function-vatomic32_get_or_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: acquire

See [vatomic32_get_or_rel](core_u32.h.md#function-vatomic32_get_or_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: release

See [vatomic32_get_or_rlx](core_u32.h.md#function-vatomic32_get_or_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Macro `vatomic_get_max`

```c
vatomic_get_max(a,v)
```

 
_Dispatches get_max calls with seq_cst memory order._ 


Dispatches get_max calls with relaxed memory order.

Dispatches get_max calls with release memory order.

Dispatches get_max calls with acquire memory order.

See [vatomic32_get_max](core_u32.h.md#function-vatomic32_get_max) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: seq_cst

See [vatomic32_get_max_acq](core_u32.h.md#function-vatomic32_get_max_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: acquire

See [vatomic32_get_max_rel](core_u32.h.md#function-vatomic32_get_max_rel) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: release

See [vatomic32_get_max_rlx](core_u32.h.md#function-vatomic32_get_max_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `v`: operand value 


**Returns:** old value

**Memory ordering**: relaxed 


##  Macro `vatomic_await_eq`

```c
vatomic_await_eq(a,c)
```

 
_Dispatches await_eq calls with seq_cst memory order._ 


See [vatomic32_await_eq](await_u32.h.md#function-vatomic32_await_eq) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** previously read value

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_eq_acq`

```c
vatomic_await_eq_acq(a,c)
```

 
_Dispatches await_eq calls with acquire memory order._ 


See [vatomic32_await_eq_acq](await_u32.h.md#function-vatomic32_await_eq_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** previously read value

**Memory ordering**: acquire 


##  Macro `vatomic_await_eq_rlx`

```c
vatomic_await_eq_rlx(a,c)
```

 
_Dispatches await_eq calls with relaxed memory order._ 


See [vatomic32_await_eq_rlx](await_u32.h.md#function-vatomic32_await_eq_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** previously read value

**Memory ordering**: relaxed 


##  Macro `vatomic_await_neq`

```c
vatomic_await_neq(a,c)
```

 
_Dispatches await_neq calls with seq_cst memory order._ 


See [vatomic32_await_neq](await_u32.h.md#function-vatomic32_await_neq) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_neq_acq`

```c
vatomic_await_neq_acq(a,c)
```

 
_Dispatches await_neq calls with acquire memory order._ 


See [vatomic32_await_neq_acq](await_u32.h.md#function-vatomic32_await_neq_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_neq_rlx`

```c
vatomic_await_neq_rlx(a,c)
```

 
_Dispatches await_neq calls with relaxed memory order._ 


See [vatomic32_await_neq_rlx](await_u32.h.md#function-vatomic32_await_neq_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_le`

```c
vatomic_await_le(a,c)
```

 
_Dispatches await_le calls with seq_cst memory order._ 


See [vatomic32_await_le](await_u32.h.md#function-vatomic32_await_le) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_le_acq`

```c
vatomic_await_le_acq(a,c)
```

 
_Dispatches await_le calls with acquire memory order._ 


See [vatomic32_await_le_acq](await_u32.h.md#function-vatomic32_await_le_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_le_rlx`

```c
vatomic_await_le_rlx(a,c)
```

 
_Dispatches await_le calls with relaxed memory order._ 


See [vatomic32_await_le_rlx](await_u32.h.md#function-vatomic32_await_le_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_lt`

```c
vatomic_await_lt(a,c)
```

 
_Dispatches await_lt calls with seq_cst memory order._ 


See [vatomic32_await_lt](await_u32.h.md#function-vatomic32_await_lt) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_lt_acq`

```c
vatomic_await_lt_acq(a,c)
```

 
_Dispatches await_lt calls with acquire memory order._ 


See [vatomic32_await_lt_acq](await_u32.h.md#function-vatomic32_await_lt_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_lt_rlx`

```c
vatomic_await_lt_rlx(a,c)
```

 
_Dispatches await_lt calls with relaxed memory order._ 


See [vatomic32_await_lt_rlx](await_u32.h.md#function-vatomic32_await_lt_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_gt`

```c
vatomic_await_gt(a,c)
```

 
_Dispatches await_gt calls with seq_cst memory order._ 


See [vatomic32_await_gt](await_u32.h.md#function-vatomic32_await_gt) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_gt_acq`

```c
vatomic_await_gt_acq(a,c)
```

 
_Dispatches await_gt calls with acquire memory order._ 


See [vatomic32_await_gt_acq](await_u32.h.md#function-vatomic32_await_gt_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_gt_rlx`

```c
vatomic_await_gt_rlx(a,c)
```

 
_Dispatches await_gt calls with relaxed memory order._ 


See [vatomic32_await_gt_rlx](await_u32.h.md#function-vatomic32_await_gt_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_ge`

```c
vatomic_await_ge(a,c)
```

 
_Dispatches await_ge calls with seq_cst memory order._ 


See [vatomic32_await_ge](await_u32.h.md#function-vatomic32_await_ge) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_ge_acq`

```c
vatomic_await_ge_acq(a,c)
```

 
_Dispatches await_ge calls with acquire memory order._ 


See [vatomic32_await_ge_acq](await_u32.h.md#function-vatomic32_await_ge_acq) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_ge_rlx`

```c
vatomic_await_ge_rlx(a,c)
```

 
_Dispatches await_ge calls with relaxed memory order._ 


See [vatomic32_await_ge_rlx](await_u32.h.md#function-vatomic32_await_ge_rlx) for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_eq_set`

```c
vatomic_await_eq_set(a,c,v)
```

 
_Dispatches await_eq_set calls with seq_cst memory order._ 


See vatomic32_await_eq_set_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_eq_set_acq`

```c
vatomic_await_eq_set_acq(a,c,v)
```

 
_Dispatches await_eq_set calls with acquire memory order._ 


See vatomic32_await_eq_set_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: acquire 


##  Macro `vatomic_await_eq_set_rel`

```c
vatomic_await_eq_set_rel(a,c,v)
```

 
_Dispatches await_eq_set calls with release memory order._ 


See vatomic32_await_eq_set_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: release 


##  Macro `vatomic_await_eq_set_rlx`

```c
vatomic_await_eq_set_rlx(a,c,v)
```

 
_Dispatches await_eq_set calls with relaxed memory order._ 


See vatomic32_await_eq_set_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: relaxed 


##  Macro `vatomic_await_eq_sub`

```c
vatomic_await_eq_sub(a,c,v)
```

 
_Dispatches await_eq_sub calls with seq_cst memory order._ 


See vatomic32_await_eq_sub_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_eq_sub_acq`

```c
vatomic_await_eq_sub_acq(a,c,v)
```

 
_Dispatches await_eq_sub calls with acquire memory order._ 


See vatomic32_await_eq_sub_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: acquire 


##  Macro `vatomic_await_eq_sub_rel`

```c
vatomic_await_eq_sub_rel(a,c,v)
```

 
_Dispatches await_eq_sub calls with release memory order._ 


See vatomic32_await_eq_sub_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: release 


##  Macro `vatomic_await_eq_sub_rlx`

```c
vatomic_await_eq_sub_rlx(a,c,v)
```

 
_Dispatches await_eq_sub calls with relaxed memory order._ 


See vatomic32_await_eq_sub_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: relaxed 


##  Macro `vatomic_await_eq_add`

```c
vatomic_await_eq_add(a,c,v)
```

 
_Dispatches await_eq_add calls with seq_cst memory order._ 


See vatomic32_await_eq_add_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_eq_add_acq`

```c
vatomic_await_eq_add_acq(a,c,v)
```

 
_Dispatches await_eq_add calls with acquire memory order._ 


See vatomic32_await_eq_add_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: acquire 


##  Macro `vatomic_await_eq_add_rel`

```c
vatomic_await_eq_add_rel(a,c,v)
```

 
_Dispatches await_eq_add calls with release memory order._ 


See vatomic32_await_eq_add_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: release 


##  Macro `vatomic_await_eq_add_rlx`

```c
vatomic_await_eq_add_rlx(a,c,v)
```

 
_Dispatches await_eq_add calls with relaxed memory order._ 


See vatomic32_await_eq_add_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** previously read value

**Memory ordering**: relaxed 


##  Macro `vatomic_await_neq_set`

```c
vatomic_await_neq_set(a,c,v)
```

 
_Dispatches await_neq_set calls with seq_cst memory order._ 


See vatomic32_await_neq_set_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_neq_set_acq`

```c
vatomic_await_neq_set_acq(a,c,v)
```

 
_Dispatches await_neq_set calls with acquire memory order._ 


See vatomic32_await_neq_set_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_neq_set_rel`

```c
vatomic_await_neq_set_rel(a,c,v)
```

 
_Dispatches await_neq_set calls with release memory order._ 


See vatomic32_await_neq_set_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_neq_set_rlx`

```c
vatomic_await_neq_set_rlx(a,c,v)
```

 
_Dispatches await_neq_set calls with relaxed memory order._ 


See vatomic32_await_neq_set_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_neq_sub`

```c
vatomic_await_neq_sub(a,c,v)
```

 
_Dispatches await_neq_sub calls with seq_cst memory order._ 


See vatomic32_await_neq_sub_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_neq_sub_acq`

```c
vatomic_await_neq_sub_acq(a,c,v)
```

 
_Dispatches await_neq_sub calls with acquire memory order._ 


See vatomic32_await_neq_sub_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_neq_sub_rel`

```c
vatomic_await_neq_sub_rel(a,c,v)
```

 
_Dispatches await_neq_sub calls with release memory order._ 


See vatomic32_await_neq_sub_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_neq_sub_rlx`

```c
vatomic_await_neq_sub_rlx(a,c,v)
```

 
_Dispatches await_neq_sub calls with relaxed memory order._ 


See vatomic32_await_neq_sub_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_neq_add`

```c
vatomic_await_neq_add(a,c,v)
```

 
_Dispatches await_neq_add calls with seq_cst memory order._ 


See vatomic32_await_neq_add_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_neq_add_acq`

```c
vatomic_await_neq_add_acq(a,c,v)
```

 
_Dispatches await_neq_add calls with acquire memory order._ 


See vatomic32_await_neq_add_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_neq_add_rel`

```c
vatomic_await_neq_add_rel(a,c,v)
```

 
_Dispatches await_neq_add calls with release memory order._ 


See vatomic32_await_neq_add_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_neq_add_rlx`

```c
vatomic_await_neq_add_rlx(a,c,v)
```

 
_Dispatches await_neq_add calls with relaxed memory order._ 


See vatomic32_await_neq_add_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_le_set`

```c
vatomic_await_le_set(a,c,v)
```

 
_Dispatches await_le_set calls with seq_cst memory order._ 


See vatomic32_await_le_set_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_le_set_acq`

```c
vatomic_await_le_set_acq(a,c,v)
```

 
_Dispatches await_le_set calls with acquire memory order._ 


See vatomic32_await_le_set_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_le_set_rel`

```c
vatomic_await_le_set_rel(a,c,v)
```

 
_Dispatches await_le_set calls with release memory order._ 


See vatomic32_await_le_set_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_le_set_rlx`

```c
vatomic_await_le_set_rlx(a,c,v)
```

 
_Dispatches await_le_set calls with relaxed memory order._ 


See vatomic32_await_le_set_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_le_sub`

```c
vatomic_await_le_sub(a,c,v)
```

 
_Dispatches await_le_sub calls with seq_cst memory order._ 


See vatomic32_await_le_sub_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_le_sub_acq`

```c
vatomic_await_le_sub_acq(a,c,v)
```

 
_Dispatches await_le_sub calls with acquire memory order._ 


See vatomic32_await_le_sub_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_le_sub_rel`

```c
vatomic_await_le_sub_rel(a,c,v)
```

 
_Dispatches await_le_sub calls with release memory order._ 


See vatomic32_await_le_sub_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_le_sub_rlx`

```c
vatomic_await_le_sub_rlx(a,c,v)
```

 
_Dispatches await_le_sub calls with relaxed memory order._ 


See vatomic32_await_le_sub_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_le_add`

```c
vatomic_await_le_add(a,c,v)
```

 
_Dispatches await_le_add calls with seq_cst memory order._ 


See vatomic32_await_le_add_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_le_add_acq`

```c
vatomic_await_le_add_acq(a,c,v)
```

 
_Dispatches await_le_add calls with acquire memory order._ 


See vatomic32_await_le_add_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_le_add_rel`

```c
vatomic_await_le_add_rel(a,c,v)
```

 
_Dispatches await_le_add calls with release memory order._ 


See vatomic32_await_le_add_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_le_add_rlx`

```c
vatomic_await_le_add_rlx(a,c,v)
```

 
_Dispatches await_le_add calls with relaxed memory order._ 


See vatomic32_await_le_add_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_lt_set`

```c
vatomic_await_lt_set(a,c,v)
```

 
_Dispatches await_lt_set calls with seq_cst memory order._ 


See vatomic32_await_lt_set_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_lt_set_acq`

```c
vatomic_await_lt_set_acq(a,c,v)
```

 
_Dispatches await_lt_set calls with acquire memory order._ 


See vatomic32_await_lt_set_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_lt_set_rel`

```c
vatomic_await_lt_set_rel(a,c,v)
```

 
_Dispatches await_lt_set calls with release memory order._ 


See vatomic32_await_lt_set_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_lt_set_rlx`

```c
vatomic_await_lt_set_rlx(a,c,v)
```

 
_Dispatches await_lt_set calls with relaxed memory order._ 


See vatomic32_await_lt_set_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_lt_sub`

```c
vatomic_await_lt_sub(a,c,v)
```

 
_Dispatches await_lt_sub calls with seq_cst memory order._ 


See vatomic32_await_lt_sub_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_lt_sub_acq`

```c
vatomic_await_lt_sub_acq(a,c,v)
```

 
_Dispatches await_lt_sub calls with acquire memory order._ 


See vatomic32_await_lt_sub_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_lt_sub_rel`

```c
vatomic_await_lt_sub_rel(a,c,v)
```

 
_Dispatches await_lt_sub calls with release memory order._ 


See vatomic32_await_lt_sub_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_lt_sub_rlx`

```c
vatomic_await_lt_sub_rlx(a,c,v)
```

 
_Dispatches await_lt_sub calls with relaxed memory order._ 


See vatomic32_await_lt_sub_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_lt_add`

```c
vatomic_await_lt_add(a,c,v)
```

 
_Dispatches await_lt_add calls with seq_cst memory order._ 


See vatomic32_await_lt_add_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_lt_add_acq`

```c
vatomic_await_lt_add_acq(a,c,v)
```

 
_Dispatches await_lt_add calls with acquire memory order._ 


See vatomic32_await_lt_add_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_lt_add_rel`

```c
vatomic_await_lt_add_rel(a,c,v)
```

 
_Dispatches await_lt_add calls with release memory order._ 


See vatomic32_await_lt_add_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_lt_add_rlx`

```c
vatomic_await_lt_add_rlx(a,c,v)
```

 
_Dispatches await_lt_add calls with relaxed memory order._ 


See vatomic32_await_lt_add_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_gt_set`

```c
vatomic_await_gt_set(a,c,v)
```

 
_Dispatches await_gt_set calls with seq_cst memory order._ 


See vatomic32_await_gt_set_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_gt_set_acq`

```c
vatomic_await_gt_set_acq(a,c,v)
```

 
_Dispatches await_gt_set calls with acquire memory order._ 


See vatomic32_await_gt_set_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_gt_set_rel`

```c
vatomic_await_gt_set_rel(a,c,v)
```

 
_Dispatches await_gt_set calls with release memory order._ 


See vatomic32_await_gt_set_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_gt_set_rlx`

```c
vatomic_await_gt_set_rlx(a,c,v)
```

 
_Dispatches await_gt_set calls with relaxed memory order._ 


See vatomic32_await_gt_set_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_gt_sub`

```c
vatomic_await_gt_sub(a,c,v)
```

 
_Dispatches await_gt_sub calls with seq_cst memory order._ 


See vatomic32_await_gt_sub_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_gt_sub_acq`

```c
vatomic_await_gt_sub_acq(a,c,v)
```

 
_Dispatches await_gt_sub calls with acquire memory order._ 


See vatomic32_await_gt_sub_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_gt_sub_rel`

```c
vatomic_await_gt_sub_rel(a,c,v)
```

 
_Dispatches await_gt_sub calls with release memory order._ 


See vatomic32_await_gt_sub_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_gt_sub_rlx`

```c
vatomic_await_gt_sub_rlx(a,c,v)
```

 
_Dispatches await_gt_sub calls with relaxed memory order._ 


See vatomic32_await_gt_sub_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_gt_add`

```c
vatomic_await_gt_add(a,c,v)
```

 
_Dispatches await_gt_add calls with seq_cst memory order._ 


See vatomic32_await_gt_add_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_gt_add_acq`

```c
vatomic_await_gt_add_acq(a,c,v)
```

 
_Dispatches await_gt_add calls with acquire memory order._ 


See vatomic32_await_gt_add_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_gt_add_rel`

```c
vatomic_await_gt_add_rel(a,c,v)
```

 
_Dispatches await_gt_add calls with release memory order._ 


See vatomic32_await_gt_add_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_gt_add_rlx`

```c
vatomic_await_gt_add_rlx(a,c,v)
```

 
_Dispatches await_gt_add calls with relaxed memory order._ 


See vatomic32_await_gt_add_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_ge_set`

```c
vatomic_await_ge_set(a,c,v)
```

 
_Dispatches await_ge_set calls with seq_cst memory order._ 


See vatomic32_await_ge_set_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_ge_set_acq`

```c
vatomic_await_ge_set_acq(a,c,v)
```

 
_Dispatches await_ge_set calls with acquire memory order._ 


See vatomic32_await_ge_set_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_ge_set_rel`

```c
vatomic_await_ge_set_rel(a,c,v)
```

 
_Dispatches await_ge_set calls with release memory order._ 


See vatomic32_await_ge_set_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_ge_set_rlx`

```c
vatomic_await_ge_set_rlx(a,c,v)
```

 
_Dispatches await_ge_set calls with relaxed memory order._ 


See vatomic32_await_ge_set_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_ge_sub`

```c
vatomic_await_ge_sub(a,c,v)
```

 
_Dispatches await_ge_sub calls with seq_cst memory order._ 


See vatomic32_await_ge_sub_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_ge_sub_acq`

```c
vatomic_await_ge_sub_acq(a,c,v)
```

 
_Dispatches await_ge_sub calls with acquire memory order._ 


See vatomic32_await_ge_sub_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_ge_sub_rel`

```c
vatomic_await_ge_sub_rel(a,c,v)
```

 
_Dispatches await_ge_sub calls with release memory order._ 


See vatomic32_await_ge_sub_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_ge_sub_rlx`

```c
vatomic_await_ge_sub_rlx(a,c,v)
```

 
_Dispatches await_ge_sub calls with relaxed memory order._ 


See vatomic32_await_ge_sub_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 


##  Macro `vatomic_await_ge_add`

```c
vatomic_await_ge_add(a,c,v)
```

 
_Dispatches await_ge_add calls with seq_cst memory order._ 


See vatomic32_await_ge_add_seq_cst for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: seq_cst 


##  Macro `vatomic_await_ge_add_acq`

```c
vatomic_await_ge_add_acq(a,c,v)
```

 
_Dispatches await_ge_add calls with acquire memory order._ 


See vatomic32_await_ge_add_acquire for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: acquire 


##  Macro `vatomic_await_ge_add_rel`

```c
vatomic_await_ge_add_rel(a,c,v)
```

 
_Dispatches await_ge_add calls with release memory order._ 


See vatomic32_await_ge_add_release for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: release 


##  Macro `vatomic_await_ge_add_rlx`

```c
vatomic_await_ge_add_rlx(a,c,v)
```

 
_Dispatches await_ge_add calls with relaxed memory order._ 


See vatomic32_await_ge_add_relaxed for details.



**Parameters:**

- `a`: atomic variable 
- `c`: condition value 
- `v`: operand value 


**Returns:** value satisfying condition

**Memory ordering**: relaxed 



---
