pub mod native_functions {
  pub mod wrap {
    use crate::native_functions; // https://doc.rust-lang.org/beta/reference/items/use-declarations.html

    pub unsafe fn string_odd_or_even(input: &str) -> String {
      let source_pointer = input.as_bytes().as_ptr();
  
      let mut buffer = Vec::with_capacity(0);
      let target_pointer = buffer.as_mut_ptr();
  
      let result_len = native_functions::string_odd_or_even(source_pointer, input.len() as i32, target_pointer) as usize;
  
      buffer.set_len(result_len);
      let res = String::from_utf8(buffer).unwrap();
  
      return res;
    }
  }

  #[link(wasm_import_module = "native_functions")]
  extern "C" {
      pub fn string_length(source_pointer: *const u8, source_length: i32) -> i32;
      pub fn string_odd_or_even(source_pointer: *const u8, source_length: i32, target_pointer: *const u8) -> i32;
      pub fn add(val1: i32, val2: i32) -> i32;
  }
}