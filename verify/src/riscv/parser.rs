/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
use nom::{
    IResult, Parser,
    branch::alt,
    bytes::complete::{tag, take_till, take_while1},
    character::complete::{char, digit1, hex_digit1, multispace0, space0, space1},
    combinator::{map, map_res, opt, recognize, value},
    multi::{many0, separated_list0},
    sequence::{delimited, pair, preceded, terminated},
};

use super::*;

fn parse_register(input: &str) -> IResult<&str, Register> {
    let (input, name) = take_while1(|c: char| c.is_alphanumeric())(input)?;
    let (reg_type, number) = match name {
        "zero" => (RegisterType::Special("zero".to_string()), None),
        "ra" => (RegisterType::Special("ra".to_string()), None),
        "sp" => (RegisterType::Special("sp".to_string()), None),
        _ if name.starts_with('a') && name.len() == 2 && name[1..].parse::<u8>().is_ok() => {
            let num = name[1..].parse::<u8>().unwrap();
            if num <= 7 {
                (RegisterType::A, Some(num))
            } else {
                (RegisterType::X, Some(num))
            }
        }
        _ if name.starts_with('t') && name.len() == 2 && name[1..].parse::<u8>().is_ok() => {
            let num = name[1..].parse::<u8>().unwrap();
            if num <= 6 {
                (RegisterType::T, Some(num))
            } else {
                (RegisterType::X, Some(num))
            }
        }
        _ if name.starts_with('s') && name.len() == 2 && name[1..].parse::<u8>().is_ok() => {
            let num = name[1..].parse::<u8>().unwrap();
            if num <= 11 {
                (RegisterType::S, Some(num))
            } else {
                (RegisterType::X, Some(num))
            }
        }
        _ if name.starts_with('x') && name.len() >= 2 && name[1..].parse::<u8>().is_ok() => {
            let num = name[1..].parse::<u8>().unwrap();
            (RegisterType::X, Some(num))
        }
        _ => {
            return Err(nom::Err::Error(nom::error::Error::new(
                input,
                nom::error::ErrorKind::Tag,
            )));
        }
    };
    Ok((input, Register { reg_type, number }))
}

fn parse_memory_operand(input: &str) -> IResult<&str, MemoryOperand> {
    map(
        (
            parse_immediate,
            delimited(char('('), parse_register, char(')')),
        ),
        |(offset, base)| MemoryOperand { offset, base },
    )
    .parse(input)
}

fn parse_immediate(input: &str) -> IResult<&str, i64> {
    let (input, _) = alt((tag("#"), tag(""))).parse(input)?;
    let (input, signed) = opt(char('-')).parse(input)?;

    let (input, value_str) = alt((
        recognize(pair(alt((tag("0x"), tag("0X"))), hex_digit1)),
        recognize(digit1),
    ))
    .parse(input)?;

    let base = if value_str.starts_with("0x") || value_str.starts_with("0X") {
        16
    } else {
        10
    };

    if base == 10 && (input.contains('b') || input.contains('f')) {
        return Err(nom::Err::Error(nom::error::Error::new(
            input,
            nom::error::ErrorKind::Tag,
        )));
    }

    let value = i64::from_str_radix(
        if base == 16 {
            &value_str[2..]
        } else {
            value_str
        },
        base,
    )
    .unwrap();

    Ok((input, if signed.is_some() { -value } else { value }))
}

fn parse_label(input: &str) -> IResult<&str, String> {
    map(
        take_while1(|c: char| c.is_alphanumeric() || c == '_' || c == '.' || c == '$'),
        |s: &str| s.to_string(),
    )
    .parse(input)
}

fn parse_operand_label(input: &str) -> IResult<&str, String> {
    let (input, label) = take_while1(|c: char| {
        c.is_alphanumeric() || c == '_' || c == '.' || c == '$' || c == 'f' || c == 'b' || c == '@'
    })(input)?;

    if label.starts_with("0x") || label.starts_with("0X") {
        return Err(nom::Err::Error(nom::error::Error::new(
            input,
            nom::error::ErrorKind::Tag,
        )));
    }

    let is_numeric_prefix = label.chars().next().unwrap_or(' ').is_numeric();
    if is_numeric_prefix && !label.ends_with('f') && !label.ends_with('b') {
        return Err(nom::Err::Error(nom::error::Error::new(
            input,
            nom::error::ErrorKind::Tag,
        )));
    }

    Ok((input, label.to_string()))
}

fn parse_operand(input: &str) -> IResult<&str, Operand> {
    alt((
        map(parse_memory_operand, Operand::Memory),
        map(parse_register, Operand::Register),
        map(parse_fence_mode_operand, Operand::FenceMode),
        map(parse_immediate, Operand::Immediate),
        map(parse_operand_label, Operand::Label),
    ))
    .parse(input)
}

