#[no_mangle]
pub fn hello() {
  unsafe {
      // //
      // //
      // let input = "https://www.google.com";
	    // let pointer = input.as_bytes().as_ptr();
      // let length = native_functions::string_length(pointer, input.len() as i32) as usize;
      // println!("string_length {:#?}: {:#?}", input, length);

      // //
      // //
      // let val1 = 4;
      // let val2 = 5;
      // println!("add {:#?}+{:#?}: {:#?}", val1, val2, native_functions::add(val1, val2));

      //
      //
      let input2 = "https://www.google.com";
	    let source_pointer = input2.as_bytes().as_ptr();

      let mut buffer = Vec::with_capacity(0);
	    let target_pointer = buffer.as_mut_ptr();

      let result_len = native_functions::string_odd_or_even(source_pointer, input2.len() as i32, target_pointer) as usize;

      buffer.set_len(result_len);
	    let res = std::str::from_utf8(&buffer).unwrap();

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