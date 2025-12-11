/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
use nom::{
    IResult, Parser,
    branch::alt,
    bytes::complete::{tag, take_till, take_while, take_while1},
    character::complete::{char, digit1, multispace0, space0, space1},
    combinator::{map, map_res, opt, recognize, value},
    multi::{many0, separated_list0},
    sequence::{delimited, preceded, terminated},
};

use super::*;

fn parse_register_type(input: &str) -> IResult<&str, RegisterType> {
    alt((
        value(RegisterType::X, char('x')),
        value(RegisterType::W, char('w')),
        value(RegisterType::V, char('v')),
        value(RegisterType::Q, char('q')),
        value(RegisterType::D, char('d')),
        value(RegisterType::S, char('s')),
        value(RegisterType::H, char('h')),
        value(RegisterType::B, char('b')),
    ))
    .parse(input)
}

fn parse_register(input: &str) -> IResult<&str, Register> {
    alt((
        map(tag("sp"), |_| Register {
            reg_type: RegisterType::SP,
            number: 0,
        }),
        map(
            (
                parse_register_type,
                map_res(digit1, |s: &str| s.parse::<u8>()),
            ),
            |(reg_type, number)| Register { reg_type, number },
        ),
    ))
    .parse(input)
}

fn parse_immediate(input: &str) -> IResult<&str, i64> {
    let (input, _) = alt((tag("#"), tag(""))).parse(input)?;
    let (input, signed) = opt(char('-')).parse(input)?;
    let (input, value_str) = alt((
        recognize((
            alt((tag("0x"), tag("0X"))),
            take_while1(|c: char| c.is_digit(16)),
        )),
        recognize(digit1),
    ))
    .parse(input)?;

    let base = if value_str.starts_with("0x") || value_str.starts_with("0X") {
        16
    } else {
        10
    };

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
        recognize((
            take_while1(|c: char| c.is_alphabetic() || c == '_' || c == '.'),
            take_while(|c: char| c.is_alphanumeric() || c == '_' || c == '.' || c == '$'),
        )),
        |s: &str| s.to_string(),
    )
    .parse(input)
}

fn parse_shift_operation(input: &str) -> IResult<&str, String> {
    map(
        alt((tag("lsl"), tag("lsr"), tag("asr"), tag("ror"))),
        |s: &str| s.to_string(),
    )
    .parse(input)
}

fn parse_condition_code(input: &str) -> Option<ConditionCode> {
    // We strip the dot to support branching like b<cond> and b.<cond>
    let trimmed = input.strip_prefix('.').unwrap_or(input);
    match trimmed {
        "eq" => Some(ConditionCode::EQ),
        "ne" => Some(ConditionCode::NE),
        "cs" => Some(ConditionCode::CS),
        "cc" => Some(ConditionCode::CC),
        "mi" => Some(ConditionCode::MI),
        "pl" => Some(ConditionCode::PL),
        "vs" => Some(ConditionCode::VS),
        "vc" => Some(ConditionCode::VC),
        "hi" => Some(ConditionCode::HI),
        "hs" => Some(ConditionCode::HS),
        "ls" => Some(ConditionCode::LS),
        "lo" => Some(ConditionCode::LO),
        "ge" => Some(ConditionCode::GE),
        "lt" => Some(ConditionCode::LT),
        "gt" => Some(ConditionCode::GT),
        "le" => Some(ConditionCode::LE),
        "al" => Some(ConditionCode::AL),
        "nv" => Some(ConditionCode::NV),
        _ => None,
    }
}

fn parse_fence_mode(input: &str) -> IResult<&str, FenceType> {
    alt((
        value(FenceType::SY, tag("sy")),
        value(FenceType::LD, tag("ld")),
    ))
    .parse(input)
}

