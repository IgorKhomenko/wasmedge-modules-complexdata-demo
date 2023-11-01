#[no_mangle]
pub fn hello() {
  unsafe {
      let input = "https://www.google.com";
	    let pointer = input.as_bytes().as_ptr();
      let res_len = native_functions::string_length(pointer, input.len() as i32) as usize;
      println!("string_length {:#?}: {:#?}", input, res_len);

      let val1 = 4;
      let val2 = 5;
      println!("add {:#?}+{:#?}: {:#?}", val1, val2, native_functions::add(val1, val2));

      let input2 = "https://www.google.com";
	    let source_pointer = input2.as_bytes().as_ptr();

      let res = "123";
      let target_pointer = res.as_bytes().as_ptr();

      native_functions::string_odd_or_even(source_pointer, input2.len() as i32, target_pointer) as usize;
      println!("string_odd_or_even {:#?}: {:#?}", input2, res);
  }
}

pub mod native_functions {
  #[link(wasm_import_module = "native_functions")]
  extern "C" {
      pub fn string_length(source_pointer: *const u8, source_length: i32) -> i32;
      pub fn string_odd_or_even(source_pointer: *const u8, source_length: i32, target_pointer: *const u8) -> i32;
      pub fn add(val1: i32, val2: i32) -> i32;
  }
}