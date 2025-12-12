/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
use generate::boogie_to_string;
use loops::unroll;
use phf::phf_map;
use std::{
     fs, io::Write, path::Path
};

use lazy_static::lazy_static;
use regex::Regex;

pub mod arm;
pub mod generate;
pub mod riscv;
pub mod loops;
pub const DUMMY_REG: &str = "dummy";

pub struct AssemblyFunction<'a> {
    pub name: &'a str,
    pub code: Vec<&'a str>,
}

#[derive(Debug, Clone, PartialEq)]
pub enum FenceConvention {
    RCsc,
    TrailingFence,
    LeadingFence,
    Mixed,
}

pub trait Arch {
    fn name(&self) -> String;
    fn all_registers(&self) -> Vec<String>;
    fn width(&self) -> Width;
    fn parse_functions(
        &self,
        assembly: &str,
    ) -> Result<Vec<BoogieFunction>, Box<dyn std::error::Error>>;
    fn state(&self) -> String;
    fn fence_convention(&self) -> FenceConvention;
}


#[derive(Debug, Clone, PartialEq, Copy)]
pub enum SideEffect {
    Local,
    Global,
}

#[derive(Debug, Clone, PartialEq)]
pub enum BoogieInstruction {
    Label(String),
    Instr(String, SideEffect, String, Vec<String>),
    Branch(Vec<String>, String),
    Unhandled(String),
    Comment(String),
    Return,
}

#[derive(Debug, Clone, PartialEq, Copy)]
pub enum ReturnType {
    Return,
    NoReturn,
}

#[derive(Debug, Clone, PartialEq, Copy)]
pub enum FunctionClass {
    Read,
    Write,
    Await(ReturnType),
    AwaitRmw,
    Rmw(ReturnType),
    Fence,
}

static FENCE_ORDERING: phf::Map<&'static str, &'static str> = phf_map! {
    "" => "order_fence_sc",
    "_acq" => "order_fence_acq",
    "_rel" => "order_fence_rel",
    "_rlx" => "order_rlx",
};

static ORDERING: phf::Map<&'static str, (&'static str, &'static str)> = phf_map! {
    "" => ("order_acq_sc","order_rel_sc"),
    "_acq" => ("order_acq","order_rlx"),
    "_rel" => ("order_rlx","order_rel"),
    "_rlx" => ("order_rlx","order_rlx"),
};

static AWAIT_OP: phf::Map<&'static str, &'static str> = phf_map! {
    "eq" => "eq",
    "neq" => "neq",
    "lt" => "lt",
    "le" => "le",
    "gt" => "gt",
    "ge" => "ge",
};

static RMW_OP: phf::Map<&'static str, &'static str> = phf_map! {
    "cmpxchg" => "cmpset",
    "add" => "add_op",
    "sub" => "sub_op",
    "xchg" => "set_op",
    "set" => "set_op",
    "dec" => "dec_op",
    "inc" => "inc_op",
    "min" => "min_op",
    "max" => "max_op",
    "and" => "and_op",
    "xor" => "xor_op",
    "or" => "or_op"
};

static ATOMIC_TYPE: phf::Map<&'static str, AtomicType> = phf_map! {
    "64" => AtomicType::V64,
    "sz" => AtomicType::VSZ,
    "ptr" => AtomicType::VPTR,
    "32" => AtomicType::V32,
    "16" => AtomicType::V16,
    "8" => AtomicType::V8,
    "" => AtomicType::VFENCE,
};

lazy_static! {
    static ref RETURNING_RMW : Regex = Regex::new(r"get|cmpxchg|xchg").unwrap();
    // @TODO: generate automatically from the keys
    static ref RMW_RE : Regex = Regex::new(r"(?<get_>get_)?(?<type>add|sub|set|cmpxchg|min|max|xchg|dec|inc|and|xor|or)(?<_get>_get)?").unwrap();
    static ref ORDERING_RE : Regex = Regex::new(r"(_rlx|_acq|_rel|)$").unwrap();
    static ref AWAIT_RE : Regex = Regex::new(r"await_([^_]+)").unwrap();
    static ref WIDTH_RE : Regex = Regex::new(r"8|16|32|sz|ptr|64").unwrap();
}

