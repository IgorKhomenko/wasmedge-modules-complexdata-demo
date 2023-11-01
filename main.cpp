#include <iostream>
#include <wasmedge/wasmedge.h>
#include <stdio.h>
#include <string>
#include <sstream>

using namespace std;

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
  Out[1] = WasmEdge_ValueGenI32(99);
  /* Return the status of success. */
  return WasmEdge_Result_Success;
}

WasmEdge_Result StringLength(void *, const WasmEdge_CallingFrameContext *CallFrameCxt,
                    const WasmEdge_Value *In, WasmEdge_Value *Out) {
  /*
  * Params: {i32, i32}
  * Returns: {i32}
  */

  uint32_t Pointer = WasmEdge_ValueGetI32(In[0]);
  uint32_t Size = WasmEdge_ValueGetI32(In[1]);

  unsigned char Url[Size];

  // printf("Pointer: %u\n", Pointer);
  // printf("Size: %u\n", Size);

  // https://wasmedge.org/docs/embed/c/host_function/#calling-frame-context
  // https://www.secondstate.io/articles/extend-webassembly/
  WasmEdge_MemoryInstanceContext *MemCxt = WasmEdge_CallingFrameGetMemoryInstance(CallFrameCxt, 0);
  // read data
  WasmEdge_Result Res = WasmEdge_MemoryInstanceGetData(MemCxt, Url, Pointer, Size);
  if (WasmEdge_ResultOK(Res)) {
    // printf("u32 at memory[%u]: %s\n", Pointer, Url);

    Out[0] = WasmEdge_ValueGenI32(Size);

    return WasmEdge_Result_Success;
  } else {
    return Res;
  }
}

WasmEdge_Result StringOddOrEven(void *, const WasmEdge_CallingFrameContext *CallFrameCxt,
                    const WasmEdge_Value *In, WasmEdge_Value *Out) {
  /*
  * Params: {i32, i32, i32}
  * Returns: {i32}
  */

  uint32_t SourcePointer = WasmEdge_ValueGetI32(In[0]);
  uint32_t SourceSize = WasmEdge_ValueGetI32(In[1]);
  uint32_t TargetPointer = WasmEdge_ValueGetI32(In[2]);

  unsigned char Url[SourceSize];

  // printf("Pointer: %u\n", SourcePointer);
  // printf("Size: %u\n", SourceSize);

  // https://wasmedge.org/docs/embed/c/host_function/#calling-frame-context
  // https://www.secondstate.io/articles/extend-webassembly/
  WasmEdge_MemoryInstanceContext *MemCxt = WasmEdge_CallingFrameGetMemoryInstance(CallFrameCxt, 0);
  // read data
  WasmEdge_Result Res = WasmEdge_MemoryInstanceGetData(MemCxt, Url, SourcePointer, SourceSize);
  if (WasmEdge_ResultOK(Res)) {
    // printf("u32 at memory[%u]: %s\n", SourcePointer, Url);

    if (SourceSize % 2 == 0) {
      const char even[] = "even";
      const size_t len = strlen(even);
      WasmEdge_MemoryInstanceSetData(MemCxt, (unsigned char *)even, TargetPointer, len);
      Out[0] = WasmEdge_ValueGenI32(len);
    } else {
      const char odd[] = "odd";
       const size_t len = strlen(odd);
      WasmEdge_MemoryInstanceSetData(MemCxt, (unsigned char *)odd, TargetPointer, len);
      Out[0] = WasmEdge_ValueGenI32(len);
    }

    return WasmEdge_Result_Success;
  } else {
    return Res;
  }
}