fn parse_shifted_register(input: &str) -> IResult<&str, Operand> {
    map(
        (
            parse_register,
            preceded((char(','), space0), parse_shift_operation),
            preceded((space0, char('#'), space0), parse_immediate),
        ),
        |(reg, shift_op, amount)| Operand::ShiftedRegister(reg, shift_op, amount),
    )
    .parse(input)
}

fn parse_register_list(input: &str) -> IResult<&str, Operand> {
    map(
        delimited(
            char('{'),
            separated_list0((char(','), space0), parse_register),
            char('}'),
        ),
        Operand::RegisterList,
    )
    .parse(input)
}

fn parse_addressing_mode(input: &str) -> IResult<&str, AddressingMode> {
    alt((
        // [Xn] - Base register only
        map(
            delimited(char('['), parse_register, char(']')),
            AddressingMode::BaseRegister,
        ),
        // [Xn, #imm]! - Pre-indexed with writeback
        map(
            terminated(
                delimited(
                    char('['),
                    (
                        parse_register,
                        preceded((char(','), space0), parse_immediate),
                    ),
                    char(']'),
                ),
                char('!'),
            ),
            |(reg, imm)| AddressingMode::PreIndexed(reg, imm),
        ),
        // [Xn, #imm] - Base register with offset
        map(
            delimited(
                char('['),
                (
                    parse_register,
                    preceded((char(','), space0), parse_immediate),
                ),
                char(']'),
            ),
            |(reg, imm)| AddressingMode::BaseRegisterWithOffset(reg, imm),
        ),
        // [Xn], #imm
        map(
            (
                delimited(char('['), parse_register, char(']')),
                preceded((char(','), space0), parse_immediate),
            ),
            |(reg, offset)| AddressingMode::PostIndexed(reg, offset),
        ),
    ))
    .parse(input)
}

fn parse_operand(input: &str) -> IResult<&str, Operand> {
    alt((
        map(parse_addressing_mode, Operand::Memory),
        map(parse_immediate, Operand::ImmediateValue),
        map(parse_register, Operand::Register),
        parse_shifted_register,
        parse_register_list,
        map(parse_label, Operand::Label),
        map(parse_fence_mode, Operand::FenceMode),
    ))
    .parse(input)
}

fn parse_operands(input: &str) -> IResult<&str, Vec<Operand>> {
    separated_list0((char(','), space0), preceded(space0, parse_operand)).parse(input)
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

pub fn parse_label_def(input: &str) -> IResult<&str, ArmInstruction> {
    map(terminated(parse_label, char(':')), |label| {
        ArmInstruction::Label(label)
    })
    .parse(input)
}

fn parse_memory_size_from_suffix(suffix: &str, operands: &[Operand]) -> MemorySize {
    match suffix {
        "b" => MemorySize::Byte,
        "h" => MemorySize::Half,
        _ if operands.iter().any(|op| {
                if let Operand::Register(reg) = op { reg.reg_type == RegisterType::W } else { false } 
            }) => MemorySize::Word,
        _ => MemorySize::Double,
    }
}

fn parse_fence_instruction(
    instr_name: &str,
    operands: Vec<Operand>,
) -> IResult<&str, ArmInstruction> {
    if instr_name.to_lowercase().as_str() != "dmb" {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Tag,
        )));
    }

    let mode = operands
        .iter()
        .find_map(|op| match op {
            Operand::FenceMode(mode) => Some(*mode),
            _ => None,
        })
        .unwrap_or(FenceType::SY);

    Ok(("", ArmInstruction::Dmb(mode)))
}

