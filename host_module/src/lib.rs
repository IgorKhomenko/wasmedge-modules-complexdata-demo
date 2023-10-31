// Define a global mutable buffer as shared memory
static mut SHARED_MEMORY: [u8; 10] = [0; 10];

static mut VAL:i32 = 5;

// Function to access and modify the shared memory
#[no_mangle]
pub unsafe extern "C" fn write_shared_memory(data: *const u8, len: usize) {
    println!("write_shared_memory1: {:#?}", SHARED_MEMORY);
    println!("write_shared_memory len: {:#?}", len);
    println!("write_shared_memory SHARED_MEMORY.len(): {:#?}", SHARED_MEMORY.len());

    println!("write_shared_memory data: {:#?}", data);

    println!("write_shared_memory DO");
    let src = data as *const u8;
    let dest = SHARED_MEMORY.as_mut_ptr().wrapping_offset(0);
    std::ptr::copy(src, dest, len);
    
    println!("write_shared_memory2: {:#?}", SHARED_MEMORY);
}

#[no_mangle]
pub fn ping() {
  println!("pong");
}

#[no_mangle]
pub unsafe fn set(val: i32) {
  VAL = val;
}

#[no_mangle]
pub unsafe fn get() -> i32 {
  return VAL;
}

#[no_mangle]
pub fn log_array(val: [u8; 5]) {
  println!("log_array: {:#?}", val);
}
