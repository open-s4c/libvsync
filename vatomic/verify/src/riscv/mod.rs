/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
mod parser;
mod transform;

// pub use genegrate::{arm_to_boogie_code, get_address_registers, get_used_registers};
pub use parser::parse_riscv_assembly;
pub use transform::{extract_riscv_functions, remove_directives, transform_labels};

use crate::{BoogieFunction, BoogieInstruction, SideEffect, ToBoogie, DUMMY_REG};

#[derive(Debug, Clone, PartialEq)]
pub struct MemoryOperand {
    pub offset: i64,
    pub base: Register,
}

#[derive(Debug, Clone, PartialEq, Copy)]
pub enum Size {
    Byte,   // 8-bit
    Half,   // 16-bit
    Word,   // 32-bit
    Double, // 64-bit
}

impl Size {
    pub fn bytes(&self) -> u32 {
        match self {
            Size::Byte => 1,
            Size::Half => 2,
            Size::Word => 4,
            Size::Double => 8,
        }
    }

    pub fn mask(&self) -> u64 {
        (2u64.overflowing_pow(self.bytes() * 8)).0.overflowing_add_signed(-1).0
    }
}

#[derive(Debug, Clone, PartialEq, Copy)]
pub enum AtomicSemantics {
    None,
    Acquire,
    Release,
    AcquireRelease,
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
pub enum FenceMode {
    Read,
    Write,
    ReadWrite,
    OutputWrite,
    InputOutputReadWrite,
}

#[derive(Debug, Clone, PartialEq)]
pub enum RegisterType {
    A,               // Argument registers (a0-a7)
    T,               // Temporary registers (t0-t6)
    S,               // Saved registers (s0-s11)
    X,               // General-purpose (x0-x31)
    Special(String), // Special registers like "zero", "ra", "sp"
}

#[derive(Debug, Clone, PartialEq)]
pub struct Register {
    pub reg_type: RegisterType,
    pub number: Option<u8>,
}

#[derive(Debug, Clone, PartialEq)]
pub enum Operand {
    Register(Register),
    Immediate(i64),
    Memory(MemoryOperand),
    Label(String),
    FenceMode(FenceMode),
}
#[derive(Debug, Clone, PartialEq, Copy)]
pub enum AtomicOp {
    Swap,
    Add,
    And,
    Or,
    Xor,
    Max,
    Min,
}
#[derive(Debug, Clone, PartialEq, Copy)]
pub enum ArithmeticOp {
    Add,
    Sub,
    Mul,
    And,
    Or,
    Xor,
    Sll, // Shift Left Logical
    Srl, // Shift Right Logical
    Sra, // Shift Right Arithmetic
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum ComparisonOp {
    Eq,   // Equal (for beq)
    Ne,   // Not equal (for bne)
    Lt,   // Less than, signed (for blt)
    Ge,   // Greater than or equal, signed (for bge)
    Ltu,  // Less than, unsigned (for bltu)
    Geu,  // Greater than or equal, unsigned (for bgeu)
    Nez,  // Not equal zero (for bnez)
    Gtu, //
    Leu, //
}

#[derive(Debug, Clone, PartialEq)]
pub enum RiscvInstruction {
    Label(String),
    Directive(Directive),
    Fence {
        pred: FenceMode,
        succ: FenceMode,
    },
    Branch {
        op: ComparisonOp,
        rs1: Register,
        rs2: Register,
        label: String,
    },
    Load {
        size: Size,
        dst: Register,
        src: MemoryOperand,
    },
    UnsignedLoad {
        size: Size,
        dst: Register,
        src: MemoryOperand,
    },
    Store {
        size: Size,
        src: Register,
        dst: MemoryOperand,
    },
    LoadReserved {
        size: Size,
        semantics: AtomicSemantics,
        rd: Register,
        addr: MemoryOperand,
    },
    StoreConditional {
        size: Size,
        semantics: AtomicSemantics,
        rd: Register,
        rs2: Register,
        addr: MemoryOperand,
    },
    LoadImmidate {
        register: Register,
        value: i64,
    },
    Atomic {
        op: AtomicOp,
        size: Size,
        semantics: AtomicSemantics,
        rd: Register,
        rs2: Register,
        addr: MemoryOperand,
    },