fn classify_function(name: &str) -> FunctionClass {
    if name.contains("read") {
        FunctionClass::Read
    } else if name.contains("write") {
        FunctionClass::Write
    } else if name.contains("await") {
        if RMW_RE.is_match(name) {
            FunctionClass::AwaitRmw
        } else {
            FunctionClass::Await(if name.contains("eq") {
                ReturnType::NoReturn
            } else {
                ReturnType::Return
            })
        }
    } else if name.contains("fence") {
        FunctionClass::Fence
    } else {
        let ret = if RETURNING_RMW.captures(name).is_some() {
            ReturnType::Return
        } else {
            ReturnType::NoReturn
        };
        FunctionClass::Rmw(ret)
    }
}

fn get_templates_for_type(func_type: FunctionClass) -> Vec<&'static str> {
    match func_type {
        FunctionClass::Read => vec!["read_only.bpl", "read.bpl"],
        FunctionClass::Write => vec!["write.bpl", "must_store.bpl"],
        FunctionClass::Await(ReturnType::NoReturn) => vec!["await.bpl"],
        FunctionClass::Await(ReturnType::Return) => vec!["read_only.bpl", "read.bpl", "await.bpl"],
        FunctionClass::Rmw(ReturnType::NoReturn) => vec!["write.bpl", "rmw.bpl"],
        FunctionClass::Rmw(ReturnType::Return) => vec!["read.bpl", "write.bpl", "rmw.bpl"],
        FunctionClass::AwaitRmw => vec!["read.bpl", "write.bpl", "rmw.bpl", "await.bpl"],
        FunctionClass::Fence => vec!["fence.bpl"],
    }
}

#[derive(Debug, Clone)]
pub struct BoogieFunction {
    pub name: String,
    pub address: String,
    pub input1: String,
    pub input2: String,
    pub output: String,
    pub instructions: Vec<BoogieInstruction>,
}

