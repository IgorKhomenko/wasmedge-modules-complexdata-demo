#[no_mangle]
pub fn hello() {
  unsafe {
      // // create a `Vec<u8>` as input
      // let input = vec![1 as u8, 2, 3, 4, 5];
      let input: [u8; 5] = [5, 2, 3, 4, 5];
      println!("input: {:#?}", input);

      // Allocate memory from the host module
      let len = input.len();

      host_module::write_shared_memory(input.as_ptr(), len);

      host_module::set(10);
      println!("get {:#?}", host_module::get());
      //
      host_module::set(20);
      println!("get {:#?}", host_module::get());

      // host_module::log_array(input);

      println!("add {:#?}", native_zera::add(2, 2));
  }
}

pub mod host_module {
  #[link(wasm_import_module = "host_module")]
  extern "C" {
      pub fn write_shared_memory(data: *const u8, len: usize);
      pub fn log_shared_memory();

      pub fn set(val: i32);
      pub fn get() -> i32;

      pub fn log_array(val: [u8; 5]);
  }
}

pub mod native_zera {
  #[link(wasm_import_module = "native_zera")]
  extern "C" {
      pub fn add(val1: i32, val2: i32) -> i32;
  }
}