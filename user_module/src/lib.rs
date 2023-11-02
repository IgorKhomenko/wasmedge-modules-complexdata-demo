use native_functions::native_functions;

#[no_mangle]
pub fn hello() {
  unsafe {
      // //
      // //
      // let input = "https://www.google.com";
	    // let pointer = input.as_bytes().as_ptr();
      // let length = native_functions::string_length(pointer, input.len() as i32) as usize;
      // println!("string_length {:#?}: {:#?}", input, length);

      //
      //
      let val1 = 4;
      let val2 = 5;
      println!("add {:#?}+{:#?}: {:#?}", val1, val2, native_functions::add(val1, val2));

      //
      //
      let input = "https://www.google.com";
      println!("string_odd_or_even {:#?}: {:#?}", input, native_functions::wrap::string_odd_or_even(input));
  }
}