pub trait ToBoogie {
    fn to_boogie(self) -> BoogieFunction;
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Width {
    Thin,
    Wide,
}

pub fn wide_arch_widths(type_name: AtomicType) -> u32 {
    match type_name {
        AtomicType::V8 => 1,
        AtomicType::V16 => 2,
        AtomicType::V32 => 4,
        _ => 8,
    }
}

pub fn thin_arch_widths(type_name: AtomicType) -> u32 {
    match type_name {
        AtomicType::V8 => 1,
        AtomicType::V16 => 2,
        AtomicType::VSZ | AtomicType::VPTR | AtomicType::V32 => 4,
        _ => 8,
    }
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum AtomicType {
    V64,
    VSZ,
    VPTR,
    V32,
    V16,
    V8,
    VFENCE,
}

pub fn atomic_types(function_name: &str) -> AtomicType {
    WIDTH_RE
        .captures(function_name)
        .map(|c| ATOMIC_TYPE[&c[0]])
        .unwrap_or(AtomicType::VFENCE)
}

impl AtomicType {
    pub fn type_width(&self, arch_width: Width) -> u32 {
        match arch_width {
            Width::Thin => thin_arch_widths(*self),
            Width::Wide => wide_arch_widths(*self),
        }
    }
}

fn get_assumptions(
    func_type: &str,
    load_order: &str,
    store_order: &str,
    rmw_op: &str,
    ret_op: &str,
    cond: &str,
) -> String {
    match func_type {
        "fence.bpl" => std::format!("    assume (fence_order == {});\n", load_order),
        "read.bpl" => std::format!(
            "    assume (load_order == {});\n    assume (ret == {});\n",
            load_order,
            ret_op
        ),
        "write.bpl" => std::format!("    assume (store_order == {});\n", store_order),
        "await.bpl" => std::format!("    assume (cond == {});\n", cond),
        "rmw.bpl" => std::format!("    assume (op == {});\n", rmw_op),
        _ => "".to_string(),
    }
}

pub fn generate_boogie_file(
    function: &BoogieFunction,
    output_dir: &str,
    template_dir: &str,
    arch: &dyn Arch,
    unroll_loop: bool,
) -> Result<(), std::io::Error> {
    let func_type = classify_function(&function.name);
    let mut templates = get_templates_for_type(func_type);
    templates.push("registers.bpl");

    let instructions = if unroll_loop { &unroll(&function.instructions ) } else { &function.instructions };

    let boogie_code = boogie_to_string(&instructions);

    let registers = arch.all_registers();

    let state = arch.state();

    let atomic_type = atomic_types(&function.name);

    let target_path = Path::new(output_dir).join(&function.name);
    fs::create_dir_all(&target_path)?;

    let mut rmw_op = "".to_string();
    let mut read_ret = "ret_old".to_string();
    if let Some(rmw_name) = RMW_RE.captures(&function.name) {
        if let Some(op) = RMW_OP.get(rmw_name.name("type").unwrap().as_str()) {
            rmw_op = op.to_string();

            if rmw_name.name("_get").is_some() {
                read_ret = op.to_string();
            }
        }

        if func_type == FunctionClass::AwaitRmw {
            rmw_op = format!("(lambda x, y1, y2: bv64 :: {}[x, y2, y1])", rmw_op);
        }
    }

    let mut await_cond = "".to_string();
    if let Some(await_name) = AWAIT_RE.captures(&function.name) {
        if let Some(op) = AWAIT_OP.get(&await_name[1]) {
            await_cond = op.to_string();
        }
    }

    let ordering = ORDERING_RE.captures(&function.name).unwrap();
    let (load_order, store_order) = if func_type == FunctionClass::Fence {
        (FENCE_ORDERING[&ordering[0]], "")
    } else {
        ORDERING[&ordering[0]]
    };

    match atomic_type {
        AtomicType::V8 => {
            await_cond = format!("bit8[{}]", await_cond);
            read_ret = format!("bit8[{}]", read_ret);
            rmw_op = format!("bit8[{}]", rmw_op);
        }
        AtomicType::V16 => {
            await_cond = format!("bit16[{}]", await_cond);
            read_ret = format!("bit16[{}]", read_ret);
            rmw_op = format!("bit16[{}]", rmw_op);
        }
        _ => {}
    }

    let pointer_size = 2u64
        .wrapping_pow(8 * AtomicType::VPTR.type_width(arch.width()))
        .wrapping_sub(1);
    let register_size = 2u64
        .wrapping_pow(8 * atomic_type.type_width(arch.width()))
        .wrapping_sub(1);

    for template in templates {
        let template_path = Path::new(template_dir).join(template);
        let template_content = fs::read_to_string(&template_path)?;

        let boogie_code_with_assume = format!(
            "
    assume (last_store < step);
    assume (sc_impl is {:?});
    assume (valid_mask({}, {}bv64));
    assume (valid_mask({}, {}bv64));
    assume (valid_mask({}, {}bv64));
    {}
    {}",
            arch.fence_convention(),
            function.address,
            pointer_size,
            function.input1,
            register_size,
            function.input2,
            register_size,
            get_assumptions(
                template,
                load_order,
                store_order,
                &rmw_op,
                &read_ret,
                &await_cond
            ),
            boogie_code
        );

        let content = template_content
            .replace("    #implementation", &boogie_code_with_assume)
            .replace("#registers", registers.join(",").as_str())
            .replace("#address", &function.address)
            .replace("#state", &state)
            .replace("#output", &function.output)
            .replace("#input1", &function.input1)
            .replace("#input2", &function.input2)
            .replace("#value_mask", &format!("{}bv64", register_size));

        fs::write(&target_path.join(template), content)?;
    }

    println!(
        "generated verification templates for function {} ({:?})",
        function.name, atomic_type
    );
    Ok(())
}

pub fn generate_debug_file(boogie: &[BoogieFunction], path: &str) -> Result<(), std::io::Error> {
    let mut file = fs::File::create(path)?;
    for function in boogie {
        let boogie_code = boogie_to_string(&function.instructions);

        let content = format!("// ---- {} ----\n{}\n", function.name, boogie_code);
        writeln!(file, "{:#?}", content)?;
    }
    Ok(())
}