fn parse_arithmetic_instruction(
    instr_name: &str,
    operands: Vec<Operand>,
) -> IResult<&str, ArmInstruction> {
    if operands.len() < 2 {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Eof,
        )));
    }

    let op = match instr_name.to_lowercase().as_str() {
        "add" => ArithmeticOp::Add,
        "sub" => ArithmeticOp::Sub,
        "mul" => ArithmeticOp::Mul,
        "and" => ArithmeticOp::And,
        "orr" => ArithmeticOp::Orr,
        "eor" => ArithmeticOp::Eor,
        _ => {
            return Err(nom::Err::Error(nom::error::Error::new(
                "",
                nom::error::ErrorKind::Tag,
            )));
        }
    };

    let third_operand = if operands.len() >= 3 {
        Some(operands[2].clone())
    } else {
        None
    };

    Ok((
        "",
        ArmInstruction::Arithmetic(op, operands[0].clone(), operands[1].clone(), third_operand),
    ))
}

fn parse_shift_instruction(
    instr_name: &str,
    operands: Vec<Operand>,
) -> IResult<&str, ArmInstruction> {
    if operands.len() != 2 {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Count,
        )));
    }

    let op = match instr_name.to_lowercase().as_str() {
        "lsl" => ShiftOp::Lsl,
        "lsr" => ShiftOp::Lsr,
        "asr" => ShiftOp::Asr,
        _ => {
            return Err(nom::Err::Error(nom::error::Error::new(
                "",
                nom::error::ErrorKind::Tag,
            )));
        }
    };

    Ok((
        "",
        ArmInstruction::Shift(op, operands[0].clone(), operands[1].clone()),
    ))
}

fn parse_move_instruction(
    instr_name: &str,
    operands: Vec<Operand>,
) -> IResult<&str, ArmInstruction> {
    if operands.len() != 2 {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Count,
        )));
    }

    let op = match instr_name.to_lowercase().as_str() {
        "mov" => MoveOp::Mov,
        "mvn" => MoveOp::Mvn,
        "neg" => MoveOp::Neg,
        _ => {
            return Err(nom::Err::Error(nom::error::Error::new(
                "",
                nom::error::ErrorKind::Tag,
            )));
        }
    };

    Ok((
        "",
        ArmInstruction::Move(op, operands[0].clone(), operands[1].clone()),
    ))
}

fn parse_memory_attrs(op: MemoryOp, instr_name: &str, operands: &[Operand]) -> MemoryAttrs {
    let mut exclusive = false;
    let mut acquire = false;
    let mut release = false;


    if instr_name.contains("xr") {
        exclusive = true;
    }

    let suffix = &instr_name[instr_name.len() - 1..instr_name.len()];
    let size = parse_memory_size_from_suffix(suffix, if op == MemoryOp::Store && exclusive { &operands[1..] } else { operands });


    if instr_name.contains('a') {
        acquire = true;
    }

    if instr_name.contains('l') {
        release = true;
    }

    MemoryAttrs {
        size,
        exclusive,
        acquire,
        release,
    }
}

