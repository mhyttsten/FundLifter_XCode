// *** JIT ***: ONLY NEEDED IF YOU WANT JIT EXECUTION
//===----------------------------------------------------------------------===//
// Copy of file: Kaleidoscope/include/KaleidoscopeJIT.h
// Contains a simple JIT definition for use in the kaleidoscope tutorials.
// No need to understand class KaleidoscopeJIT to understand LLVM basics below
//===----------------------------------------------------------------------===//
#ifndef LLVM_EXECUTIONENGINE_ORC_KALEIDOSCOPEJIT_H
#define LLVM_EXECUTIONENGINE_ORC_KALEIDOSCOPEJIT_H
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>
namespace llvm {
namespace orc {
class KaleidoscopeJIT {
public:
  using ObjLayerT = LegacyRTDyldObjectLinkingLayer;
  using CompileLayerT = LegacyIRCompileLayer<ObjLayerT, SimpleCompiler>;

  KaleidoscopeJIT()
      : Resolver(createLegacyLookupResolver(
            ES,
            [this](const std::string &Name) { return findMangledSymbol(Name); },
            [](Error Err) { cantFail(std::move(Err), "lookupFlags failed"); })),
        TM(EngineBuilder().selectTarget()), DL(TM->createDataLayout()),
        ObjectLayer(AcknowledgeORCv1Deprecation, ES,
                    [this](VModuleKey) {
                      return ObjLayerT::Resources{
                          std::make_shared<SectionMemoryManager>(), Resolver};
                    }),
        CompileLayer(AcknowledgeORCv1Deprecation, ObjectLayer,
                     SimpleCompiler(*TM)) {
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
  }

  TargetMachine &getTargetMachine() { return *TM; }

  VModuleKey addModule(std::unique_ptr<Module> M) {
    auto K = ES.allocateVModule();
    cantFail(CompileLayer.addModule(K, std::move(M)));
    ModuleKeys.push_back(K);
    return K;
  }

  void removeModule(VModuleKey K) {
    ModuleKeys.erase(find(ModuleKeys, K));
    cantFail(CompileLayer.removeModule(K));
  }

  JITSymbol findSymbol(const std::string Name) {
    return findMangledSymbol(mangle(Name));
  }

private:
  std::string mangle(const std::string &Name) {
    std::string MangledName;
    {
      raw_string_ostream MangledNameStream(MangledName);
      Mangler::getNameWithPrefix(MangledNameStream, Name, DL);
    }
    return MangledName;
  }
  
  JITSymbol findMangledSymbol(const std::string &Name) {
#ifdef _WIN32
    // The symbol lookup of ObjectLinkingLayer uses the SymbolRef::SF_Exported
    // flag to decide whether a symbol will be visible or not, when we call
    // IRCompileLayer::findSymbolIn with ExportedSymbolsOnly set to true.
    //
    // But for Windows COFF objects, this flag is currently never set.
    // For a potential solution see: https://reviews.llvm.org/rL258665
    // For now, we allow non-exported symbols on Windows as a workaround.
    const bool ExportedSymbolsOnly = false;
#else
    const bool ExportedSymbolsOnly = true;
#endif
    // Search modules in reverse order: from last added to first added.
    // This is the opposite of the usual search order for dlsym, but makes more
    // sense in a REPL where we want to bind to the newest available definition.
    for (auto H : make_range(ModuleKeys.rbegin(), ModuleKeys.rend()))
      if (auto Sym = CompileLayer.findSymbolIn(H, Name, ExportedSymbolsOnly))
        return Sym;
    // If we can't find the symbol in the JIT, try looking in the host process.
    if (auto SymAddr = RTDyldMemoryManager::getSymbolAddressInProcess(Name))
      return JITSymbol(SymAddr, JITSymbolFlags::Exported);
#ifdef _WIN32
    // For Windows retry without "_" at beginning, as RTDyldMemoryManager uses
    // GetProcAddress and standard libraries like msvcrt.dll use names
    // with and without "_" (for example "_itoa" but "sin").
    if (Name.length() > 2 && Name[0] == '_')
      if (auto SymAddr =
              RTDyldMemoryManager::getSymbolAddressInProcess(Name.substr(1)))
        return JITSymbol(SymAddr, JITSymbolFlags::Exported);
#endif
    return nullptr;
  }