    Not {
        rd: Register,
        rs: Register,
    },
    Neg {
        rd: Register,
        rs: Register,
        size: Option<Size>,
    },
    ArithmeticRR {
        op: ArithmeticOp,
        rd: Register,
        rs1: Register,
        rs2: Register,
        size: Option<Size>,
    },
    ArithmeticRI {
        op: ArithmeticOp,
        rd: Register,
        rs1: Register,
        imm: i64,
        size: Option<Size>,
    },
    Move(Register, Register),
    SignExtendWord(Register, Register),
    Jump {
        rd: Register,
        label: Option<String>,
    },
    Return,
    // TODO: add to boogie
    Call {
        label: String,
    },
    // TODO: add to boogie
    LoadAddress {
        register: Register,
        label: String,
    },
    Unhandled(String),
}

#[derive(Debug, Clone)]
pub struct RiscvFunction {
    pub name: String,
    pub instructions: Vec<RiscvInstruction>,
}

impl ToBoogie for RiscvFunction {
    fn to_boogie(self) -> BoogieFunction {
        let instructions = self
            .instructions
            .iter()
            .map(|instr| riscv_instruction_to_boogie(instr))
            .collect();

        BoogieFunction {
            name: self.name.clone(),
            instructions,
            address: "a0".to_string(),
            input1: "a1".to_string(),
            input2: "a2".to_string(),
            output: "a0".to_string(),
        }
    }
}

pub fn riscv_instruction_to_boogie(instr: &RiscvInstruction) -> BoogieInstruction {
    match riscv_instruction_to_boogie_direct(instr) {
        BoogieInstruction::Instr(instr, side_effects, reg,  args) if reg.to_lowercase() == "zero" => 
            BoogieInstruction::Instr(instr, side_effects, DUMMY_REG.to_string(), args),
        instr => instr,
    }
}

fn riscv_instruction_to_boogie_direct(instr: &RiscvInstruction) -> BoogieInstruction {
    match instr {
        RiscvInstruction::Label(name) => BoogieInstruction::Label(name.clone()),
        RiscvInstruction::Fence { pred, succ } => {
            let (pr, pw) = fence_mode_to_boogie(pred);
            let (sr, sw) = fence_mode_to_boogie(succ);

            BoogieInstruction::Instr(
                "fence".to_string(), SideEffect::Global,
                DUMMY_REG.to_string(),
                vec![
                    pr.to_string(),
                    pw.to_string(),
                    sr.to_string(),
                    sw.to_string(),
                ],
            )
        }
        RiscvInstruction::Branch {
            op,
            rs1,
            rs2,
            label,
        } => {
            let r1 = register_to_string(rs1);
            let r2 = register_to_string(rs2);
            BoogieInstruction::Branch(
                vec![label.to_string()],
                match *op {
                    ComparisonOp::Nez => format!("bnez({})", r1.to_string()),
                    _ => format!(
                        "b{}({}, {})",
                        format!("{:?}", op).to_lowercase(),
                        r1.to_string(),
                        r2.to_string()
                    ),
                },
            )
        }
        RiscvInstruction::Load { dst, src, size, .. } => {
            let dst_reg = register_to_string(dst);
            let src_reg = operand_to_boogie(&Operand::Memory(src.clone()));

            BoogieInstruction::Instr("ld".to_string(), SideEffect::Global, dst_reg, vec![src_reg, format!("{}bv64", size.mask())])
        }
        RiscvInstruction::UnsignedLoad { dst, src, size, .. } => {
            let dst_reg = register_to_string(dst);
            let src_reg = operand_to_boogie(&Operand::Memory(src.clone()));

            BoogieInstruction::Instr("ldu".to_string(), SideEffect::Global, dst_reg, vec![src_reg, format!("{}bv64", size.mask())])
        }
        RiscvInstruction::Store { size, dst, src, .. } => {
            let src_reg = operand_to_boogie(&Operand::Register(src.clone()));
            let dst_reg = operand_to_boogie(&Operand::Memory(dst.clone()));

            BoogieInstruction::Instr(
                "sb".to_string(), SideEffect::Global,
                DUMMY_REG.to_string(),
                vec![src_reg, dst_reg,format!("{}bv64", size.mask()),],
            )
        }
        RiscvInstruction::LoadReserved {
            semantics,
            rd,
            addr,
            size,
            ..
        } => {
            let aq = matches!(
                semantics,
                AtomicSemantics::Acquire | AtomicSemantics::AcquireRelease
            );
            let rl = matches!(
                semantics,
                AtomicSemantics::Release | AtomicSemantics::AcquireRelease
            );
            let dst_reg = register_to_string(rd);
            let src_reg = operand_to_boogie(&Operand::Memory(addr.clone()));

            BoogieInstruction::Instr(
                "lr".to_string(), SideEffect::Global,
                dst_reg,
                vec![aq.to_string(), rl.to_string(), src_reg, format!("{}bv64", size.mask())],
            )
        }
        RiscvInstruction::Call { label } => BoogieInstruction::Instr(
            "call".to_string(),
            SideEffect::Local,
            DUMMY_REG.to_string(),
            vec![label.to_string()],
        ),
        RiscvInstruction::LoadAddress { register, label } => {
            let dst_reg = register_to_string(register);
            BoogieInstruction::Instr("la".to_string(), SideEffect::Global, dst_reg, vec![label.to_string()])
        }
        RiscvInstruction::StoreConditional {
            size,
            semantics,
            rd,
            rs2,
            addr,
            ..
        } => {
            let aq = matches!(
                semantics,
                AtomicSemantics::Acquire | AtomicSemantics::AcquireRelease
            );
            let rl = matches!(
                semantics,
                AtomicSemantics::Release | AtomicSemantics::AcquireRelease
            );

            let dst_reg = register_to_string(rd);
            let src_reg = operand_to_boogie(&Operand::Register(rs2.clone()));
            let addr_op = operand_to_boogie(&Operand::Memory(addr.clone()));

            BoogieInstruction::Instr(
                "sc".to_string(), SideEffect::Global,
                dst_reg,
                vec![aq.to_string(), rl.to_string(), src_reg, addr_op, format!("{}bv64", size.mask())],
            )
        }
        RiscvInstruction::LoadImmidate { register, value } => {
            let dst = register_to_string(register);
            let value = operand_to_boogie(&Operand::Immediate(*value));

            BoogieInstruction::Instr("li".to_string(), SideEffect::Local, dst, vec![value])
        }
        RiscvInstruction::Move(dst, src) => {
            let dst_reg = register_to_string(dst);
            let src_reg = operand_to_boogie(&Operand::Register(src.clone()));
            BoogieInstruction::Instr("mv".to_string(), SideEffect::Local, dst_reg, vec![src_reg])
        }
        RiscvInstruction::Atomic {
            op,
            semantics,
            rd,
            rs2,
            addr,
            size,
            ..
        } => {
            let atomic_op = format!(
                "Atomic{}()",
                match op {
                    AtomicOp::Add => "Add",
                    AtomicOp::And => "And",
                    AtomicOp::Max => "Max",
                    AtomicOp::Min => "Min",
                    AtomicOp::Or => "Or",
                    AtomicOp::Xor => "Xor",
                    AtomicOp::Swap => "Swap",
                }
            );

            let aq = matches!(
                semantics,
                AtomicSemantics::Acquire | AtomicSemantics::AcquireRelease
            );
            let rl = matches!(
                semantics,
                AtomicSemantics::Release | AtomicSemantics::AcquireRelease
            );

            let dst_reg = register_to_string(rd);
            let src_reg = operand_to_boogie(&Operand::Register(rs2.clone()));
            let addr_op = operand_to_boogie(&Operand::Memory(addr.clone()));

            BoogieInstruction::Instr(
                "atomic".to_string(), SideEffect::Global,
                dst_reg,
                vec![atomic_op, aq.to_string(), rl.to_string(), src_reg, addr_op, format!("{}bv64", size.mask())],
            )
        }
        RiscvInstruction::ArithmeticRR {
            op,
            rd,
            rs1,
            rs2,
            size,
        } => {
            let dst_reg = register_to_string(rd);
            let src1_reg = register_to_string(rs1);
            let src2_reg = register_to_string(rs2);

            let op_name = match op {
                ArithmeticOp::Add => "add",
                ArithmeticOp::Sub => "sub",
                ArithmeticOp::Mul => "mul",
                ArithmeticOp::And => "and",
                ArithmeticOp::Or => "or",
                ArithmeticOp::Xor => "xor",
                ArithmeticOp::Sll => "sll",
                ArithmeticOp::Srl => "srl",
                ArithmeticOp::Sra => "sra",
            };

            let _op_suffix = if let Some(Size::Word) = size { "w" } else { "" };

            BoogieInstruction::Instr(format!("{}", op_name), SideEffect::Local, dst_reg, vec![src1_reg, src2_reg])
        }
        RiscvInstruction::ArithmeticRI {
            op,
            rd,
            rs1,
            imm,
            size,
        } => {
            let dst_reg = register_to_string(rd);
            let src_reg = register_to_string(rs1);
            let imm_str = format!("{}bv64", *imm as u64);

            let op_name = match op {
                ArithmeticOp::Add => "addi",
                ArithmeticOp::Sub => "subi",
                ArithmeticOp::And => "andi",
                ArithmeticOp::Or => "ori",
                ArithmeticOp::Xor => "xori",
                ArithmeticOp::Sll => "slli",
                ArithmeticOp::Srl => "srli",
                ArithmeticOp::Sra => "srai",
                _ => unreachable!(),
            };

            let _op_suffix = if let Some(Size::Word) = size { "w" } else { "" };

            BoogieInstruction::Instr(format!("{}", op_name), SideEffect::Local, dst_reg, vec![src_reg, imm_str])
        }
        RiscvInstruction::Not { rd, rs } => {
            let dst_reg = register_to_string(rd);
            let src_reg = register_to_string(rs);
            BoogieInstruction::Instr("not".to_string(), SideEffect::Local, dst_reg, vec![src_reg])
        }
        RiscvInstruction::Neg { rd, rs, size } => {
            let dst_reg = register_to_string(rd);
            let src_reg = register_to_string(rs);
            let op_name = if let Some(Size::Word) = size {
                "negw"
            } else {
                "neg"
            };
            BoogieInstruction::Instr(op_name.to_string(), SideEffect::Local, dst_reg, vec![src_reg])
        }
        RiscvInstruction::SignExtendWord(dst, src) => {
            let dst_reg = register_to_string(dst);
            let src_reg = register_to_string(src);
            BoogieInstruction::Instr("sext".to_string(), SideEffect::Local, dst_reg, vec![src_reg])
        }
        RiscvInstruction::Jump { rd, label } => {
            if let Some(label) = label {
                BoogieInstruction::Branch(vec![label.clone()], "true".to_string())
            } else {
                let reg = register_to_string(rd);
                if reg.to_lowercase() == "ra" {
                    // @TODO: check that ra is maintained properly! perhaps just emit an assertion that ra==old(ra)
                    BoogieInstruction::Return
                } else {
                    // TODO: support this in boogie!
                    BoogieInstruction::Instr("jr".to_string(), SideEffect::Local, reg.to_string(), vec![])
                }
            }
        }
        RiscvInstruction::Return => BoogieInstruction::Return,
        RiscvInstruction::Directive(d) => BoogieInstruction::Comment(format!("Directive: {:?}", d)),
        RiscvInstruction::Unhandled(instr) => {
            log::warn!("Unhandled: {:?}", instr);
            BoogieInstruction::Unhandled(format!("Unhandled Risc Instruction {}", instr))
        }
    }
}

fn register_to_string(reg: &Register) -> String {
    match &reg.reg_type {
        RegisterType::A => format!("a{}", reg.number.unwrap()),
        RegisterType::T => format!("t{}", reg.number.unwrap()),
        RegisterType::S => format!("s{}", reg.number.unwrap()),
        RegisterType::X => format!("x{}", reg.number.unwrap()),
        RegisterType::Special(name) => name.to_string(),
    }
}

fn operand_to_boogie(operand: &Operand) -> String {
    match operand.clone() {
        Operand::Register(reg) => register_to_string(&reg),
        Operand::Immediate(val) => format!("{}bv64", val as u64),
        Operand::Memory(op) => {
            if op.offset == 0 {
                register_to_string(&op.base)
            } else {
                format!("{}+{}", register_to_string(&op.base), op.offset)
            }
        }
        _ => unimplemented!(),
    }
}

fn fence_mode_to_boogie(mode: &FenceMode) -> (bool, bool) {
    match mode {
        FenceMode::Read => (true, false),
        FenceMode::Write => (false, true),
        FenceMode::ReadWrite => (true, true),
        FenceMode::OutputWrite => (false, true), // TODO: fix this
        FenceMode::InputOutputReadWrite => (true, true),
    }
}
