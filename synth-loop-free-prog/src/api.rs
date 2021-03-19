use crate::{Program, component, Specification, Library, Synthesizer, ProgramBuilder, Result};
use structopt::*;
use std::os::raw::c_char;
use std::ffi::{CString, CStr};

#[no_mangle]
pub extern fn run(components_str: *const c_char, program_str: *const c_char) -> *const c_char{
    let comp_str = unsafe { CStr::from_ptr(components_str)};
    let components = match comp_str.to_str() {
        Err(_) => "there",
        Ok(string) => string,
    };

    let prog_str = unsafe { CStr::from_ptr(program_str)};
    let program = match prog_str.to_str() {
        Err(_) => "there",
        Ok(string) => string,
    };
    // env_logger::init();

    let opts = Options::from_args();

    let mut config = z3::Config::new();
    config.set_bool_param_value("auto_config", false);
    config.set_model_generation(true);
    let context = z3::Context::new(&config);
    
    println!("==================== P1 ====================");
    let then = std::time::Instant::now();
    let program = synthesize_prog(&context, &opts, components, program);
    let elapsed = then.elapsed();

    println!(
        "\nElapsed: {}.{:03}s\n",
        elapsed.as_secs(),
        elapsed.subsec_millis()
    );
    match program {
        Ok(prog) => {
            let ret_str = CString::new(prog.to_string()).expect("CString::new failed");
            // let ret_str = CString::new("test").expect("CString::new failed");
            // println!("{}", prog.to_string());
            return ret_str.into_raw();
            // println!("Synthesized:\n\n{}", prog);
        }
        Err(e) => {
            let ret_str = CString::new("Error").expect("CString::new failed");
            return ret_str.into_raw();
            // println!("Error: {:?}\n", e);
        }
    }
}

#[derive(StructOpt)]
struct Options {
    /// Set a timeout, in milliseconds.
    #[structopt(short = "t", long = "timeout")]
    timeout: Option<u32>,

    /// Synthesize the optimally smallest programs.
    #[structopt(short = "m", long = "minimal")]
    minimal: bool,

    /// Should constants be given or synthesized? It isn't always clear which
    /// they did in the paper, and sort seems like they did a mix depending on
    /// the benchmark problem.
    #[structopt(short = "c", long = "synthesize-constants")]
    synthesize_constants: bool,
}


fn synthesize(
    opts: &Options,
    context: &z3::Context,
    spec: &dyn Specification,
    library: &Library,
) -> Result<Program> {
    Synthesizer::new(context, library, spec)?
        .set_timeout(opts.timeout)
        .should_synthesize_minimal_programs(true)//opts.minimal)
        .synthesize()
}

fn synthesize_prog(context: &z3::Context, opts: &Options, components: &str,
     program: &str) -> Result<Program> {
    let mut library = Library{components: vec![]};

    let components_list = components.split(",");
    for comp in components_list {
        match comp {
            "Add" => library.components.push(component::add()),
            "Sub" => library.components.push(component::sub()),
            "Xor" => library.components.push(component::xor()),
            "And" => library.components.push(component::and()),
            _ => println!("Unknown Component"),
        }
        // println!("{}", comp);
    }
    library.components.push(component::const_(Some(0)));
    library.components.push(component::const_(Some(1)));
    library.components.push(component::const_(Some(std::u64::MAX)));

    // library
    //     .components
    //     .push(component::const_(if opts.synthesize_constants {
    //         None
    //     } else {
    //         None
    //         // Some(1)
    //     }));
    
    // library.components.iter().for_each(|it| {
    //     println!("{:#?}", it);
    // });
    
    let mut builder = ProgramBuilder::new();
    let mut vars = Vec::new();
    let program_list = program.split(",");
    for line in program_list {
        let operands = line.split(" ").collect::<Vec<&str>>();
        match operands[0] {
            "Var" => vars.push(builder.var()),
            "Const" => vars.push(builder.const_(operands[1].parse().unwrap())),
            "Mul" => {
                let a: usize = operands[1].parse().unwrap();
                let b: usize = operands[2].parse().unwrap();
                vars.push(builder.mul(vars[a], vars[b]));
            },
            _ => println!("Unknown program line"),
        }
    }
    // let a = builder.var();
    // let b = builder.const_(1);
    // let b = builder.const_(3);
    // let _ = builder.mul(a, b);
    // let c = builder.sub(a, b);
    // let _ = builder.and(a, c);
    let spec = builder.finish();

    synthesize(opts, context, &spec, &library)
}