/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
mod parser;
mod transform;

use std::fmt::Display;

pub use parser::parse_arm_assembly;
pub use transform::{extract_arm_functions, remove_directives, transform_labels};

use crate::{
    atomic_types, AtomicType, BoogieFunction, BoogieInstruction, SideEffect, ToBoogie, Width, DUMMY_REG
};

#[derive(Debug, Clone, PartialEq)]
pub enum RegisterType {
    X, // 64-bit general purpose register
    W, // 32-bit general purpose register
    V, // SIMD/FP register
    Q, // 128-bit SIMD register
    D, // 64-bit SIMD register
    S, // 32-bit SIMD register
    H, // 16-bit SIMD register
    B, // 8-bit SIMD register
    SP,
}

#[derive(Debug, Clone, PartialEq)]
pub struct Register {
    pub reg_type: RegisterType,
    pub number: u8,
}

#[derive(Debug, Clone, PartialEq)]
pub enum AddressingMode {
    // [Xn]
    BaseRegister(Register),
    // [Xn, #imm]
    BaseRegisterWithOffset(Register, i64),
    // [Xn, #imm]!
    PreIndexed(Register, i64),
    // [Xn], #imm
    PostIndexed(Register, i64),
}

#[derive(Debug, Clone, PartialEq)]
pub enum Operand {
    Register(Register),
    ImmediateValue(i64),
    Memory(AddressingMode),
    Label(String),
    ShiftedRegister(Register, String, i64),
    RegisterList(Vec<Register>),
    FenceMode(FenceType),
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum ConditionCode {
    EQ, // Equal
    NE, // Not equal

    HS, // Higher or same
    CS, // Carry set

    LO, // Lower
    CC, // Carry clear

    MI, // Minus, negative
    PL, // Plus, positive or zero
    VS, // Overflow
    VC, // No overflow
    HI, // Unsigned higher
    LS, // Unsigned lower or same
    GE, // Signed greater than or equal
    LT, // Signed less than
    GT, // Signed greater than
    LE, // Signed less than or equal
    AL, // Always (default)
    NV, // Never
}

#[derive(Debug, Clone, PartialEq)]
pub enum Condition {
    Code(ConditionCode),
    Zero(Operand),
    NotZero(Operand),
}

#[derive(Debug, Clone, PartialEq)]
pub enum Directive {
    CFIStartProc,
    CFIEndProc,
    Size(String, String),
    Align(u32),
    P2Align(u32, u32, u32),
    Type(String, String),
}

#[derive(Debug, Clone, PartialEq, Copy)]
pub enum ArithmeticOp {
    Add,
    Sub,
    Mul,
    And,
    Orr,
    Eor,
}

#[derive(Debug, Clone, PartialEq, Copy)]
pub enum ShiftOp {
    Lsl,
    Lsr,
    Asr,
}

#[derive(Debug, Clone, PartialEq, Copy)]
pub enum MemoryOp {
    Load,
    Store,
    Rmw
}

#[derive(Debug, Clone, PartialEq, Copy)]
pub enum LSEop {
    Max,
    Clr,
    Set,
    Eor,
    Add,
    Swp,
    Cas
}

#[derive(Debug, Clone, PartialEq, Copy)]
pub enum MemorySize {
    Byte,   // 8-bit
    Half,   // 16-bit
    Word,   // 32-bit
    Double, // 64-bit
}

impl MemorySize {
    pub fn bytes(&self) -> u32 {
        match self {
            MemorySize::Byte => 1,
            MemorySize::Half => 2,
            MemorySize::Word => 4,
            MemorySize::Double => 8,
        }
    }
    pub fn mask(&self) -> u64 {
        (2u64.overflowing_pow(self.bytes() * 8)).0.overflowing_add_signed(-1).0
    }
}

#[derive(Debug, Clone, PartialEq)]
pub struct MemoryAttrs {
    pub size: MemorySize,
    pub exclusive: bool,
    pub acquire: bool,
    pub release: bool,
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum MoveOp {
    Mov,
    Mvn,
    Neg
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum FenceType {
    SY,
    LD,
}

impl Display for FenceType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "{}",
            match self {
                Self::SY => "SY()",
                Self::LD => "LD()",
            }
        )
    }
}

#[derive(Debug, Clone, PartialEq)]
pub enum ArmInstruction {
    Arithmetic(ArithmeticOp, Operand, Operand, Option<Operand>),

    Shift(ShiftOp, Operand, Operand),

    Move(MoveOp, Operand, Operand),

    Memory(MemoryOp, MemoryAttrs, Operand, Operand),
    MemoryLSE(MemoryOp, MemoryAttrs, LSEop, Operand, Operand, Operand),
    MemoryPair(MemoryOp, Register, Register, AddressingMode),
    MemoryExclusive(MemoryOp, MemoryAttrs, Operand, Operand, Operand),
    Cmp(Operand, Operand),
    Csel(Operand, Operand, Operand, ConditionCode),

    Dmb(FenceType),

    Branch(Option<Condition>, Operand),
    BranchLink(Operand),
    BranchLinkRegister(Operand),
    BranchRegister(Operand),
    Return(Option<Operand>),

