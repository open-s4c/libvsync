/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
use super::*;

use head_tail_iter::HeadTailIterator;
use std::collections::HashMap;

pub fn extract_riscv_functions(
    parsed: Vec<RiscvInstruction>,
    names: Option<&[String]>,
    valid_prefix: &[&str],
) -> Vec<RiscvFunction> {
    let mut functions = Vec::new();
    let mut current_function: Option<(String, Vec<RiscvInstruction>)> = None;

    for instr in parsed {
        match &instr {
            RiscvInstruction::Label(name)
                if name
                    .chars()
                    .next()
                    .map_or(false, |c| c.is_alphabetic() || c == '_') =>
            {
                if let Some((prev_name, prev_instrs)) = current_function {
                    functions.push(RiscvFunction {
                        name: prev_name,
                        instructions: prev_instrs,
                    });
                }
                current_function = Some((name.clone(), vec![instr.clone()]));
            }
            _ => {
                if let Some((_, ref mut instrs)) = current_function {
                    instrs.push(instr.clone());
                }
            }
        }
    }

    if let Some((name, instrs)) = current_function {
        functions.push(RiscvFunction {
            name,
            instructions: instrs,
        });
    }

    match names {
        Some(names) => {
            let valid_ns = valid_prefix;
            functions
                .into_iter()
                .filter(|f| {
                    if f.name.starts_with("vatomic") {
                        let rest = &f.name[7..]; // Remove "vatomic"
                        if let Some((n, suffix)) = rest.split_once('_') {
                            valid_ns.contains(&n) && names.contains(&suffix.to_string())
                        } else {
                            false
                        }
                    } else {
                        false
                    }
                })
                .collect()
        }
        None => functions,
    }
}
pub fn remove_directives(function: &RiscvFunction) -> RiscvFunction {
    let new_instructions = function
        .instructions
        .iter()
        .filter(|instruction| !matches!(instruction, RiscvInstruction::Directive(_)))
        .cloned()
        .collect();

    RiscvFunction {
        name: function.name.clone(),
        instructions: new_instructions,
    }
}

macro_rules! to_label_name {
    ($i:expr) => {
        format!("L{}", $i)
    };
}

fn get_label_index(
    fwd_map: &HashMap<&str, usize>,
    back_map: &HashMap<&str, usize>,
    label: &String,
) -> String {
    let fwd = label.ends_with('f');
    let bwd = label.ends_with('b');
    let new_label_idx = if fwd || bwd {
        let short_label = &label[0..label.len() - 1];
        (if fwd { &fwd_map } else { &back_map })[short_label]
    } else {
        back_map
            .get(label.as_str())
            .copied()
            .unwrap_or_else(|| fwd_map[label.as_str()])
    };
    to_label_name!(new_label_idx)
}

pub fn transform_labels(function: &RiscvFunction) -> RiscvFunction {
    let mut back_map = HashMap::new();
    let new_instructions = function
        .instructions
        .head_tail_pairs()
        .enumerate()
        .map(|(i, (instr, remaining))| {
            let mut fwd_map: HashMap<&str, usize> = HashMap::new();
            for (idx, instr) in remaining.iter().enumerate() {
                if let RiscvInstruction::Label(label) = instr {
                    fwd_map.entry(label.as_str()).or_insert(idx + i + 1);
                }
            }

            match instr {
                RiscvInstruction::Label(name) => {
                    back_map.insert(name.as_str(), i);
                    RiscvInstruction::Label(to_label_name!(i))
                }
                RiscvInstruction::Jump { rd, label: Some(label) } => RiscvInstruction::Jump {
                    rd: rd.clone(),
                    label: Some(get_label_index(&fwd_map, &back_map, label)),
                },
                RiscvInstruction::Branch {
                    op,
                    rs1,
                    rs2,
                    label,
                } => RiscvInstruction::Branch {
                    op: op.clone(),
                    rs1: rs1.clone(),
                    rs2: rs2.clone(),
                    label: get_label_index(&fwd_map, &back_map, label),
                },
                _ => instr.clone(),
            }
        })
        .collect();

    RiscvFunction {
        name: function.name.clone(),
        instructions: new_instructions,
    }
}