  ExecutionSession ES;
  std::shared_ptr<SymbolResolver> Resolver;
  std::unique_ptr<TargetMachine> TM;
  const DataLayout DL;
  ObjLayerT ObjectLayer;
  CompileLayerT CompileLayer;
  std::vector<VModuleKey> ModuleKeys;
};
} // end namespace orc
} // end namespace llvm
#endif // LLVM_EXECUTIONENGINE_ORC_KALEIDOSCOPEJIT_H

//===----------------------------------------------------------------------===//
// Starting the LLVM tutorial code
//===----------------------------------------------------------------------===//
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>

using namespace llvm;
using namespace llvm::orc;

// Create this function that we can call from our machine generated code
extern "C" double putchard(double X) {
  printf("%c\n", (char)X);
  return 0.0;
}

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
 // Code gets generated into TheModule
static std::unique_ptr<Module> TheModule = std::make_unique<Module>("My Cool Module", TheContext);
static std::unique_ptr<legacy::FunctionPassManager> TheFPM;

// For JIT generation
static std::unique_ptr<KaleidoscopeJIT> TheJIT;
// For Object file generation (see description below)
TargetMachine *TargetMachine = nullptr;

int main() {
  printf("Starting\n");
  
  // Initialize the optimizer (using TheFPM is optional)
  TheFPM = std::make_unique<legacy::FunctionPassManager>(TheModule.get());
  TheFPM->add(createPromoteMemoryToRegisterPass());  // mem2reg pass, see chapter 7 (generate stack variables, but let mem2reg promote to register ones)
  TheFPM->add(createInstructionCombiningPass());  // Peephole & bit_twiddling optimizations
  TheFPM->add(createReassociatePass());  // Reassociate expressions
  TheFPM->add(createGVNPass());  // Eliminate common subexpressions
  TheFPM->add(createCFGSimplificationPass());  // Opt. control-flow (delete dead code, etc)
  TheFPM->doInitialization();

  // -------- __bar function (takes no argument and returns 2.72)
  FunctionType *FT_0DParam_1DReturn = FunctionType::get(Type::getDoubleTy(TheContext), false);
  const std::string FNbar = "__bar";
  Function *Fbar = Function::Create(FT_0DParam_1DReturn, Function::ExternalLinkage, FNbar, TheModule.get());
  BasicBlock *BBbar = BasicBlock::Create(TheContext, "entry", Fbar);
  Builder.SetInsertPoint(BBbar);
  Value *eRetVal = ConstantFP::get(TheContext, APFloat(2.72));
  Builder.CreateRet(eRetVal);
  verifyFunction(*Fbar);  // Validates IR code
  TheFPM->run(*Fbar);  // Optional, optimizes according to TheFPM configuration
  
  // -------- __foo function
  // Define a function __foo
  std::vector<Type *> Doubles(1, Type::getDoubleTy(TheContext));  // 1 == 1 argument
  FunctionType *FT_1DParam_1DReturn = FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);
  const std::string FNfoo = "foo";
  Function *Ffoo = Function::Create(FT_1DParam_1DReturn, Function::ExternalLinkage, FNfoo, TheModule.get());
  BasicBlock *BBfoo = BasicBlock::Create(TheContext, "entry", Ffoo);
  Builder.SetInsertPoint(BBfoo);
  
  // Add parameter with 3.14
  Value *parameter0 = Ffoo->args().begin();  // Get 1st parameter
  Value *piConstant = ConstantFP::get(TheContext, APFloat(3.14));
  Value *VAddtmp0 = Builder.CreateFAdd(parameter0, piConstant, "addtmp");

  // Call 'sin()' from libc with 1.56 as argument (pi/2)
  Function *Fsin = Function::Create(FT_1DParam_1DReturn, Function::ExternalLinkage, "sin", TheModule.get());
  std::vector<Value *> ArgsVsin;
  ArgsVsin.push_back(ConstantFP::get(TheContext, APFloat(1.57)));
  Value *Vcallsin = Builder.CreateCall(Fsin, ArgsVsin, "callsin");

  // Add result with previous result
  Value *RetVal = Builder.CreateFAdd(VAddtmp0, Vcallsin, "addtmp");
  
  // Call __bar
  std::vector<Value *> ArgsVbar;
  Value *Vcallbar = Builder.CreateCall(Fbar, ArgsVbar, "bar");

  // Add result with previous result
  RetVal = Builder.CreateFAdd(Vcallbar, RetVal, "addtmp");

  // *** Chapter 05
  // Visualize control flow graph, options:
  //   1. llvm-as < t.ll | opt -analyze -view-cfg  # LLVM IR in t.ll
  //   2. F->viewCFG()” or “F->viewCFGOnly()  # Add to code or call in debugger

  // **** if-statement
  // Let's do an if statement: if param0 > 1.0 then RetVal += 1 else RetVal += 2
  BasicBlock *ThenBB =  BasicBlock::Create(TheContext, "then", Ffoo);  // Attach now to Function
  BasicBlock *ElseBB =  BasicBlock::Create(TheContext, "else");    // Don't attach to Function yet
  BasicBlock *MergeBB = BasicBlock::Create(TheContext, "ifcont");  // Don't attach to Function yet
  Value *CondV = Builder.CreateFCmpOGT(parameter0, ConstantFP::get(TheContext, APFloat(1.0)), "ifcond");
  Builder.CreateCondBr(CondV, ThenBB, ElseBB);  // Go to ThenBB if condition > 1 else goto ElseBB
  
  // Manage the 'then' block
  Builder.SetInsertPoint(ThenBB);  // Code added from now on goes into this block
  Value *TmpV = ConstantFP::get(TheContext, APFloat(1.0));  // +1.0 if param0 > 1.0
  Value *ThenV = Builder.CreateFAdd(RetVal, TmpV, "addthen");
  // Observe, below uncondition branch - all BasicBlock must terminate with return or branch
  // Implicit fallthroughs are not permitted
  Builder.CreateBr(MergeBB);  // Unconditionally move to the Merge block
  // Below: Needed to get correct phi-value in MergeBB
  // Imagine if our ThenBB has nested if-then-elses, the it would change notion of 'current' block
  ThenBB = Builder.GetInsertBlock();
  
  // Else block
  Ffoo->getBasicBlockList().push_back(ElseBB);  // Attach to Function object (since we did not @ construction)
  Builder.SetInsertPoint(ElseBB);  // Code added from now on goes into this block
  Value *TmpV2 = ConstantFP::get(TheContext, APFloat(2.0));  // +2.0 if else (param0 !> 1.0)
  Value *ElseV = Builder.CreateFAdd(RetVal, TmpV2, "addelse");
  Builder.CreateBr(MergeBB);  // Unconditionally move to the Merge block
  ElseBB = Builder.GetInsertBlock();  // As above
  
  // Merge block
  Ffoo->getBasicBlockList().push_back(MergeBB);  // Attach to Function object (since we did not @ construction)
  Builder.SetInsertPoint(MergeBB);  // Code added from now on goes into this block
  // Add PHINode: PN == ThenV if we came from ThenBB, otherwise PN == ElseV
  // Works since all blocks must ends with a final SSA (what if no SSA in block at all == error?)
  PHINode *PN = Builder.CreatePHI(Type::getDoubleTy(TheContext), 2, "iftmp");  // PHINode is Value
  PN->addIncoming(ThenV, ThenBB);
  PN->addIncoming(ElseV, ElseBB);
  RetVal = PN;
  // **** end of if-statement

  // **** for-statement
  Value *ForStartV = ConstantFP::get(TheContext, APFloat(0.0));  // Initial value of loop variable
  BasicBlock *PreheaderBB = Builder.GetInsertBlock();
  BasicBlock *LoopBB = BasicBlock::Create(TheContext, "loop", Ffoo);
  Builder.CreateBr(LoopBB);
  // Start label for 'for' loop
  Builder.SetInsertPoint(LoopBB);
  // Create phi variable of type double, coming from 2 incoming variables
  PHINode *PhiV = Builder.CreatePHI(Type::getDoubleTy(TheContext), 2, "phi_variable");
  PhiV->addIncoming(ForStartV, PreheaderBB);  // ERROR: Always 1?
  // Call putchard, print 49 == 0x31 == '1'
  Function *Fputchard = Function::Create(FT_1DParam_1DReturn, Function::ExternalLinkage, "putchard", TheModule.get());
  std::vector<Value *> ArgsVputchard;
  ArgsVputchard.push_back(ConstantFP::get(TheContext, APFloat(49.0)));
  Value *Vcallputchard = Builder.CreateCall(Fputchard, ArgsVputchard, "callputchard_9.99");
  // Increment loop index variable by 1.0
  Value *NextVar = Builder.CreateFAdd(PhiV, ConstantFP::get(TheContext, APFloat(1.0)), "nextvar");
  // Compare if < 4.0? Compare 'Unordered' (can be QNAN) 'Less Than'. Result is bool == i1 (integer 1-bit)
  Value *ForIterAgainV = Builder.CreateFCmpULT(NextVar, ConstantFP::get(TheContext, APFloat(4.0)), "cmptmp");
  // Convert bool (i1) to double 0.0 or 1.0: 'Unsigned Integer To Floating Point'
  Value *Bool2Double = Builder.CreateUIToFP(ForIterAgainV, Type::getDoubleTy(TheContext), "booltmp");
  // Compare 'Ordered" (cannot be QNAN) 'Not Equal'
  Value *ForIterEndCond = Builder.CreateFCmpONE(Bool2Double, ConstantFP::get(TheContext, APFloat(0.0)), "loopcond");
  // LoopEnd is at the end of the loop (but we still might execute another round of loop)
  BasicBlock *LoopEndBB = Builder.GetInsertBlock();
  // Add entry to phi from LoopEnd with variable NextVr
  PhiV->addIncoming(NextVar, LoopEndBB);
  // Create 'after' label, at this point loop as terminated
  BasicBlock *AfterBB = BasicBlock::Create(TheContext, "afterloop", Ffoo);
  // Create conditional branch, we either iterate again or we're done
  Builder.CreateCondBr(ForIterEndCond, LoopBB, AfterBB);
  // Loop has terminated, after loop block starts here
  Builder.SetInsertPoint(AfterBB);
  // **** end of for-statement
  
  // *** Chapter 06 - User-defied operators
  // Why would you allow this in a language?
  //    For a language designer, it allows you to define many parts of the language in the library
  //    E.g. if we have <= and !
  //       Then we can create user-define operator for '>'  as '!<='

  // *** Chapter 07 - Mutable variables
  // Create IRBuilder object pointing at first instruction of the entry block
  //    ('entry' label block is first block always inserted implicitly into functions)
  IRBuilder<> TmpB(&Ffoo->getEntryBlock(), Ffoo->getEntryBlock().begin());
  // Allocate stack variable with name: "__av"
  const std::string VarName = "__av";
  AllocaInst* Alloca = TmpB.CreateAlloca(Type::getDoubleTy(TheContext), 0, VarName.c_str());
  // Function arguments should also use stack variables: for (auto &Arg : TheFunction->args()) + alloc as per above
  //    Before we used: Value *parameter0 = Ffoo->args().begin();  to get 1st parameter
  // Store value 53.0 into stack variable
  Builder.CreateStore(ConstantFP::get(TheContext, APFloat(53.0)), Alloca);
  // Load stack variable
  Value *Vloada = Builder.CreateLoad(Alloca, VarName.c_str());
  // Print '5'
  ArgsVputchard.clear();
  ArgsVputchard.push_back(Vloada);
  Vcallputchard = Builder.CreateCall(Fputchard, ArgsVputchard, "callputchard_9.99");

  // Return result
  Builder.CreateRet(RetVal);
  verifyFunction(*Ffoo);  // Validates IR code
  //TheFPM->run(*Ffoo);  // Optional, optimizes according to TheFPM configuration
  
  // Print generated LLVM IR code
  printf("\nCode Generated for __bar");
  Fbar->print(errs());
  printf("\nCode Generated for foo");
  Ffoo->print(errs());
  
  //*******************
  // Done with code generation, now let's executing it on JIT or create Object file
  
  // If true, generate & execute machine code directly into our process space (chapter 4)
  // If false, we write an object file (chapter 8)
  bool isJIT = true;