    TestBitBranch(bool, Operand, Operand, Operand),

    Label(String),

    Directive(Directive),
    Unhandled(String),
}

#[derive(Debug, Clone)]
pub struct ArmFunction {
    pub name: String,
    pub instructions: Vec<ArmInstruction>,
}

pub const ARMV8_WIDTH: Width = Width::Wide;

impl ToBoogie for ArmFunction {
    fn to_boogie(self) -> BoogieFunction {
        let instructions = self
            .instructions
            .iter()
            .map(|instr| arm_instruction_to_boogie(instr))
            .collect();

        let atomic_type = atomic_types(&self.name);

        let register_ident = match atomic_type.type_width(ARMV8_WIDTH) {
            8 => "x",
            _ => "w",
        };

        let ptr_ident = match AtomicType::VPTR.type_width(ARMV8_WIDTH) {
            8 => "x",
            _ => "w",
        };

        BoogieFunction {
            name: self.name.clone(),
            instructions,
            address: format!("{}0", ptr_ident),
            input1: format!("{}1", register_ident),
            input2: format!("{}2", register_ident),
            output: format!("{}0", register_ident),
        }
    }
}

pub fn condition_code_to_boogie(code: ConditionCode) -> String {
    match code {
        ConditionCode::CS => condition_code_to_boogie(ConditionCode::HS),
        ConditionCode::CC => condition_code_to_boogie(ConditionCode::LO),
        _ => format!("{:?}()", code),
    }
}

pub fn condition_to_boogie(cond: &Condition) -> String {
    match cond {
        Condition::Code(code) => format!("branch({},flags)", condition_code_to_boogie(*code)),
        Condition::Zero(reg) => format!("cbz({})", operand_to_boogie(&reg)),
        Condition::NotZero(reg) => format!("cbnz({})", operand_to_boogie(&reg)),
    }
}

