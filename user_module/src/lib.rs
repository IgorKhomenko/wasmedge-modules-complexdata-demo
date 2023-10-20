#[no_mangle]
pub fn hello() {
  unsafe {
      // create a `Vec<u8>` as input
      let input = vec![1 as u8, 2, 3, 4, 5];

      // call the `alloc` function
      let ptr = host_module::alloc(input.len());
      let res: u8;

      // copy the contents of `input`into the buffer
      // returned by `alloc`
      std::ptr::copy(input.as_ptr(), ptr, input.len());
      // call the `array_sum` function with the pointer
      // and the length of the array
      res = host_module::array_sum(ptr, input.len());

      // print the result
      println!("Result: {:#?}", res);
  }
}

pub mod host_module {
  #[link(wasm_import_module = "host_module")]
  extern "C" {
    pub fn alloc(len: usize) -> *mut u8;
    pub fn array_sum(ptr: *mut u8, len: usize) -> u8;
  }
}