//  bool isJIT = false;

  // Initialization for JIT execution (generate and execute from within our process space)
  if (isJIT) {
    // Native targets
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
    
    // TheJIT enables code to run within same process space
    TheJIT = std::make_unique<KaleidoscopeJIT>();
    TheModule->setDataLayout(TheJIT->getTargetMachine().createDataLayout());

    // JIT and execute generated code, then remove it from our process space
    auto H = TheJIT->addModule(std::move(TheModule));  // Code cannot be added to TheModule after this
    auto ExprSymbol = TheJIT->findSymbol(FNfoo);
    assert(ExprSymbol && "Function not found");
    double (*FP)(double) = (double (*)(double))(intptr_t)cantFail(ExprSymbol.getAddress());
    printf("Executed foo(3.14), result: %f\n", FP(3.14));
    printf("Executed foo(0.99), result: %f\n", FP(0.99));
    printf("Executed foo(1.00), result: %f\n", FP(1.00));
    printf("Executed foo(1.01), result: %f\n", FP(1.01));
    TheJIT->removeModule(H);  // Delete function from JIT space
  }
  
  // Initialization for Object file generation
  else {
    // Initialize all targets (not needed unless you want all of them))
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    // Target triple: <arch><sub>-<vendor>-<sys>-<abi>, see http://clang.llvm.org/docs/CrossCompilation.html#target-triple
    // See what clang thinks our target is: $ clang --version | grep Target
    auto TargetTriple = sys::getDefaultTargetTriple();
    std::string Error;
    // Holds target specific information
    // Generated from a target triple: <arch><sub>-<vendor>-<sys>-<abi>
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // TargetMachine = Machine description of the machine we’re targeting
    // Also specifies features (e.g. SSE) or a specific CPU (e.g. Sandylake)
    // To see which features and CPUs that LLVM knows about, e.g. for X86:
    //   $ llvm-as < /dev/null | llc -march=x86 -mattr=help
    // Just use boilerplate CPU without any features
    auto CPU = "generic";
    auto Features = "";
    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TheTargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    TheModule->setDataLayout(TheTargetMachine->createDataLayout());
    TheModule->setTargetTriple(TargetTriple);
    
    auto Filename = "/tmp/llvm_foo.o";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);
    if (EC) {
      errs() << "Could not open file: " << EC.message();
      return 1;
    }

    legacy::PassManager pass;