fn parse_memory_instruction(
    instr_name: &str,
    operands: Vec<Operand>,
) -> IResult<&str, ArmInstruction> {
    if operands.len() < 2 {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Eof,
        )));
    }

    let base_op = instr_name.to_lowercase();
    let op = if base_op.starts_with("ld") || base_op.starts_with("l") {
        MemoryOp::Load
    } else if base_op.starts_with("swp") || base_op.starts_with("cas") {
        MemoryOp::Rmw
    } else if base_op.starts_with("st") || base_op.starts_with("s") {
        MemoryOp::Store
    }  else {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Tag,
        )));
    };

    let attrs = parse_memory_attrs(op, &base_op, &operands);

    // Exclusive instructions (STXR, STLXR, etc.)
    if (base_op.contains("stlxr") || base_op.contains("stxr")) && operands.len() >= 3 {
        return Ok((
            "",
            ArmInstruction::MemoryExclusive(
                op,
                attrs,
                operands[0].clone(),
                operands[1].clone(),
                operands[2].clone(),
            ),
        ));
    }

    // LSE atomic memory operations (LD*/ST*)
    if base_op.starts_with("ldadd")
        || base_op.starts_with("ldeor")
        || base_op.starts_with("ldclr")
        || base_op.starts_with("ldset")
        || base_op.starts_with("ldumax")
        || base_op.starts_with("stadd")
        || base_op.starts_with("steor")
        || base_op.starts_with("stclr")
        || base_op.starts_with("stset")
        || base_op.starts_with("stumax")
        || base_op.starts_with("swp")
        || base_op.starts_with("cas")
    {
        let lse_op = if base_op.contains("add") {
            LSEop::Add
        } else if base_op.contains("eor") {
            LSEop::Eor
        } else if base_op.contains("clr") {
            LSEop::Clr
        } else if base_op.contains("set") {
            LSEop::Set
        } else if base_op.contains("max") || base_op.contains("umax") {
            LSEop::Max
        } else if base_op.contains("swp") {
            LSEop::Swp
        } else if base_op.contains("cas") {
            LSEop::Cas
        } else {
            unreachable!()
        };

        if base_op.starts_with("ld") || base_op.starts_with("swp") || base_op.starts_with("cas") {
            if operands.len() >= 3 {
                return Ok((
                    "",
                    ArmInstruction::MemoryLSE(
                        op,
                        attrs,
                        lse_op,
                        operands[0].clone(),
                        operands[1].clone(),
                        operands[2].clone(),
                    ),
                ));
            } else {
                return Err(nom::Err::Error(nom::error::Error::new(
                    "",
                    nom::error::ErrorKind::Eof,
                )));
            }
        } else if base_op.starts_with("st") {
            if operands.len() >= 2 {
                return Ok((
                    "",
                    ArmInstruction::MemoryLSE(
                        op,
                        attrs,
                        lse_op,
                        operands[0].clone(), // dummy    
                        operands[0].clone(), 
                        operands[1].clone(),
                    ),
                ));
            } else {
                return Err(nom::Err::Error(nom::error::Error::new(
                    "",
                    nom::error::ErrorKind::Eof,
                )));
            }
        } else if base_op.starts_with("swp") {
            
        }
    }

    // Default: plain memory op
    Ok((
        "",
        ArmInstruction::Memory(op, attrs, operands[0].clone(), operands[1].clone()),
    ))
}

fn parse_pair_memory_instruction(
    instr_name: &str,
    operands: Vec<Operand>,
) -> IResult<&str, ArmInstruction> {
    if instr_name != "stp" && instr_name != "ldp" {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Tag,
        )));
    }

    if operands.len() != 3 {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Count,
        )));
    }

    if let (Operand::Register(reg1), Operand::Register(reg2), Operand::Memory(addr)) = (
        operands[0].clone(),
        operands[1].clone(),
        operands[2].clone(),
    ) {
        if reg1.reg_type != reg2.reg_type {
            return Err(nom::Err::Error(nom::error::Error::new(
                "",
                nom::error::ErrorKind::Verify,
            )));
        }

        let op = if instr_name == "ldp" {
            MemoryOp::Load
        } else {
            MemoryOp::Store
        };

        Ok(("", ArmInstruction::MemoryPair(op, reg1, reg2, addr)))
    } else {
        Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Verify,
        )))
    }
}

