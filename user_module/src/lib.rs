#[no_mangle]
pub fn hello() {
  unsafe {
      let url = "https://www.google.com";
	    let pointer = url.as_bytes().as_ptr();
      let res_len = native_functions::string_length(pointer, url.len() as i32) as usize;
      println!("string_length {:#?}: {:#?}", "https://www.google.com", res_len);
  }
}

pub mod native_functions {
  #[link(wasm_import_module = "native_functions")]
  extern "C" {
      pub fn string_length(url_pointer: *const u8, url_length: i32) -> i32;
  }
}