//    auto FileType = llvm::TargetMachine::CGFT_ObjectFile;
    auto FileType = CGFT_ObjectFile;
    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
      errs() << "TheTargetMachine can't emit a file of this type";
      return 1;
    }
    pass.run(*TheModule);
    dest.flush();
    outs() << "Wrote " << Filename << "\n";
    
    std::string main_src = " \
      // Compile and run: $ clang++ llvm_main.cpp llvm_foo.o -o llvm_main\n \
      #include <iostream>\n \
      extern \"C\" { double foo(double); }\n \
      int main() {\n \
        printf(\"Executed foo(3.14), result: %f\\n\", foo(3.14));\n \
        printf(\"Executed foo(0.99), result: %f\\n\", foo(0.99));\n \
        printf(\"Executed foo(1.00), result: %f\\n\", foo(1.00));\n \
        printf(\"Executed foo(1.01), result: %f\\n\", foo(1.01));\n \
      }\n \
      // Need to have this as our generated llvm_foo.o calls it\n \
      extern \"C\" double putchard(double X) {\n \
         printf(\"%c\\n\", (char)X);\n \
         return 0.0;\n \
      }\n \
    ";
    std::ofstream main_src_os("/tmp/llvm_main.cpp");
    main_src_os << main_src;
    main_src_os.close();
    outs() << "Wrote /tmp/llvm_main.cpp\n";
  }
  
  return 0;
}