fn parse_branch_instruction(
    instr_name: &str,
    operands: Vec<Operand>,
) -> IResult<&str, ArmInstruction> {
    let instr_name = instr_name.to_lowercase();

    match instr_name.as_str() {
        "bl" => {
            if operands.len() != 1 {
                return Err(nom::Err::Error(nom::error::Error::new(
                    "",
                    nom::error::ErrorKind::Count,
                )));
            }
            return Ok(("", ArmInstruction::BranchLink(operands[0].clone())));
        }

        "br" => {
            if operands.len() != 1 {
                return Err(nom::Err::Error(nom::error::Error::new(
                    "",
                    nom::error::ErrorKind::Count,
                )));
            }
            return Ok(("", ArmInstruction::BranchRegister(operands[0].clone())));
        }

        "blr" => {
            if operands.len() != 1 {
                return Err(nom::Err::Error(nom::error::Error::new(
                    "",
                    nom::error::ErrorKind::Count,
                )));
            }
            return Ok(("", ArmInstruction::BranchLinkRegister(operands[0].clone())));
        }

        "ret" => {
            let op = if operands.is_empty() {
                None
            } else {
                Some(operands[0].clone())
            };
            return Ok(("", ArmInstruction::Return(op)));
        }

        "cbz" => {
            if operands.len() != 2 {
                return Err(nom::Err::Error(nom::error::Error::new(
                    "",
                    nom::error::ErrorKind::Count,
                )));
            }
            return Ok((
                "",
                ArmInstruction::Branch(
                    Some(Condition::Zero(operands[0].clone())),
                    operands[1].clone(),
                ),
            ));
        }

        "cbnz" => {
            if operands.len() != 2 {
                return Err(nom::Err::Error(nom::error::Error::new(
                    "",
                    nom::error::ErrorKind::Count,
                )));
            }
            return Ok((
                "",
                ArmInstruction::Branch(
                    Some(Condition::NotZero(operands[0].clone())),
                    operands[1].clone(),
                ),
            ));
        }

        "tbz" => {
            if operands.len() != 3 {
                return Err(nom::Err::Error(nom::error::Error::new(
                    "",
                    nom::error::ErrorKind::Count,
                )));
            }
            return Ok((
                "",
                ArmInstruction::TestBitBranch(
                    false,
                    operands[0].clone(),
                    operands[1].clone(),
                    operands[2].clone(),
                ),
            ));
        }

        "tbnz" => {
            if operands.len() != 3 {
                return Err(nom::Err::Error(nom::error::Error::new(
                    "",
                    nom::error::ErrorKind::Count,
                )));
            }
            return Ok((
                "",
                ArmInstruction::TestBitBranch(
                    true,
                    operands[0].clone(),
                    operands[1].clone(),
                    operands[2].clone(),
                ),
            ));
        }

        _ => {}
    }

    if operands.len() != 1 {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Count,
        )));
    }

    if instr_name == "b" {
        return Ok(("", ArmInstruction::Branch(None, operands[0].clone())));
    }

    if instr_name.starts_with('b') && instr_name.len() > 1 {
        let cond_str = &instr_name[1..];
        let condition = parse_condition_code(cond_str);

        if let Some(cond) = condition {
            return Ok((
                "",
                ArmInstruction::Branch(Some(Condition::Code(cond)), operands[0].clone()),
            ));
        }
    }

    Err(nom::Err::Error(nom::error::Error::new(
        "",
        nom::error::ErrorKind::Tag,
    )))
}

fn parse_comparison_instruction(
    instr_name: &str,
    operands: Vec<Operand>,
) -> IResult<&str, ArmInstruction> {
    if instr_name.to_lowercase() != "cmp" {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Tag,
        )));
    }

    // @NOTE: we ignore sign extension here and always assume uxtb
    Ok((
        "",
        ArmInstruction::Cmp(operands[0].clone(), operands[1].clone()),
    ))
}

fn parse_conditional_select(
    instr_name: &str,
    operands: Vec<Operand>,
) -> IResult<&str, ArmInstruction> {
    if instr_name.to_lowercase() != "csel" || operands.len() < 4 {
        return Err(nom::Err::Error(nom::error::Error::new(
            "",
            nom::error::ErrorKind::Tag,
        )));
    }

    let condition = if let Operand::Label(condition_str) = &operands[3] {
        match condition_str.to_lowercase().as_str() {
            "eq" => ConditionCode::EQ,
            "ne" => ConditionCode::NE,
            "cs" => ConditionCode::CS,
            "cc" => ConditionCode::CC,
            "mi" => ConditionCode::MI,
            "pl" => ConditionCode::PL,
            "vs" => ConditionCode::VS,
            "vc" => ConditionCode::VC,
            "hi" => ConditionCode::HI,
            "ls" => ConditionCode::LS,
            "ge" => ConditionCode::GE,
            "lt" => ConditionCode::LT,
            "gt" => ConditionCode::GT,
            "le" => ConditionCode::LE,
            "al" => ConditionCode::AL,
            "nv" => ConditionCode::NV,
            _ => ConditionCode::AL,
        }
    } else {
        ConditionCode::AL
    };

    Ok((
        "",
        ArmInstruction::Csel(
            operands[0].clone(),
            operands[1].clone(),
            operands[2].clone(),
            condition,
        ),
    ))
}

