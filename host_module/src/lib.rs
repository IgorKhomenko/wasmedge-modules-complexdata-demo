/// Allocate memory into the module's linear memory
/// and return the offset to the start of the block.
#[no_mangle]
pub fn alloc(len: usize) -> *mut u8 {
  // create a new mutable buffer with capacity `len`
  let mut buf = Vec::with_capacity(len);
  // take a mutable pointer to the buffer
  let ptr = buf.as_mut_ptr();
  // take ownership of the memory block and
  // ensure that its destructor is not
  // called when the object goes out of scope
  // at the end of the function

  println!("alloc: {:#?}", len);

  std::mem::forget(buf);
  // return the pointer so the runtime
  // can write data at this offset
  return ptr;
}

/// Given a pointer to the start of a byte array and
/// its length, return the sum of its elements.
#[no_mangle]
pub unsafe fn array_sum(ptr: *mut u8, len: usize) -> u8 {
    println!("array_sum: {:#?}", len);

    // create a Vec<u8> from the pointer to the
    // linear memory and the length
    let data = Vec::from_raw_parts(ptr, len, len);
    println!("array_sum data: {:#?}", data);

    // actually compute the sum and return it
    data.iter().sum()
}

#[no_mangle]
pub fn ping() {
  println!("pong");
}