WasmEdge_ModuleInstanceContext *CreateExternModule() {
  // 1. Module Instance Creation
  WasmEdge_String HostModuleName = WasmEdge_StringCreateByCString("native_functions");
  WasmEdge_ModuleInstanceContext *HostModCxt = WasmEdge_ModuleInstanceCreate(HostModuleName);


  // 2. Create a function type and function context - "add"
  enum WasmEdge_ValType ParamList[2] = {WasmEdge_ValType_I32, WasmEdge_ValType_I32};
  enum WasmEdge_ValType ReturnList[1] = {WasmEdge_ValType_I32};
  /* Create a function type: {i32, i32} -> {i32, i32}. */
  WasmEdge_FunctionTypeContext *HostFTypeAdd = WasmEdge_FunctionTypeCreate(ParamList, 2, ReturnList, 1);
  WasmEdge_FunctionInstanceContext *HostFuncCtxAdd = WasmEdge_FunctionInstanceCreate(HostFTypeAdd, Add, NULL, 0);
  //
  // Add the host function created above with the export name "add"
  WasmEdge_String HostFuncName = WasmEdge_StringCreateByCString("add");
  WasmEdge_ModuleInstanceAddFunction(HostModCxt, HostFuncName, HostFuncCtxAdd);
  WasmEdge_StringDelete(HostFuncName);
  WasmEdge_FunctionTypeDelete(HostFTypeAdd);


  // 3. Create a function type and function context - "string_length"
  enum WasmEdge_ValType ParamListStringLength[2] = {WasmEdge_ValType_I32,
                                        WasmEdge_ValType_I32};
  enum WasmEdge_ValType ReturnListStringLength[1] = {WasmEdge_ValType_I32};
  /* Create a function type: {i32, i32} -> {i32}. */
  WasmEdge_FunctionTypeContext *HostFTypeStringLength = WasmEdge_FunctionTypeCreate(ParamListStringLength, 2, ReturnListStringLength, 1);
  WasmEdge_FunctionInstanceContext *HostFuncCtxStringLength = WasmEdge_FunctionInstanceCreate(HostFTypeStringLength, StringLength, NULL, 0);

  HostFuncName = WasmEdge_StringCreateByCString("string_length");
  WasmEdge_ModuleInstanceAddFunction(HostModCxt, HostFuncName, HostFuncCtxStringLength);
  WasmEdge_StringDelete(HostFuncName);
  WasmEdge_FunctionTypeDelete(HostFTypeStringLength);


  // 4. Create a function type and function context - "string_odd_or_even"
  enum WasmEdge_ValType ParamListStringOddOrEven[3] = {WasmEdge_ValType_I32,WasmEdge_ValType_I32, WasmEdge_ValType_I32};
  enum WasmEdge_ValType ReturnListStringOddOrEven[1] = {WasmEdge_ValType_I32};
  /* Create a function type: {i32, i32, i32} -> {i32}. */
  WasmEdge_FunctionTypeContext *HostFTypeStringOddOrEven = WasmEdge_FunctionTypeCreate(ParamListStringOddOrEven, 3, ReturnListStringOddOrEven, 1);
  WasmEdge_FunctionInstanceContext *HostFuncCtxStringOddOrEven = WasmEdge_FunctionInstanceCreate(HostFTypeStringOddOrEven, StringOddOrEven, NULL, 0);

  HostFuncName = WasmEdge_StringCreateByCString("string_odd_or_even");
  WasmEdge_ModuleInstanceAddFunction(HostModCxt, HostFuncName, HostFuncCtxStringOddOrEven);
  WasmEdge_StringDelete(HostFuncName);
  WasmEdge_FunctionTypeDelete(HostFTypeStringOddOrEven);


  // 5. cleanup
  WasmEdge_StringDelete(HostModuleName);

  return HostModCxt;
}

int run(char *wasmFile, char *wasmFunction, int argc, char **argv, int preopenLen, const char *const *preopens) {
  /* Create the configure context and add the WASI support. */
  /* This step is not necessary unless you need WASI support. */
  WasmEdge_ConfigureContext *ConfCxt = WasmEdge_ConfigureCreate();
  WasmEdge_ConfigureAddProposal(ConfCxt, WasmEdge_Proposal_MultiValue);
  WasmEdge_ConfigureAddHostRegistration(ConfCxt, WasmEdge_HostRegistration_Wasi);

  /* The configure and store context to the VM creation can be NULL. */
  WasmEdge_StoreContext *StoreCxt = WasmEdge_StoreCreate();
  WasmEdge_VMContext *VMCxt = WasmEdge_VMCreate(ConfCxt, StoreCxt);

  WasmEdge_ExecutorContext *ExecCxt = WasmEdge_ExecutorCreate(NULL, NULL);

  // add args/envs/preopens
  WasmEdge_ModuleInstanceContext *WasiCxt = WasmEdge_VMGetImportModuleContext(VMCxt, WasmEdge_HostRegistration_Wasi);
  WasmEdge_ModuleInstanceInitWASI(WasiCxt, argv, argc, NULL, 0, preopens, preopenLen);


  WasmEdge_Result Res;

  // Add host module
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