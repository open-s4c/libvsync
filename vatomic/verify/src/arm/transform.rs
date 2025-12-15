/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
use super::*;

pub fn extract_arm_functions(
    parsed: Vec<ArmInstruction>,
    names: Option<&[String]>,
    valid_prefix: &[&str],
) -> Vec<ArmFunction> {
    let mut functions = Vec::new();
    let mut current_function: Option<(String, Vec<ArmInstruction>)> = None;

    for instr in parsed {
        match &instr {
            ArmInstruction::Label(name) if !name.starts_with(".") => {
                if let Some((prev_name, prev_instrs)) = current_function {
                    functions.push(ArmFunction {
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
        functions.push(ArmFunction {
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

pub fn transform_labels(function: &ArmFunction) -> ArmFunction {
    use std::collections::HashMap;

    let mut label_map: HashMap<String, String> = HashMap::new();
    let mut forward_map: HashMap<String, String> = HashMap::new();
    let mut counter = 0usize;
    let mut new_instructions = Vec::new();

    for instr in &function.instructions {
        match instr {
            ArmInstruction::Label(name) => {
                // If it was a forward ref, bind it and remove from forward_map
                let new_name = if let Some(forward) = forward_map.remove(name) {
                    label_map.insert(name.clone(), forward.clone());
                    forward
                } else {
                    let new_label = format!("L{}", counter + 1);
                    counter += 1;
                    label_map.insert(name.clone(), new_label.clone());
                    new_label
                };
                new_instructions.push(ArmInstruction::Label(new_name));
            }
            ArmInstruction::Branch(cond, Operand::Label(name)) => {
                // Handle backward/forward shorthand in name if desired
                let new_name = if name.ends_with('b') {
                    let base = &name[..name.len() - 1];
                    label_map.get(base).cloned().unwrap_or_else(|| {
                        // No backward def yet; create one
                        let new_label = format!("L{}", counter + 1);
                        counter += 1;
                        label_map.insert(base.to_string(), new_label.clone());
                        new_label
                    })
                } else if name.ends_with('f') {
                    let base = &name[..name.len() - 1];
                    forward_map.get(base).cloned().unwrap_or_else(|| {
                        // First forward ref for this base
                        let new_label = format!("L{}", counter + 1);
                        counter += 1;
                        forward_map.insert(base.to_string(), new_label.clone());
                        new_label
                    })
                } else {
                    // Normal named label
                    if let Some(lbl) = label_map.get(name) {
                        lbl.clone()
                    } else if let Some(lbl) = forward_map.get(name) {
                        lbl.clone()
                    } else {
                        // Treat it like a first forward ref
                        let new_label = format!("L{}", counter + 1);
                        counter += 1;
                        forward_map.insert(name.to_string(), new_label.clone());
                        new_label
                    }
                };
                new_instructions.push(ArmInstruction::Branch(
                    cond.clone(),
                    Operand::Label(new_name),
                ));
            }
            ArmInstruction::BranchLink(Operand::Label(name)) => {
                let new_name = label_map.get(name)
                    .cloned()
                    .or_else(|| {
                        // Treat as forward if unknown
                        if !forward_map.contains_key(name) {
                            let new_label = format!("L{}", counter + 1);
                            counter += 1;
                            forward_map.insert(name.clone(), new_label.clone());
                            Some(new_label)
                        } else {
                            forward_map.get(name).cloned()
                        }
                    })
                    .unwrap_or_else(|| name.clone());
                new_instructions.push(ArmInstruction::BranchLink(Operand::Label(new_name)));
            }
            ArmInstruction::TestBitBranch(cond, op1, op2, Operand::Label(name)) => {
                let new_name = label_map.get(name)
                    .cloned()
                    .or_else(|| {
                        if !forward_map.contains_key(name) {
                            let new_label = format!("L{}", counter + 1);
                            counter += 1;
                            forward_map.insert(name.clone(), new_label.clone());
                            Some(new_label)
                        } else {
                            forward_map.get(name).cloned()
                        }
                    })
                    .unwrap_or_else(|| name.clone());
                new_instructions.push(ArmInstruction::TestBitBranch(
                    *cond,
                    op1.clone(),
                    op2.clone(),
                    Operand::Label(new_name),
                ));
            }
            _ => new_instructions.push(instr.clone()),
        }
    }

    ArmFunction {
        name: function.name.clone(),
        instructions: new_instructions,
    }
}

pub fn remove_directives(function: &ArmFunction) -> ArmFunction {
    let new_instructions = function
        .instructions
        .iter()
        .filter(|instruction| !matches!(instruction, ArmInstruction::Directive(_)))
        .cloned()
        .collect();

    ArmFunction {
        name: function.name.clone(),
        instructions: new_instructions,
    }
}