fn parse_instruction(input: &str) -> IResult<&str, ArmInstruction> {
    let (input, instr_name) = take_while1(|c: char| c.is_alphabetic() || c == '.')(input)?;
    let (input, operands) = parse_operands(input)?;
    let (input, _) = take_till(|c| c == '\n')(input)?;
    let (input, _) = opt(char('\n')).parse(input)?;

    let result = alt((
        |_: &str| parse_arithmetic_instruction(instr_name, operands.clone()),
        |_: &str| parse_shift_instruction(instr_name, operands.clone()),
        |_: &str| parse_move_instruction(instr_name, operands.clone()),
        |_: &str| parse_pair_memory_instruction(instr_name, operands.clone()),
        |_: &str| parse_memory_instruction(instr_name, operands.clone()),
        |_: &str| parse_branch_instruction(instr_name, operands.clone()),
        |_: &str| parse_comparison_instruction(instr_name, operands.clone()),
        |_: &str| parse_conditional_select(instr_name, operands.clone()),
        |_: &str| parse_fence_instruction(instr_name, operands.clone()),
    ))
    .parse(input);

    result.map(|(_, instr)| (input, instr))
}

fn parse_line(input: &str) -> IResult<&str, ArmInstruction> {
    if input.is_empty() {
        return Err(nom::Err::Error(nom::error::Error::new(
            input,
            nom::error::ErrorKind::Eof,
        )));
    }
    let (input, line) = take_till(|c| c == '\n')(input)?;
    let (input, _) = opt(char('\n')).parse(input)?;
    let line_trimmed = line.trim();

    if line_trimmed.is_empty() {
        return Ok((input, ArmInstruction::Unhandled(String::new())));
    }

    let result = alt((
        parse_label_def,
        map(parse_directive, |dir| ArmInstruction::Directive(dir)),
        parse_instruction,
    ))
    .parse(line_trimmed);

    match result {
        Ok((remaining, instr)) if remaining.is_empty() => Ok((input, instr)),
        _ => Ok((input, ArmInstruction::Unhandled(line.to_string()))),
    }
}