fn parse_fence_mode_operand(input: &str) -> IResult<&str, FenceMode> {
    alt((
        value(FenceMode::InputOutputReadWrite, tag("iorw")),
        value(FenceMode::OutputWrite, tag("ow")),
        value(FenceMode::ReadWrite, tag("rw")),
        value(FenceMode::Read, tag("r")),
        value(FenceMode::Write, tag("w")),
    ))
    .parse(input)
}

fn parse_operands(input: &str) -> IResult<&str, Vec<Operand>> {
    separated_list0((space0, char(','), space0), preceded(space0, parse_operand)).parse(input)
}

fn parse_directive(input: &str) -> IResult<&str, Directive> {
    preceded(
        char('.'),
        alt((
            value(Directive::CFIStartProc, tag("cfi_startproc")),
            value(Directive::CFIEndProc, tag("cfi_endproc")),
            map(
                (
                    tag("size"),
                    space1,
                    parse_label,
                    char(','),
                    space0,
                    recognize(take_till(|c| c == '\n' || c == '\r')),
                ),
                |(_, _, name, _, _, value)| Directive::Size(name, value.to_string()),
            ),
            map(
                (
                    tag("align"),
                    space1,
                    map_res(digit1, |s: &str| s.parse::<u32>()),
                ),
                |(_, _, align)| Directive::Align(align),
            ),
            map(
                preceded(
                    tag("p2align"),
                    preceded(
                        space1,
                        (
                            map_res(digit1, |s: &str| s.parse::<u32>()),
                            opt(preceded(
                                (char(','), space0),
                                opt(map_res(digit1, |s: &str| s.parse::<u32>())),
                            )),
                            opt(preceded(
                                (char(','), space0),
                                opt(map_res(digit1, |s: &str| s.parse::<u32>())),
                            )),
                        ),
                    ),
                ),
                |(p1, opt_p2, opt_p3)| {
                    let p2 = opt_p2.flatten().unwrap_or(0);
                    let p3 = opt_p3.flatten().unwrap_or(0);
                    Directive::P2Align(p1, p2, p3)
                },
            ),
            map(
                (
                    tag("type"),
                    space1,
                    parse_label,
                    char(','),
                    space0,
                    recognize(take_till(|c| c == '\n' || c == '\r')),
                ),
                |(_, _, name, _, _, value)| Directive::Type(name, value.to_string()),
            ),
        )),
    )
    .parse(input)
}

fn parse_label_def(input: &str) -> IResult<&str, RiscvInstruction> {
    map(terminated(parse_label, char(':')), |label| {
        RiscvInstruction::Label(label)
    })
    .parse(input)
}

fn parse_instruction_name(input: &str) -> IResult<&str, String> {
    map(
        take_while1(|c: char| c.is_alphanumeric() || c == '.'),
        |s: &str| s.to_string(),
    )
    .parse(input)
}

fn is_load_instruction(name: &str) -> Option<Size> {
    match name.to_lowercase().as_str() {
        "lb" => Some(Size::Byte),
        "lh" => Some(Size::Half),
        "lw" => Some(Size::Word),
        "ld" => Some(Size::Double),
        _ => None,
    }
}

fn is_store_instruction(name: &str) -> Option<Size> {
    match name.to_lowercase().as_str() {
        "sb" => Some(Size::Byte),
        "sh" => Some(Size::Half),
        "sw" => Some(Size::Word),
        "sd" => Some(Size::Double),
        _ => None,
    }
}

fn is_unsigned_load_instruction(name: &str) -> Option<Size> {
    match name.to_lowercase().as_str() {
        "lbu" => Some(Size::Byte),
        "lhu" => Some(Size::Half),
        "lwu" => Some(Size::Word),
        "ldu" => Some(Size::Double),
        _ => None,
    }
}