// Instructions
//    Builder.CreateCondBr(ForIterEndCond, LoopBB, AfterBB);
//       br i1 %Condition, label %cond_true, label %cond_false
//    Builder.CreateBr(MergeBB);
//       br label %cond_next

/*
 From Chapter 7
 LLVM does require all register values to be in SSA form, it does not require (or permit) memory objects to be in SSA form
 Some compilers try to version memory objects (SSA)
    LLVM does not do this in IR, but rather in Analysis Phases: https://llvm.org/docs/WritingAnLLVMPass.html
    Global variables - Create using GlobalVariable class
       @G = weak global i32 0   ; type of @G is i32*
       @H = weak global i32 0   ; type of @H is i32*
    Stack variables:
       %X = alloca i32           ; type of %X is i32*.
       %tmp = load i32* %X       ; load the stack value %X from the stack.
       %tmp2 = add i32 %tmp, 1   ; increment it
       store i32 %tmp2, i32* %X  ; store it back

    Below could be implemented with phi variables, but also using a stack variable
    Using stack is a major performanc problem, but you don't need to thing about this
    Typically the 'mem2reg' optimization pass uses optimal solution (stack or phi) even if you use stack
       $ llvm-as < example.ll | opt -mem2reg | llvm-dis
       mem2reg only optimizes during some conditions (complex)
       Also mem2reg cannot promote structs or arrays to registers (sroa pass can though)
    You should rely on mem2reg, and not do SSA/phi explicitly, it's proven, fast, and needed for debug info generation
    int G, H;
    int test(_Bool Condition) {
       int X;
       if (Condition) X = G;
       else X = H;
       return X;
    }
 Managing symbol tables in LLVM: NamedValues map
    Before we used SSA values: static std::map<std::string, Value*> NamedValues;
    With stack variables:      static std::map<std::string, AllocaInst*> NamedValues;
 LLVM also supports Accurate GC: https://llvm.org/docs/GarbageCollection.html
 
 
 *** Chapter 9
 LLVM uses DWARF - A format used by compilers & debuggers to do source level debugging
 Debug information is hard when you do optimizations, so let's not do optimizations for now
 Details are here: http://llvm.org/docs/SourceLevelDebugging.html
 Similar to IRBuilder there is a DIBuilder class that constructs debug metadata for an LLVM IR file
 Chapter 9 updates code to emit source level debugging, won't do it here
 
 *** Chapter 10
 LLVM supports many other things:
 - Arrays, struct, vectors: https://llvm.org/docs/GetElementPtr.html, https://llvm.org/docs/LangRef.html#getelementptr-instruction
 - Zero cost exceptions: https://llvm.org/docs/ExceptionHandling.html
 Help: http://lists.llvm.org/mailman/listinfo/llvm-dev
 
 ********************************************************************
 LLVM Reference Guide
 
 IR aims to be the "universal IR"
 SSA - Static Single Assignment
 
 Three equivalent forms of LLVM code representation
 - In-memory compiler IR
 - On-disk bitcode representation (suitable for fast loading by a JIT)
 - Human readable assembly (this guide)
 
 Identifiers
 - Global e.g. functions, global variables, starts with @ character
 - Local  e.g. register names, type, starts with % character
 - 3 types: 1) named e.g: @hello.how.are.you, @.hello, 2) unnamed e.g: @2, %1, 3) constants:
 
 Comments: Start with ';' and go to EOL
 
 LLVM programs are composed of Modules (like source files): containing functions, global variables, symbol tables
 Modules are combined together with linker
 
 Linkage Types: How global functions & variables are treated across modules
   (private, internal, available_externally, linkonce, weak, common, appending, linkonce_odr/weak_odr, external)
 Callig Conventions: How a function is called - more may be added in future
   (ccc, fastcc, coldcc, cc 10, cc 11, webkit_jscc, anyregcc, preserve_mostcc, preserve_allcc, cxx_fast_tlscc,
    swiftcc, tailcc, cfguard_checkcc, cc <n>)
 Visibility styles for global variables/functions
   default: Visible to other modules (external linkage)
   hidden: Not visible to other modules
   protected: Hidden to defining module and cannot be overriden
 DLL Storage classes: dllimport, dllexport
 
 
 */