pub fn parse_arm_assembly(input: &str) -> IResult<&str, Vec<ArmInstruction>> {
    let (input, _) = multispace0(input)?;
    let (input, instructions) = many0(parse_line).parse(input)?;
    Ok((input, instructions))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_register() {
        let (_, reg) = parse_register("x0").unwrap();
        assert_eq!(
            reg,
            Register {
                reg_type: RegisterType::X,
                number: 0
            }
        );

        let (_, reg) = parse_register("w12").unwrap();
        assert_eq!(
            reg,
            Register {
                reg_type: RegisterType::W,
                number: 12
            }
        );

        let (_, reg) = parse_register("v31").unwrap();
        assert_eq!(
            reg,
            Register {
                reg_type: RegisterType::V,
                number: 31
            }
        );
    }

    #[test]
    fn test_parse_immediate() {
        let (_, imm) = parse_immediate("42").unwrap();
        assert_eq!(imm, 42);

        let (_, imm) = parse_immediate("#-100").unwrap();
        assert_eq!(imm, -100);

        let (_, imm) = parse_immediate("#0x1F").unwrap();
        assert_eq!(imm, 31);
    }

    #[test]
    fn test_parse_addressing_mode() {
        let (_, addr) = parse_addressing_mode("[x0]").unwrap();
        assert_eq!(
            addr,
            AddressingMode::BaseRegister(Register {
                reg_type: RegisterType::X,
                number: 0
            })
        );

        let (_, addr) = parse_addressing_mode("[x1, #16]").unwrap();
        assert_eq!(
            addr,
            AddressingMode::BaseRegisterWithOffset(
                Register {
                    reg_type: RegisterType::X,
                    number: 1
                },
                16
            )
        );

        let (_, addr) = parse_addressing_mode("[x2, #16]!").unwrap();
        assert_eq!(
            addr,
            AddressingMode::PreIndexed(
                Register {
                    reg_type: RegisterType::X,
                    number: 2
                },
                16
            )
        );

        let (_, addr) = parse_addressing_mode("[sp, #16]!").unwrap();
        assert_eq!(
            addr,
            AddressingMode::PreIndexed(
                Register {
                    reg_type: RegisterType::SP,
                    number: 0
                },
                16
            )
        );
    }

    #[test]
    fn test_parse_arithmetic_instruction() {
        let operands = vec![
            Operand::Register(Register {
                reg_type: RegisterType::X,
                number: 0,
            }),
            Operand::Register(Register {
                reg_type: RegisterType::X,
                number: 1,
            }),
            Operand::Register(Register {
                reg_type: RegisterType::X,
                number: 2,
            }),
        ];
        let (_, instr) = parse_arithmetic_instruction("add", operands).unwrap();
        assert_eq!(
            instr,
            ArmInstruction::Arithmetic(
                ArithmeticOp::Add,
                Operand::Register(Register {
                    reg_type: RegisterType::X,
                    number: 0
                }),
                Operand::Register(Register {
                    reg_type: RegisterType::X,
                    number: 1
                }),
                Some(Operand::Register(Register {
                    reg_type: RegisterType::X,
                    number: 2
                }))
            )
        );

        let operands = vec![
            Operand::Register(Register {
                reg_type: RegisterType::W,
                number: 0,
            }),
            Operand::Register(Register {
                reg_type: RegisterType::W,
                number: 1,
            }),
            Operand::ImmediateValue(42),
        ];
        let (_, instr) = parse_arithmetic_instruction("sub", operands).unwrap();
        assert_eq!(
            instr,
            ArmInstruction::Arithmetic(
                ArithmeticOp::Sub,
                Operand::Register(Register {
                    reg_type: RegisterType::W,
                    number: 0
                }),
                Operand::Register(Register {
                    reg_type: RegisterType::W,
                    number: 1
                }),
                Some(Operand::ImmediateValue(42))
            )
        );
    }

    #[test]
    fn test_parse_memory_instruction() {
        let operands = vec![
            Operand::Register(Register {
                reg_type: RegisterType::X,
                number: 0,
            }),
            Operand::Memory(AddressingMode::BaseRegisterWithOffset(
                Register {
                    reg_type: RegisterType::X,
                    number: 1,
                },
                16,
            )),
        ];
        let (_, instr) = parse_memory_instruction("ldr", operands).unwrap();
        match instr {
            ArmInstruction::Memory(op, attrs, dst, src) => {
                assert_eq!(op, MemoryOp::Load);
                assert_eq!(attrs.size, MemorySize::Double);
                assert_eq!(
                    dst,
                    Operand::Register(Register {
                        reg_type: RegisterType::X,
                        number: 0
                    })
                );
                if let Operand::Memory(AddressingMode::BaseRegisterWithOffset(reg, offset)) = src {
                    assert_eq!(reg.number, 1);
                    assert_eq!(offset, 16);
                } else {
                    panic!("Expected memory operand with base register and offset");
                }
            }
            _ => panic!("Expected memory instruction"),
        }
    }

    #[test]
    fn test_parse_branch_instruction() {
        let operands = vec![Operand::Label(String::from("label1"))];
        let (_, instr) = parse_branch_instruction("b", operands).unwrap();
        assert_eq!(
            instr,
            ArmInstruction::Branch(None, Operand::Label(String::from("label1")))
        );

        let operands = vec![Operand::Label(String::from("label2"))];
        let (_, instr) = parse_branch_instruction("beq", operands).unwrap();
        assert_eq!(
            instr,
            ArmInstruction::Branch(
                Some(Condition::Code(ConditionCode::EQ)),
                Operand::Label(String::from("label2"))
            )
        );
    }

    #[test]
    fn test_parse_directive() {
        let (_, dir) = parse_directive(".align 4").unwrap();
        assert_eq!(dir, Directive::Align(4));

        let (_, dir) = parse_directive(".size function, 128").unwrap();
        assert_eq!(
            dir,
            Directive::Size(String::from("function"), String::from("128"))
        );
    }

    #[test]
    fn test_parse_full_instruction() {
        let (_, instr) = parse_instruction("add x0, x1, x2").unwrap();
        match instr {
            ArmInstruction::Arithmetic(op, dst, src1, Some(src2)) => {
                assert_eq!(op, ArithmeticOp::Add);
                assert_eq!(
                    dst,
                    Operand::Register(Register {
                        reg_type: RegisterType::X,
                        number: 0
                    })
                );
                assert_eq!(
                    src1,
                    Operand::Register(Register {
                        reg_type: RegisterType::X,
                        number: 1
                    })
                );
                assert_eq!(
                    src2,
                    Operand::Register(Register {
                        reg_type: RegisterType::X,
                        number: 2
                    })
                );
            }
            _ => panic!("Expected arithmetic instruction"),
        }

        let (_, instr) = parse_instruction("mov x0, #42").unwrap();
        match instr {
            ArmInstruction::Move(op, dst, src) => {
                assert_eq!(op, MoveOp::Mov);
                assert_eq!(
                    dst,
                    Operand::Register(Register {
                        reg_type: RegisterType::X,
                        number: 0
                    })
                );
                assert_eq!(src, Operand::ImmediateValue(42));
            }
            _ => panic!("Expected move instruction"),
        }
    }

    #[test]
    fn test_parse_arm_assembly() {
        let input = r#"
        .align 4
        .type function, %function
        function:
            stp x29, x30, [sp, #-16]!
            mov x29, sp
            add x0, x0, #1
            bl helper_function
            ldp x29, x30, [sp], #16
            ret
        helper_function:
            stp x29, x30, [sp, #-16]!
            mov x0, #42
        .block_function:
            ldp x29, x30, [sp], #16
            ret"#;

        let (_unparsed, instructions) = parse_arm_assembly(input).unwrap();
        assert_eq!(instructions.len(), 15);

        match &instructions[0] {
            ArmInstruction::Directive(Directive::Align(4)) => {}
            _ => panic!("Expected .align 4 directive"),
        }

        match &instructions[2] {
            ArmInstruction::Label(label) => assert_eq!(label, "function"),
            _ => panic!("Expected function label"),
        }

        match &instructions[6] {
            ArmInstruction::BranchLink(Operand::Label(label)) => {
                assert_eq!(label, "helper_function")
            }
            _ => panic!("Expected branch and link instruction"),
        }
    }

    #[test]
    fn test_parse_beginning() {
        let input: &str = r#"
        .arch armv8-a
        .text
        .align 2
        .type test_function, %function
        test_function:
        .LFB0:
            .cfi_startproc
            mov x0, 0
            ret
            .cfi_endproc
        .LFE0:
            .size test_function, .-test_function"#;

        let (_unparsed, instructions) = parse_arm_assembly(input).unwrap();

        assert_eq!(instructions.len(), 12);
    }
}