fn map_instruction(name: &str, operands: Vec<Operand>) -> RiscvInstruction {
    let name_lower = name.to_lowercase();

    match name_lower.as_str() {
        "beq" | "bne" | "blt" | "bge" | "bgtu" | "bltu" | "bgeu" | "bleu" => {
            let op = match name_lower.as_str() {
                "beq" => ComparisonOp::Eq,
                "bne" => ComparisonOp::Ne,
                "blt" => ComparisonOp::Lt,
                "bge" => ComparisonOp::Ge,
                "bltu" => ComparisonOp::Ltu,
                "bgeu" => ComparisonOp::Geu,
                "bgtu" => ComparisonOp::Gtu,
                "bleu" => ComparisonOp::Leu,
                _ => unreachable!(),
            };
            if operands.len() == 3 {
                if let (Operand::Register(rs1), Operand::Register(rs2), Operand::Label(label)) = (
                    operands[0].clone(),
                    operands[1].clone(),
                    operands[2].clone(),
                ) {
                    RiscvInstruction::Branch {
                        op,
                        rs1,
                        rs2,
                        label,
                    }
                } else {
                    RiscvInstruction::Unhandled(format!(
                        "Invalid operands for {}: {:?}",
                        name, operands
                    ))
                }
            } else {
                RiscvInstruction::Unhandled(format!(
                    "{} requires three operands, got {}",
                    name,
                    operands.len()
                ))
            }
        }
        "not" => {
            if operands.len() == 2 {
                if let (Operand::Register(rd), Operand::Register(rs)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    return RiscvInstruction::Not { rd, rs };
                }
            }
            RiscvInstruction::Unhandled(format!("Invalid operands for not: {:?}", operands))
        }
        "neg" => {
            if operands.len() == 2 {
                if let (Operand::Register(rd), Operand::Register(rs)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    return RiscvInstruction::Neg { rd, rs, size: None };
                }
            }
            RiscvInstruction::Unhandled(format!("Invalid operands for neg: {:?}", operands))
        }
        "negw" => {
            if operands.len() == 2 {
                if let (Operand::Register(rd), Operand::Register(rs)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    return RiscvInstruction::Neg {
                        rd,
                        rs,
                        size: Some(Size::Word),
                    };
                }
            }
            RiscvInstruction::Unhandled(format!("Invalid operands for negw: {:?}", operands))
        }
        "li" => {
            if operands.len() == 2 {
                if let (Operand::Register(rd), Operand::Immediate(value)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    RiscvInstruction::LoadImmidate {
                        register: rd,
                        value,
                    }
                } else {
                    RiscvInstruction::Unhandled(format!("Invalid operands for li: {:?}", operands))
                }
            } else {
                RiscvInstruction::Unhandled(format!(
                    "li requires two operands, got {}",
                    operands.len()
                ))
            }
        }
        "bnez" => {
            if operands.len() == 2 {
                if let (Operand::Register(rs), Operand::Label(label)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    RiscvInstruction::Branch {
                        op: ComparisonOp::Nez,
                        rs1: rs,
                        rs2: Register {
                            reg_type: RegisterType::Special("zero".to_string()),
                            number: None,
                        },
                        label,
                    }
                } else {
                    RiscvInstruction::Unhandled(format!(
                        "Invalid operands for bnez: {:?}",
                        operands
                    ))
                }
            } else {
                RiscvInstruction::Unhandled(format!(
                    "bnez requires two operands, got {}",
                    operands.len()
                ))
            }
        }
        "beqz" => {
            if operands.len() == 2 {
                if let (Operand::Register(rs), Operand::Label(label)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    RiscvInstruction::Branch {
                        op: ComparisonOp::Eq,
                        rs1: rs,
                        rs2: Register {
                            reg_type: RegisterType::Special("zero".to_string()),
                            number: None,
                        },
                        label,
                    }
                } else {
                    RiscvInstruction::Unhandled(format!(
                        "Invalid operands for beqz: {:?}",
                        operands
                    ))
                }
            } else {
                RiscvInstruction::Unhandled(format!(
                    "beqz requires two operands, got {}",
                    operands.len()
                ))
            }
        }
        "jal" => {
            if operands.len() == 2 {
                if let (Operand::Register(rd), Operand::Label(label)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    RiscvInstruction::Jump {
                        rd,
                        label: Some(label),
                    }
                } else {
                    RiscvInstruction::Unhandled(format!("Invalid operands for jal: {:?}", operands))
                }
            } else {
                RiscvInstruction::Unhandled(format!(
                    "jal requires two operands, got {}",
                    operands.len()
                ))
            }
        }
        "j" => {
            if operands.len() == 1 {
                if let Operand::Label(label) = operands[0].clone() {
                    RiscvInstruction::Jump {
                        rd: Register {
                            reg_type: RegisterType::Special("zero".to_string()),
                            number: None,
                        },
                        label: Some(label),
                    }
                } else {
                    RiscvInstruction::Unhandled(format!("Invalid operand for j: {:?}", operands[0]))
                }
            } else {
                RiscvInstruction::Unhandled(format!(
                    "j requires one operand, got {}",
                    operands.len()
                ))
            }
        }
        "jr" => {
            if operands.len() == 1 {
                if let Operand::Register(Register { reg_type, .. }) = operands[0].clone() {
                    RiscvInstruction::Jump {
                        rd: Register {
                            reg_type,
                            number: None,
                        },
                        label: None,
                    }
                } else {
                    RiscvInstruction::Unhandled(format!(
                        "Invalid operand for jr: {:?}",
                        operands[0]
                    ))
                }
            } else {
                RiscvInstruction::Unhandled(format!(
                    "jr requires one operand, got {}",
                    operands.len()
                ))
            }
        }
        "call" => {
            if operands.len() == 1 {
                if let Operand::Label(name) = operands[0].clone() {
                    RiscvInstruction::Call { label: name }
                } else {
                    RiscvInstruction::Unhandled(format!(
                        "Invalid operand for call: {:?}",
                        operands[0]
                    ))
                }
            } else {
                RiscvInstruction::Unhandled(format!(
                    "call requires one operand/label, got {}",
                    operands.len()
                ))
            }
        }
        "la" => {
            if operands.len() == 2 {
                if let (Operand::Register(register), Operand::Label(label)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    RiscvInstruction::LoadAddress { register, label }
                } else {
                    RiscvInstruction::Unhandled(format!("Invalid la operands: {:?}", operands))
                }
            } else {
                RiscvInstruction::Unhandled(format!(
                    "la requires two operands, got {}",
                    operands.len()
                ))
            }
        }
        "fence" => {
            if operands.len() == 2 {
                if let (Operand::FenceMode(pred), Operand::FenceMode(succ)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    RiscvInstruction::Fence { pred, succ }
                } else {
                    RiscvInstruction::Unhandled(format!("Invalid fence operands: {:?}", operands))
                }
            } else {
                RiscvInstruction::Unhandled(format!(
                    "Fence requires two operands, got {}",
                    operands.len()
                ))
            }
        }
        "ret" if operands.is_empty() => RiscvInstruction::Return,
        "mv" => {
            if operands.len() == 2 {
                if let (Operand::Register(dst), Operand::Register(src)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    RiscvInstruction::Move(dst, src)
                } else {
                    RiscvInstruction::Unhandled(format!("{} {:?}", name, operands))
                }
            } else {
                RiscvInstruction::Unhandled(format!("{} {:?}", name, operands))
            }
        }
        "sext.w" => {
            if operands.len() == 2 {
                if let (Operand::Register(dst), Operand::Register(src)) =
                    (operands[0].clone(), operands[1].clone())
                {
                    RiscvInstruction::SignExtendWord(dst, src)
                } else {
                    RiscvInstruction::Unhandled(format!("{} {:?}", name, operands))
                }
            } else {
                RiscvInstruction::Unhandled(format!("{} {:?}", name, operands))
            }
        }
        _ => {
            if let Some(size) = is_load_instruction(&name_lower) {
                if operands.len() == 2 {
                    if let (Operand::Register(dst), Operand::Memory(src)) =
                        (operands[0].clone(), operands[1].clone())
                    {
                        return RiscvInstruction::Load { size, dst, src };
                    } else {
                        return RiscvInstruction::Unhandled(format!(
                            "Invalid operands for load: {:?}",
                            operands
                        ));
                    }
                } else {
                    return RiscvInstruction::Unhandled(format!(
                        "Invalid operands for load: {:?}",
                        operands
                    ));
                }
            }
            if let Some(size) = is_unsigned_load_instruction(&name_lower) {
                if operands.len() == 2 {
                    if let (Operand::Register(dst), Operand::Memory(src)) =
                        (operands[0].clone(), operands[1].clone())
                    {
                        return RiscvInstruction::UnsignedLoad { size, dst, src };
                    } else {
                        return RiscvInstruction::Unhandled(format!(
                            "Invalid operands for load: {:?}",
                            operands
                        ));
                    }
                } else {
                    return RiscvInstruction::Unhandled(format!(
                        "Invalid operands for load: {:?}",
                        operands
                    ));
                }
            } else if let Some(size) = is_store_instruction(&name_lower) {
                if operands.len() == 2 {
                    if let (Operand::Register(src), Operand::Memory(dst)) =
                        (operands[0].clone(), operands[1].clone())
                    {
                        return RiscvInstruction::Store { size, src, dst };
                    } else {
                        return RiscvInstruction::Unhandled(format!(
                            "Invalid operands for store:{:?}",
                            operands
                        ));
                    }
                } else {
                    return RiscvInstruction::Unhandled(format!(
                        "Invalid operands for store: {:?}",
                        operands
                    ));
                }
            } else if name_lower.starts_with("lr")
                || name_lower.starts_with("sc")
                || name_lower.starts_with("amo")
            {
                let parts: Vec<&str> = name_lower.split('.').collect();
                if parts.len() >= 2 {
                    let base_op = parts[0];
                    let size_str = parts[1];
                    let semantics_str = if parts.len() > 2 { parts[2] } else { "" };

                    let size = match size_str {
                        "b" => Size::Byte,
                        "h" => Size::Half,
                        "w" => Size::Word,
                        "d" => Size::Double,
                        _ => {
                            return RiscvInstruction::Unhandled(format!("{} {:?}", name, operands));
                        }
                    };

                    let semantics = match semantics_str {
                        "aq" => AtomicSemantics::Acquire,
                        "rl" => AtomicSemantics::Release,
                        "aqrl" => AtomicSemantics::AcquireRelease,
                        "" => AtomicSemantics::None,
                        _ => {
                            return RiscvInstruction::Unhandled(format!("{} {:?}", name, operands));
                        }
                    };

                    if base_op == "lr" {
                        if operands.len() == 2 {
                            if let (Operand::Register(rd), Operand::Memory(addr)) =
                                (operands[0].clone(), operands[1].clone())
                            {
                                return RiscvInstruction::LoadReserved {
                                    size,
                                    semantics,
                                    rd,
                                    addr,
                                };
                            }
                        }
                    } else if base_op == "sc" {
                        if operands.len() == 3 {
                            if let (
                                Operand::Register(rd),
                                Operand::Register(rs2),
                                Operand::Memory(addr),
                            ) = (
                                operands[0].clone(),
                                operands[1].clone(),
                                operands[2].clone(),
                            ) {
                                return RiscvInstruction::StoreConditional {
                                    size,
                                    semantics,
                                    rd,
                                    rs2,
                                    addr,
                                };
                            }
                        }
                    } else if base_op.starts_with("amo") {
                        let amo_op_str = &base_op[3..];
                        let op = match amo_op_str {
                            "swap" => AtomicOp::Swap,
                            "add" => AtomicOp::Add,
                            "and" => AtomicOp::And,
                            "or" => AtomicOp::Or,
                            "xor" => AtomicOp::Xor,
                            "max" => AtomicOp::Max,
                            "min" => AtomicOp::Min,
                            _ => {
                                return RiscvInstruction::Unhandled(format!(
                                    "{} {:?}",
                                    name, operands
                                ));
                            }
                        };
                        if operands.len() == 3 {
                            if let (
                                Operand::Register(rd),
                                Operand::Register(rs2),
                                Operand::Memory(addr),
                            ) = (
                                operands[0].clone(),
                                operands[1].clone(),
                                operands[2].clone(),
                            ) {
                                return RiscvInstruction::Atomic {
                                    op,
                                    size,
                                    semantics,
                                    rd,
                                    rs2,
                                    addr,
                                };
                            }
                        }
                    }
                    return RiscvInstruction::Unhandled(format!("{} {:?}", name, operands));
                } else {
                    return RiscvInstruction::Unhandled(format!(
                        "Invalid operands for load: {:?}",
                        operands
                    ));
                }
            } else {
                if name_lower.ends_with('w') {
                    let base_name = name_lower.trim_end_matches('w');

                    match base_name {
                        "add" | "sub" | "mul" | "and" | "or" | "xor" | "sll" | "srl" | "sra" => {
                            if operands.len() == 3 {
                                if let (
                                    Operand::Register(rd),
                                    Operand::Register(rs1),
                                    Operand::Register(rs2),
                                ) = (
                                    operands[0].clone(),
                                    operands[1].clone(),
                                    operands[2].clone(),
                                ) {
                                    let op = match base_name {
                                        "add" => ArithmeticOp::Add,
                                        "sub" => ArithmeticOp::Sub,
                                        "mul" => ArithmeticOp::Mul,
                                        "and" => ArithmeticOp::And,
                                        "or" => ArithmeticOp::Or,
                                        "xor" => ArithmeticOp::Xor,
                                        "sll" => ArithmeticOp::Sll,
                                        "srl" => ArithmeticOp::Srl,
                                        "sra" => ArithmeticOp::Sra,
                                        _ => unreachable!(),
                                    };
                                    return RiscvInstruction::ArithmeticRR {
                                        op,
                                        rd,
                                        rs1,
                                        rs2,
                                        size: Some(Size::Word),
                                    };
                                }
                            }
                            return RiscvInstruction::Unhandled(format!("{} {:?}", name, operands));
                        }
                        _ => {}
                    }
                }

                if name_lower.ends_with("iw") {
                    let base_name = name_lower.trim_end_matches("iw");

                    match base_name {
                        "add" | "and" | "or" | "xor" | "sll" | "srl" | "sra" => {
                            if operands.len() == 3 {
                                if let (
                                    Operand::Register(rd),
                                    Operand::Register(rs1),
                                    Operand::Immediate(imm),
                                ) = (
                                    operands[0].clone(),
                                    operands[1].clone(),
                                    operands[2].clone(),
                                ) {
                                    let op = match base_name {
                                        "add" => ArithmeticOp::Add,
                                        "and" => ArithmeticOp::And,
                                        "or" => ArithmeticOp::Or,
                                        "xor" => ArithmeticOp::Xor,
                                        "sll" => ArithmeticOp::Sll,
                                        "srl" => ArithmeticOp::Srl,
                                        "sra" => ArithmeticOp::Sra,
                                        _ => unreachable!(),
                                    };
                                    return RiscvInstruction::ArithmeticRI {
                                        op,
                                        rd,
                                        rs1,
                                        imm,
                                        size: Some(Size::Word),
                                    };
                                }
                            }
                            return RiscvInstruction::Unhandled(format!("{} {:?}", name, operands));
                        }
                        _ => {}
                    }
                }
                match name_lower.as_str() {
                    "add" | "sub" | "mul" | "and" | "or" | "xor" | "sll" | "srl" | "sra" => {
                        if operands.len() == 3 {
                            if let (
                                Operand::Register(rd),
                                Operand::Register(rs1),
                                Operand::Register(rs2),
                            ) = (
                                operands[0].clone(),
                                operands[1].clone(),
                                operands[2].clone(),
                            ) {
                                let op = match name_lower.as_str() {
                                    "add" => ArithmeticOp::Add,
                                    "sub" => ArithmeticOp::Sub,
                                    "mul" => ArithmeticOp::Mul,
                                    "and" => ArithmeticOp::And,
                                    "or" => ArithmeticOp::Or,
                                    "xor" => ArithmeticOp::Xor,
                                    "sll" => ArithmeticOp::Sll,
                                    "srl" => ArithmeticOp::Srl,
                                    "sra" => ArithmeticOp::Sra,
                                    _ => unreachable!(),
                                };
                                return RiscvInstruction::ArithmeticRR {
                                    op,
                                    rd,
                                    rs1,
                                    rs2,
                                    size: None,
                                };
                            }
                        }
                        RiscvInstruction::Unhandled(format!("{} {:?}", name, operands))
                    }
                    "addi" | "andi" | "ori" | "xori" | "slli" | "srli" | "srai" => {
                        if operands.len() == 3 {
                            if let (
                                Operand::Register(rd),
                                Operand::Register(rs1),
                                Operand::Immediate(imm),
                            ) = (
                                operands[0].clone(),
                                operands[1].clone(),
                                operands[2].clone(),
                            ) {
                                let op = match name_lower.as_str() {
                                    "addi" => ArithmeticOp::Add,
                                    "andi" => ArithmeticOp::And,
                                    "ori" => ArithmeticOp::Or,
                                    "xori" => ArithmeticOp::Xor,
                                    "slli" => ArithmeticOp::Sll,
                                    "srli" => ArithmeticOp::Srl,
                                    "srai" => ArithmeticOp::Sra,
                                    _ => unreachable!(),
                                };
                                return RiscvInstruction::ArithmeticRI {
                                    op,
                                    rd,
                                    rs1,
                                    imm,
                                    size: None,
                                };
                            }
                        }
                        RiscvInstruction::Unhandled(format!("{} {:?}", name, operands))
                    }
                    _ => RiscvInstruction::Unhandled(format!("{} {:?}", name, operands)),
                }
            }
        }
    }
}

