/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
use asm2boogie::arm::{self, extract_arm_functions, parse_arm_assembly};
use asm2boogie::riscv::{self, extract_riscv_functions, parse_riscv_assembly};
use asm2boogie::{Arch, ToBoogie, generate_boogie_file, generate_debug_file};
use asm2boogie::{BoogieFunction, DUMMY_REG, FenceConvention, Width};

use clap::{Parser, ValueEnum};
use std::path::Path;
use std::{fs, iter};

enum OutputMode {
    File(String),
    Directory(String),
}

#[derive(Debug, Clone, Copy, PartialEq, ValueEnum)]
enum ArchSpecifier {
    #[value(name = "armv8")]
    ArmV8,

    #[value(name = "riscv")]
    RiscV,
}

impl Arch for ArchSpecifier {
    fn name(&self) -> String {
        format!("{:?}", self)
    }

    fn all_registers(&self) -> Vec<String> {
        match self {
            ArchSpecifier::ArmV8 => iter::once(DUMMY_REG.to_string())
                .chain((0..=31).flat_map(|i| [format!("x{}", i), format!("w{}", i)]))
                .collect(),
            ArchSpecifier::RiscV => iter::once(DUMMY_REG.to_string())
                .chain((0..=7).map(|i| format!("a{}", i)))
                .chain((0..=11).map(|i| format!("s{}", i)))
                .chain((0..=6).map(|i| format!("t{}", i)))
                .collect()
        }
    }

    fn width(&self) -> asm2boogie::Width {
        Width::Wide
    }

    fn parse_functions(
        &self,
        assembly: &str,
    ) -> Result<Vec<BoogieFunction>, Box<dyn std::error::Error>> {
        /* @TODO intermediate step -- Vec<AsmFunction>. AsmFunction -> BoogieFunction. Here AsmFunction is a generic type of just name + string defining code. */
        match self {
            ArchSpecifier::ArmV8 => {
                let (_, parsed_asm) =
                    parse_arm_assembly(assembly).map_err(|err| err.to_string())?;
                let functions =
                    extract_arm_functions(parsed_asm, None, &["ptr", "32", "64", "sz", "8", ""])
                        .into_iter()
                        .map(|f| arm::transform_labels(&f));
                Ok(functions.into_iter().map(ToBoogie::to_boogie).collect())
            }
             ArchSpecifier::RiscV => {
                let (_, parsed_asm) =
                    parse_riscv_assembly(assembly).map_err(|err| err.to_string())?;
                let functions =
                    extract_riscv_functions(parsed_asm, None, &["ptr", "32", "64", "sz", "8", ""])
                        .into_iter()
                        .map(|f| riscv::transform_labels(&f));
                Ok(functions.into_iter().map(ToBoogie::to_boogie).collect())
            }
        }
    }

    fn state(&self) -> String {
        match self {
            ArchSpecifier::ArmV8 => {
                "local_monitor, monitor_exclusive, flags, event_register".to_string()
            }
            ArchSpecifier::RiscV => "local_monitor, monitor_exclusive".to_string()
        }
    }

    fn fence_convention(&self) -> FenceConvention {
        match self {
            ArchSpecifier::ArmV8 => FenceConvention::RCsc,
            ArchSpecifier::RiscV => FenceConvention::Mixed,
        }
    }
}

#[derive(Parser, Debug)]
#[clap(author, version, about = "Generate Verifiable Boogie Code from ASM", long_about = None)]
struct Args {
    #[clap(
        short = 'a',
        long,
        value_enum,
        default_value = "armv8",
        help = "Target architecture (armv8 or riscv)"
    )]
    arch: ArchSpecifier,

    #[clap(short = 'u', long, help = "unroll the main loop to prove at least one iteration of the loop occurs", action)]
    unroll: bool,

    #[clap(short = 'i', long, value_name = "FILE", help = "input file")]
    input: String,
    #[clap(short = 'f', long, value_name = "FILE", help = "function names")]
    functions: String,
    #[clap(
        short = 'o',
        long,
        value_name = "FILE",
        help = "Sets the output file",
        conflicts_with = "directory"
    )]
    output: Option<String>,
    #[clap(
        short = 'd',
        long,
        value_name = "DIR",
        help = "Sets the output directory",
        conflicts_with = "output"
    )]
    directory: Option<String>,
    #[clap(short = 't', long, value_name = "DIR", help = "template directory")]
    templates: Option<String>,
}

fn read_function_names(file_path: &str) -> Result<Vec<String>, std::io::Error> {
    let content = fs::read_to_string(file_path)?;
    Ok(content
        .lines()
        .map(str::trim)
        .filter(|s| !s.is_empty())
        .map(String::from)
        .collect())
}

fn ensure_directory_exists(path: &str) -> Result<(), std::io::Error> {
    if !Path::new(path).exists() {
        fs::create_dir_all(path)?;
    }
    Ok(())
}


fn main() -> Result<(), Box<dyn std::error::Error>> {
    env_logger::init();
    let args = Args::parse();

    let output_mode = match (&args.output, &args.directory) {
        (Some(file), None) => OutputMode::File(file.clone()),
        (None, Some(dir)) => {
            if args.templates.is_none() {
                eprintln!("Error: --templates is required when --directory is specified.");
                std::process::exit(1);
            }
            OutputMode::Directory(dir.clone())
        }
        (None, None) => {
            eprintln!("Error: Either --output or --directory must be specified.");
            std::process::exit(1);
        }
        (Some(_), Some(_)) => {
            unreachable!("Clap should prevent both output and directory from being specified.");
        }
    };

    let function_names = read_function_names(&args.functions).unwrap_or_else(|e| {
        eprintln!("Error reading functions file '{}': {}", args.functions, e);
        std::process::exit(1);
    });

    log::info!("Input file: {}", args.input);
    log::info!("Functions file: {}", args.functions);
    log::info!("Arch: {:?}", args.arch);

    if let Some(output) = &args.output {
        log::info!("Output file: {}", output);
    }
    if let Some(directory) = &args.directory {
        log::info!("Output directory: {}", directory);
    }
    if let Some(templates) = &args.templates {
        log::info!("Template directory: {}", templates);
    }

    log::info!("Function Extraction Names: {:?}", function_names);

    let input_content = fs::read_to_string(&args.input).unwrap_or_else(|e| {
        eprintln!("Error reading input file '{}': {}", args.input, e);
        std::process::exit(1);
    });
    log::info!("Successfully read input file '{}'", args.input);

    let name_re = regex::Regex::new(
        format!("^({})$", function_names.join("|")).as_str()
    )?;

    let boogie_functions: Vec<_> = args
        .arch
        .parse_functions(&input_content)?
        .iter()
        .filter(|func| name_re.is_match(func.name.as_str()))
        .cloned()
        .collect();

    match output_mode {
        OutputMode::File(file_path) => {
            log::info!("Generating output file: {}", file_path);
            generate_debug_file(&boogie_functions, &file_path)?;
        }
        OutputMode::Directory(dir_path) => {
            let template_dir = args.templates.as_ref().unwrap();
            log::info!("Generating output files in directory: {}", dir_path);
            ensure_directory_exists(&dir_path)?;

            for function in &boogie_functions {
                log::info!("Generating Boogie code for function: {}", function.name);
                generate_boogie_file(function, &dir_path, template_dir, &args.arch, args.unroll)?;
            }
        }
    }

    log::info!("Successfully generated Boogie code");
    Ok(())
}
