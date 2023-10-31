#include <iostream>
#include <wasmedge/wasmedge.h>
#include <stdio.h>
#include <string>
#include <sstream>

using namespace std;


/* This function can add 2 i32 values and return the result. */
WasmEdge_Result Add(void *, const WasmEdge_CallingFrameContext *,
                    const WasmEdge_Value *In, WasmEdge_Value *Out) {
  /*
  * Params: {i32, i32}
  * Returns: {i32}
  */

  /* Retrieve the value 1. */
  int32_t Val1 = WasmEdge_ValueGetI32(In[0]);
  /* Retrieve the value 2. */
  int32_t Val2 = WasmEdge_ValueGetI32(In[1]);
  /* Output value 1 is Val1 + Val2. */
  Out[0] = WasmEdge_ValueGenI32(Val1 + Val2);
  /* Return the status of success. */
  return WasmEdge_Result_Success;
}

WasmEdge_ModuleInstanceContext *CreateExternModule() {
  // 1. Create a function type and function context
  enum WasmEdge_ValType ParamList[2] = {WasmEdge_ValType_I32,
                                        WasmEdge_ValType_I32};
  enum WasmEdge_ValType ReturnList[1] = {WasmEdge_ValType_I32};
  /* Create a function type: {i32, i32} -> {i32}. */
  WasmEdge_FunctionTypeContext *HostFType =
      WasmEdge_FunctionTypeCreate(ParamList, 2, ReturnList, 1);
  /*
    * Create a function context with the function type and host function body.
    * The `Cost` parameter can be 0 if developers do not need the cost
    * measuring.
    */
  WasmEdge_FunctionInstanceContext *HostFunc = WasmEdge_FunctionInstanceCreate(HostFType, Add, NULL, 0);
  /*
    * The third parameter is the pointer to the additional data.
    * Developers should guarantee the life cycle of the data, and it can be NULL if the external data is not needed.
    */

  // 2. Module Instance Creation
  WasmEdge_String HostModuleName = WasmEdge_StringCreateByCString("native_functions");
  WasmEdge_ModuleInstanceContext *HostModCxt = WasmEdge_ModuleInstanceCreate(HostModuleName);

  /* 3. Add the host function created above with the export name "add". */
  WasmEdge_String HostFuncName = WasmEdge_StringCreateByCString("add");
  WasmEdge_ModuleInstanceAddFunction(HostModCxt, HostFuncName, HostFunc);

  // 4. cleanup
  WasmEdge_StringDelete(HostFuncName);
  WasmEdge_StringDelete(HostModuleName);
  WasmEdge_FunctionTypeDelete(HostFType);

  return HostModCxt;
}

int run(char *wasmFile, char *wasmFunction, int argc, char **argv, int preopenLen, const char *const *preopens) {
  /* Create the configure context and add the WASI support. */
  /* This step is not necessary unless you need WASI support. */
  WasmEdge_ConfigureContext *ConfCxt = WasmEdge_ConfigureCreate();
  WasmEdge_ConfigureAddHostRegistration(ConfCxt, WasmEdge_HostRegistration_Wasi);

  /* The configure and store context to the VM creation can be NULL. */
  WasmEdge_StoreContext *StoreCxt = WasmEdge_StoreCreate();
  WasmEdge_VMContext *VMCxt = WasmEdge_VMCreate(ConfCxt, StoreCxt);

  WasmEdge_ExecutorContext *ExecCxt = WasmEdge_ExecutorCreate(NULL, NULL);

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


  // Add internal host function
  //

  WasmEdge_ModuleInstanceContext *HostModCxt = CreateExternModule();

  // 4. Register Host Modules to WasmEdge
  /* Register the module instance into the store. */
  Res = WasmEdge_ExecutorRegisterImport(ExecCxt, StoreCxt, HostModCxt);
  if (!WasmEdge_ResultOK(Res)) {
    printf("Host module registration failed: %s\n",
          WasmEdge_ResultGetMessage(Res));
    return -1;
  }
  
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

  WasmEdge_ModuleInstanceDelete(HostModCxt);
  WasmEdge_StoreDelete(StoreCxt);
  WasmEdge_ExecutorDelete(ExecCxt);
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