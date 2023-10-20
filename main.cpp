#include <iostream>
#include <wasmedge/wasmedge.h>
#include <stdio.h>
#include <string>
#include <sstream>

using namespace std;

int run(char *wasmFile, char *wasmFunction, int argc, char **argv, int preopenLen, const char *const *preopens) {
  /* Create the configure context and add the WASI support. */
  /* This step is not necessary unless you need WASI support. */
  WasmEdge_ConfigureContext *ConfCxt = WasmEdge_ConfigureCreate();
  WasmEdge_ConfigureAddHostRegistration(ConfCxt, WasmEdge_HostRegistration_Wasi);

  /* The configure and store context to the VM creation can be NULL. */
  WasmEdge_StoreContext *StoreCxt = WasmEdge_StoreCreate();
  WasmEdge_VMContext *VMCxt = WasmEdge_VMCreate(ConfCxt, StoreCxt);

  // add args/envs/preopens
  WasmEdge_ModuleInstanceContext *WasiCxt = WasmEdge_VMGetImportModuleContext(VMCxt, WasmEdge_HostRegistration_Wasi);
  WasmEdge_ModuleInstanceInitWASI(WasiCxt, argv, argc, NULL, 0, preopens, preopenLen);


  WasmEdge_Result Res;

  // connect host module
  //
  WasmEdge_String ModuleName = WasmEdge_StringCreateByCString("host_module");

  // // host functions written in rust
  Res = WasmEdge_VMRegisterModuleFromFile(VMCxt, ModuleName, "host_module/target/wasm32-wasi/release/host_module.wasm");

  if (!WasmEdge_ResultOK(Res)) {
    WasmEdge_VMDelete(VMCxt);
    printf("Register host functions error: %s\n", WasmEdge_ResultGetMessage(Res));
    return -1;
  }

  WasmEdge_StringDelete(ModuleName);
  //
  //

  // /* The parameters and returns arrays. */
  WasmEdge_Value Params[0];
  WasmEdge_Value Returns[0];
  // /* Function name. */
  WasmEdge_String FuncName = WasmEdge_StringCreateByCString(wasmFunction);


 /* Step 1: Load WASM file. */
  Res = WasmEdge_VMLoadWasmFromFile(VMCxt, wasmFile); 
 
  /*
   * Developers can load the WASM binary from buffer with the
   * `WasmEdge_VMLoadWasmFromBuffer()` API, or from
   * `WasmEdge_ASTModuleContext` object with the
   * `WasmEdge_VMLoadWasmFromASTModule()` API.
   */
  if (!WasmEdge_ResultOK(Res)) {
    printf("Loading phase failed: %s\n", WasmEdge_ResultGetMessage(Res));
    return 1;
  }
  /* Step 2: Validate the WASM module. */
  Res = WasmEdge_VMValidate(VMCxt);
  if (!WasmEdge_ResultOK(Res)) {
    printf("Validation phase failed: %s\n", WasmEdge_ResultGetMessage(Res));
    return 1;
  }
  /* Step 3: Instantiate the WASM module. */
  Res = WasmEdge_VMInstantiate(VMCxt);
  /*
   * Developers can load, validate, and instantiate another WASM module to
   * replace the instantiated one. In this case, the old module will be
   * cleared, but the registered modules are still kept.
   */
  if (!WasmEdge_ResultOK(Res)) {
    printf("Instantiation phase failed: %s\n",
           WasmEdge_ResultGetMessage(Res));
    return 1;
  }

   /*
   * Step 4: Execute WASM functions. You can execute functions repeatedly
   * after instantiation.
   */
  Res = WasmEdge_VMExecute(VMCxt, FuncName, Params, 0, Returns, 0);
  
  if (WasmEdge_ResultOK(Res)) {
    // printf("\nExecuted successfully\n");
  } else {
    printf("\nExecution phase failed: %s\n", WasmEdge_ResultGetMessage(Res));
  }

  
  /* Resources deallocations. */
  WasmEdge_VMDelete(VMCxt);
  WasmEdge_ConfigureDelete(ConfCxt);
  WasmEdge_StringDelete(FuncName);

  return 0;
}

int main(int argc, char **argv) {
  char *wasmFile = argv[1];
  char *wasmFunction = argv[2];

  run(wasmFile, wasmFunction, argc, argv, 0, NULL);

  return 0;
}