fn parse_instruction(input: &str) -> IResult<&str, RiscvInstruction> {
    let (input, name) = parse_instruction_name(input)?;
    let (input, operands) = parse_operands(input)?;
    let instr = map_instruction(&name, operands);
    Ok((input, instr))
}

fn parse_line(input: &str) -> IResult<&str, Option<RiscvInstruction>> {
    if input.is_empty() {
        return Err(nom::Err::Error(nom::error::Error::new(
            input,
            nom::error::ErrorKind::Eof,
        )));
    }

    let (input, line) = take_till(|c| c == '\n')(input)?;
    let (input, _) = opt(char('\n')).parse(input)?;
    let line_without_comment = line.split('#').next().unwrap().trim();

    if line_without_comment.is_empty() {
        Ok((input, None))
    } else {
        let result = alt((
            parse_label_def,
            map(parse_directive, |dir| RiscvInstruction::Directive(dir)),
            parse_instruction,
        ))
        .parse(line_without_comment);

        match result {
            Ok((remaining, instr)) if remaining.is_empty() => Ok((input, Some(instr))),
            _ => Ok((input, Some(RiscvInstruction::Unhandled(line.to_string())))),
        }
    }
}

pub fn parse_riscv_assembly(input: &str) -> IResult<&str, Vec<RiscvInstruction>> {
    let (input, _) = multispace0(input)?;
    let (input, options) = many0(parse_line).parse(input)?;
    let instructions: Vec<RiscvInstruction> = options.into_iter().flatten().collect();
    Ok((input, instructions))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_register() {
        let (_, reg) = parse_register("a0").unwrap();
        assert_eq!(
            reg,
            Register {
                reg_type: RegisterType::A,
                number: Some(0)
            }
        );
    }

    #[test]
    fn test_parse_memory_operand() {
        let (_, mem) = parse_memory_operand("0(a0)").unwrap();
        assert_eq!(
            mem,
            MemoryOperand {
                offset: 0,
                base: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                }
            }
        );
    }

    #[test]
    fn test_parse_fence() {
        let (_, instr) = parse_instruction("fence rw,w").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::Fence {
                pred: FenceMode::ReadWrite,
                succ: FenceMode::Write,
            }
        );
    }

    #[test]
    fn test_parse_store() {
        let (_, instr) = parse_instruction("sd a1,0(a0)").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::Store {
                size: Size::Double,
                src: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                dst: MemoryOperand {
                    offset: 0,
                    base: Register {
                        reg_type: RegisterType::A,
                        number: Some(0)
                    }
                },
            }
        );
    }

    #[test]
    fn test_parse_atomic() {
        let (_, instr) = parse_instruction("amoswap.d.aqrl a1,a1,0(a0)").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::Atomic {
                op: AtomicOp::Swap,
                size: Size::Double,
                semantics: AtomicSemantics::AcquireRelease,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                rs2: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                addr: MemoryOperand {
                    offset: 0,
                    base: Register {
                        reg_type: RegisterType::A,
                        number: Some(0)
                    }
                },
            }
        );
    }

    #[test]
    fn test_parse_label() {
        let input = "label:";
        let (_, instruction) = parse_riscv_assembly(input).unwrap();
        assert_eq!(instruction[0], RiscvInstruction::Label("label".to_string()));

        let input = ".label:";
        let (_, instruction) = parse_riscv_assembly(input).unwrap();
        assert_eq!(
            instruction[0],
            RiscvInstruction::Label(".label".to_string())
        );
    }

    #[test]
    fn test_parse_labels() {
        let input = r#"
my_label:
.local_label:
func.123:
_start:
.label$with$dollars:
    "#;
        let (_, instructions) = parse_riscv_assembly(input).unwrap();
        assert_eq!(instructions.len(), 5);
        assert_eq!(
            instructions[0],
            RiscvInstruction::Label("my_label".to_string())
        );
        assert_eq!(
            instructions[1],
            RiscvInstruction::Label(".local_label".to_string())
        );
        assert_eq!(
            instructions[2],
            RiscvInstruction::Label("func.123".to_string())
        );
        assert_eq!(
            instructions[3],
            RiscvInstruction::Label("_start".to_string())
        );
        assert_eq!(
            instructions[4],
            RiscvInstruction::Label(".label$with$dollars".to_string())
        );
    }

    #[test]
    fn test_parse_full_assembly() {
        let input = r#"
vatomic64_write:
    fence rw,w
    sd a1,0(a0)
    .inner_label:
    fence rw,rw
    ret
        "#;
        let (_, instructions) = parse_riscv_assembly(input).unwrap();
        assert_eq!(instructions.len(), 6);
        assert_eq!(
            instructions[0],
            RiscvInstruction::Label("vatomic64_write".to_string())
        );
        assert_eq!(
            instructions[1],
            RiscvInstruction::Fence {
                pred: FenceMode::ReadWrite,
                succ: FenceMode::Write,
            }
        );
    }

    #[test]
    fn test_parse_load_reserved() {
        let (_, instr) = parse_instruction("lr.w.aq a0,0(a1)").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::LoadReserved {
                size: Size::Word,
                semantics: AtomicSemantics::Acquire,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                addr: MemoryOperand {
                    offset: 0,
                    base: Register {
                        reg_type: RegisterType::A,
                        number: Some(1)
                    }
                },
            }
        );
    }

    #[test]
    fn test_parse_store_conditional() {
        let (_, instr) = parse_instruction("sc.d.rl a0,a2,0(a1)").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::StoreConditional {
                size: Size::Double,
                semantics: AtomicSemantics::Release,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                rs2: Register {
                    reg_type: RegisterType::A,
                    number: Some(2)
                },
                addr: MemoryOperand {
                    offset: 0,
                    base: Register {
                        reg_type: RegisterType::A,
                        number: Some(1)
                    }
                },
            }
        );
    }

    #[test]
    fn test_parse_atomic_swap() {
        let (_, instr) = parse_instruction("amoswap.d.aqrl a1,a2,0(a0)").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::Atomic {
                op: AtomicOp::Swap,
                size: Size::Double,
                semantics: AtomicSemantics::AcquireRelease,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                rs2: Register {
                    reg_type: RegisterType::A,
                    number: Some(2)
                },
                addr: MemoryOperand {
                    offset: 0,
                    base: Register {
                        reg_type: RegisterType::A,
                        number: Some(0)
                    }
                },
            }
        );
    }

    #[test]
    fn test_parse_arithmetic_rr() {
        let (_, instr) = parse_instruction("add a0,a1,a2").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::ArithmeticRR {
                op: ArithmeticOp::Add,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                rs1: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                rs2: Register {
                    reg_type: RegisterType::A,
                    number: Some(2)
                },
                size: None,
            }
        );
    }

    #[test]
    fn test_parse_arithmetic_ri() {
        let (_, instr) = parse_instruction("addi a0,a1,42").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::ArithmeticRI {
                op: ArithmeticOp::Add,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                rs1: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                imm: 42,
                size: None,
            }
        );
    }

    #[test]
    fn test_parse_branch() {
        let (_, instr) = parse_instruction("beq a0, a1, my_label").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::Branch {
                op: ComparisonOp::Eq,
                rs1: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                rs2: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                label: "my_label".to_string(),
            }
        );
    }

    #[test]
    fn test_parse_branch_pseudo() {
        let (_, instr) = parse_instruction("bnez a0, my_label").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::Branch {
                op: ComparisonOp::Nez,
                rs1: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                rs2: Register {
                    reg_type: RegisterType::Special("zero".to_string()),
                    number: None,
                },
                label: "my_label".to_string(),
            }
        );
    }

    #[test]
    fn test_parse_jump() {
        let (_, instr) = parse_instruction("jal ra, my_function").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::Jump {
                rd: Register {
                    reg_type: RegisterType::Special("ra".to_string()),
                    number: None,
                },
                label: Some("my_function".to_string()),
            }
        );
    }

    #[test]
    fn test_parse_pseudo_jump() {
        let (_, instr) = parse_instruction("j my_label").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::Jump {
                rd: Register {
                    reg_type: RegisterType::Special("zero".to_string()),
                    number: None,
                },
                label: Some("my_label".to_string()),
            }
        );
    }

    #[test]
    fn test_parse_instruction_with_comment() {
        let input = "beq a0, a1, my_label # branch if equal\n";
        let (_, instructions) = parse_riscv_assembly(input).unwrap();
        assert_eq!(instructions.len(), 1);
        assert_eq!(
            instructions[0],
            RiscvInstruction::Branch {
                op: ComparisonOp::Eq,
                rs1: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                rs2: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                label: "my_label".to_string(),
            }
        );
    }

    #[test]
    fn test_parse_amoswap() {
        let (_, instr) = parse_instruction("amoswap.w.rl a1,a1,0(a0)").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::Atomic {
                op: AtomicOp::Swap,
                size: Size::Word,
                semantics: AtomicSemantics::Release,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                rs2: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                addr: MemoryOperand {
                    offset: 0,
                    base: Register {
                        reg_type: RegisterType::A,
                        number: Some(0)
                    }
                },
            }
        );
    }

    #[test]
    fn test_parse_amoadd() {
        let (_, instr) = parse_instruction("amoadd.w.aqrl a5,a1,0(a0)").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::Atomic {
                op: AtomicOp::Add,
                size: Size::Word,
                semantics: AtomicSemantics::AcquireRelease,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(5)
                },
                rs2: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                addr: MemoryOperand {
                    offset: 0,
                    base: Register {
                        reg_type: RegisterType::A,
                        number: Some(0)
                    }
                },
            }
        );
    }

    #[test]
    fn test_parse_hex_immediate() {
        let (_, instr) = parse_instruction("andi a0, a5, 0xff").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::ArithmeticRI {
                op: ArithmeticOp::And,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                rs1: Register {
                    reg_type: RegisterType::A,
                    number: Some(5)
                },
                imm: 0xff,
                size: None,
            }
        );

        let (_, instr) = parse_instruction("addi a0, a1, 0xFF").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::ArithmeticRI {
                op: ArithmeticOp::Add,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                rs1: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                imm: 0xFF,
                size: None,
            }
        );

        let (_, instr) = parse_instruction("addi a0, a1, 0x10").unwrap();
        assert_eq!(
            instr,
            RiscvInstruction::ArithmeticRI {
                op: ArithmeticOp::Add,
                rd: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                rs1: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                imm: 16,
                size: None,
            }
        );
    }

    #[test]
    fn test_parse_numeric_local_labels() {
        let (_, instructions) = parse_riscv_assembly("1f:\n  beq a0, a1, 1f").unwrap();
        assert_eq!(instructions.len(), 2);
        assert_eq!(instructions[0], RiscvInstruction::Label("1f".to_string()));
        assert_eq!(
            instructions[1],
            RiscvInstruction::Branch {
                op: ComparisonOp::Eq,
                rs1: Register {
                    reg_type: RegisterType::A,
                    number: Some(0)
                },
                rs2: Register {
                    reg_type: RegisterType::A,
                    number: Some(1)
                },
                label: "1f".to_string(),
            }
        );
    }

    #[test]
    fn test_parse_multiline_assembly() {
        let input = r#"
main:
    addi sp, sp, -16
    sd ra, 8(sp)
    li a0, 10
    li a1, 20
    add a2, a0, a1
loop_start:
    addi a0, a0, -1
    bnez a0, loop_start

    mv a0, a2 
    ld ra, 8(sp)
    addi sp, sp, 16
    ret
"#;

        let (remaining, instructions) = parse_riscv_assembly(input).unwrap();

        assert!(remaining.is_empty(), "Parser did not consume all input");

        assert_eq!(
            instructions.len(),
            13,
            "Incorrect number of instructions parsed"
        );

        assert!(
            instructions
                .iter()
                .any(|instr| matches!(instr, RiscvInstruction::Label(label) if label == "main")),
            "Main label not found"
        );

        let arithmetic_count = instructions
            .iter()
            .filter(|instr| {
                matches!(
                    instr,
                    RiscvInstruction::ArithmeticRI { .. } | RiscvInstruction::ArithmeticRR { .. }
                )
            })
            .count();
        assert_eq!(
            arithmetic_count, 4,
            "Incorrect number of arithmetic instructions"
        );

        let has_branch = instructions.iter().any(|instr| {
            matches!(
                instr,
                RiscvInstruction::Branch {
                    op: ComparisonOp::Nez,
                    ..
                }
            )
        });
        assert!(has_branch, "Branch instruction not found");

        let has_return = instructions
            .iter()
            .any(|instr| matches!(instr, RiscvInstruction::Return));
        assert!(has_return, "Return instruction not found");
    }
}
