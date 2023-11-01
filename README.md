# How to compile

1. Compile user module

```
cd user_module
cargo build --release --target=wasm32-wasi
cd ..
```

2. run host app

```
gcc main.cpp -lwasmedge && ./a.out user_module/target/wasm32-wasi/release/user_module.wasm hello
```