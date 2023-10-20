# How to compile

1. Compile host module

```
cd host_module
cargo build --release --target=wasm32-wasi
cd ..
```

2. Compile user module

```
cd user_module
cargo build --release --target=wasm32-wasi
cd ..
```

3. run app

```
gcc main.cpp -lwasmedge && ./a.out user_module/target/wasm32-wasi/release/user_module.wasm hello
```