pub fn arm_instruction_to_boogie(instr: &ArmInstruction) -> BoogieInstruction {
    match instr {
        ArmInstruction::Label(name) => BoogieInstruction::Label(name.clone()),
        ArmInstruction::Branch(cond_opt, target) => match target {
            Operand::Label(label_name) => {
                BoogieInstruction::Branch(vec![label_name.to_string()], if let Some(cond) = cond_opt {
                    condition_to_boogie(cond)
                } else {
                    "true".to_string()
                })
            }
            _ => BoogieInstruction::Unhandled(format!(
                "// Unhandled Branch Type: {:?}, {:?}",
                cond_opt, target
            )),
        },
        ArmInstruction::Dmb(mode) => BoogieInstruction::Instr(
            "dmb".to_string(), SideEffect::Global,
            DUMMY_REG.to_string(),
            vec![mode.to_string()],
        ),
        ArmInstruction::Return(_) => BoogieInstruction::Return,
        ArmInstruction::Arithmetic(op, dest, src1, src2_opt) => {
            let op_name = match op {
                ArithmeticOp::Add => "add",
                ArithmeticOp::Sub => "sub",
                ArithmeticOp::Mul => "mul",
                ArithmeticOp::And => "andd",
                ArithmeticOp::Orr => "orr",
                ArithmeticOp::Eor => "eor",
            };
            let dest_reg = operand_to_boogie(dest);
            let src1 = operand_to_boogie(src1);

            let ops = if let Some(src2) = src2_opt {
                let src2 = operand_to_boogie(src2);
                vec![src1, src2]
            } else {
                vec![src1]
            };
            BoogieInstruction::Instr(op_name.to_string(), SideEffect::Local, dest_reg, ops)
        }
        ArmInstruction::Move(op, dest, src) => {
            let op_name = match op {
                MoveOp::Mov => "mov",
                MoveOp::Mvn => "mvn",
                MoveOp::Neg => "neg"
            };

            let dest_reg = operand_to_boogie(dest);
            let src_reg = operand_to_boogie(src);
            BoogieInstruction::Instr(op_name.to_string(), SideEffect::Local, dest_reg, vec![src_reg])
        }
        ArmInstruction::Memory(op, attrs, reg1, reg2) => {
            let (op_name, has_output) = match op {
                MemoryOp::Load => {
                    if attrs.exclusive {
                        ("ldx", true)
                    } else {
                        ("ld", true)
                    }
                }
                MemoryOp::Store => ("st", false),
                MemoryOp::Rmw => unimplemented!()
            };

            let dest_or_src_reg = operand_to_boogie(reg1);
            let addr_reg = operand_to_boogie(reg2);

            if has_output {
                BoogieInstruction::Instr(
                    op_name.to_string(), SideEffect::Global,
                    dest_or_src_reg,
                    vec![attrs.acquire.to_string(), addr_reg, format!("{}bv64", attrs.size.mask())],
                )
            } else {
                BoogieInstruction::Instr(
                    op_name.to_string(), SideEffect::Global,
                    DUMMY_REG.to_string(),
                    vec![attrs.release.to_string(), dest_or_src_reg, format!("{}bv64", attrs.size.mask()), addr_reg],
                )
            }
        }
        ArmInstruction::MemoryExclusive(op, attrs, dest, src, addr) => {
            let op_name = match op {
                MemoryOp::Store => "stx",
                _ => unimplemented!(),
            };

            let dest_reg = operand_to_boogie(dest);
            let addr_reg = operand_to_boogie(addr);
            let src_reg = operand_to_boogie(src);

            BoogieInstruction::Instr(
                op_name.to_string(), SideEffect::Global,
                dest_reg,
                vec![attrs.release.to_string(), src_reg, format!("{}bv64", attrs.size.mask()), addr_reg],
            )
        }
        ArmInstruction::MemoryLSE(op, attrs, lse_op, src, dest, addr) => {
            let lse_name = match lse_op {
                LSEop::Max => "umax",
                LSEop::Clr => "clr",
                LSEop::Set => "set",
                LSEop::Eor => "eor",
                LSEop::Add => "add",
                LSEop::Swp => "swp",
                LSEop::Cas => "cas"
            };
            let addr_reg = operand_to_boogie(addr);
            let src_reg = operand_to_boogie(src);
            let dest_reg = match lse_op {
                LSEop::Cas => src_reg.clone(),
                _ => operand_to_boogie(dest)
            };

            let ret = match op {
                MemoryOp::Load => 
                    BoogieInstruction::Instr(
                        "ld".to_string() + lse_name, SideEffect::Global,
                        dest_reg,
                        vec![attrs.acquire.to_string(), attrs.release.to_string(), src_reg, addr_reg, format!("{}bv64", attrs.size.mask()), format!("{}bv64", attrs.size.mask())]
                    ),
                MemoryOp::Store =>                     
                    BoogieInstruction::Instr(
                        "st".to_string() + lse_name, SideEffect::Global,
                        DUMMY_REG.to_string(),
                        vec![attrs.release.to_string(), src_reg, addr_reg, format!("{}bv64", attrs.size.mask()), format!("{}bv64", attrs.size.mask())]
                    ),
                MemoryOp::Rmw =>
                    BoogieInstruction::Instr(
                        lse_name.to_string(), SideEffect::Global,
                        dest_reg,
                        match lse_op {
                            LSEop::Cas
                                => vec![attrs.acquire.to_string(), attrs.release.to_string(), src_reg, operand_to_boogie(dest), addr_reg, format!("{}bv64", attrs.size.mask()), format!("{}bv64", attrs.size.mask())],
                            LSEop::Swp
                                => vec![attrs.acquire.to_string(), attrs.release.to_string(), src_reg, addr_reg, format!("{}bv64", attrs.size.mask()), format!("{}bv64", attrs.size.mask())],
                            _ => unimplemented!()
                        }
                    ),
            };            
            ret
        }
        ArmInstruction::Cmp(op1, op2) => {
            let op1_reg = operand_to_boogie(op1);
            let op2_reg = operand_to_boogie(op2);

            BoogieInstruction::Instr(
                "cmp".to_string(), SideEffect::Local,
                DUMMY_REG.to_string(),
                vec![op1_reg, op2_reg],
            )
        }
        ArmInstruction::Csel(dest, op1, op2, ce) => {
            let dest_reg = operand_to_boogie(dest);
            let op1_reg = operand_to_boogie(op1);
            let op2_reg = operand_to_boogie(op2);
            let cond = condition_to_boogie(&Condition::Code(*ce));
            BoogieInstruction::Instr("csel".to_string(), SideEffect::Local, dest_reg, vec![op1_reg, op2_reg, cond])
        }
        ArmInstruction::Directive(directive) => {
            BoogieInstruction::Comment(format!("Directive: {:?}", directive))
        }
        _ => {
            log::warn!("Unhandled: {:?}", instr);
            BoogieInstruction::Unhandled(format!("{:?}", instr))
        }
    }
}

fn register_to_boogie(reg: &Register) -> String {
    match reg.reg_type {
        RegisterType::X => format!("x{}", reg.number),
        RegisterType::W => format!("w{}", reg.number),
        RegisterType::V => format!("v{}", reg.number),
        RegisterType::Q => format!("q{}", reg.number),
        RegisterType::D => format!("d{}", reg.number),
        RegisterType::S => format!("s{}", reg.number),
        RegisterType::H => format!("h{}", reg.number),
        RegisterType::B => format!("b{}", reg.number),
        RegisterType::SP => "sp".to_string(),
    }
}

fn operand_to_boogie(operand: &Operand) -> String {
    match operand.clone() {
        Operand::Register(r) => register_to_boogie(&r),
        Operand::ImmediateValue(val) => {
            if val != -1 {
                format!("{}bv64", val)
            } else {
                "bit_inv(0bv64)".to_string()
            }
        },
        Operand::Memory(addr_mode) => match addr_mode {
            AddressingMode::BaseRegister(reg) => register_to_boogie(&reg),
            AddressingMode::BaseRegisterWithOffset(reg, offset) => {
                format!("{}+{}", register_to_boogie(&reg), offset)
            }
            _ => format!("/* unsupported addressing mode {:?} /*", operand),
        },
        _ => unimplemented!(),
    }
}
