#include "../include/KaleidoscopeJIT.h"
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
#include "llvm/Target/TargetMachine.h"
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
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace llvm;
using namespace llvm::orc;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value *> NamedValues;
static std::unique_ptr<legacy::FunctionPassManager> TheFPM;
static std::unique_ptr<KaleidoscopeJIT> TheJIT;

// Create this function that we can call from our machine generated code
extern "C" double putchard(double X) {
  printf("%c\n", (char)X);
//  fputc((char)X, stdout);
  return 0.0;
}

int main() {
  printf("Starting\n");
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();
  
  // Initialize structures
  //    Code gets geneated into 'TheModule'
  //    TheJIT enables code to run within same process space
  TheJIT = std::make_unique<KaleidoscopeJIT>();
  TheModule = std::make_unique<Module>("my cool jit", TheContext);
  TheModule->setDataLayout(TheJIT->getTargetMachine().createDataLayout());
  
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
  const std::string FNfoo = "__foo";
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
  
  // *** Chapter 06
  // User-defied operators
  // Why would you allow this in a language?
  //    For a language designer, it allows you to define many parts of the language in the library
  //    E.g. if we have <= and !
  //       Then we can create user-define operator for '>'  as '!<='

  // *** Chapter 07
  // Mutable variables
  
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
  
  // *** Chapter 08
  // Object Files
  

  
  
  // Return result
  Builder.CreateRet(RetVal);
  verifyFunction(*Ffoo);  // Validates IR code
  //TheFPM->run(*Ffoo);  // Optional, optimizes according to TheFPM configuration
  
  // Print generated LLVM IR code
  printf("\nCode Generated for __bar");
  Fbar->print(errs());
  printf("\nCode Generated for __foo");
  Ffoo->print(errs());
  
  // JIT and execute generated code, then remove it from our process space
  auto H = TheJIT->addModule(std::move(TheModule));  // Code cannot be added to TheModule after this
  auto ExprSymbol = TheJIT->findSymbol(FNfoo);
  assert(ExprSymbol && "Function not found");
  double (*FP)(double) = (double (*)(double))(intptr_t)cantFail(ExprSymbol.getAddress());
  printf("Executed __foo(3.14), result: %f\n", FP(3.14));
  printf("Executed __foo(0.99), result: %f\n", FP(0.99));
  printf("Executed __foo(1.00), result: %f\n", FP(1.00));
  printf("Executed __foo(1.01), result: %f\n", FP(1.01));
  TheJIT->removeModule(H);  // Delete function from JIT space
  
  return 0;
}

// Instructions
//    Builder.CreateCondBr(ForIterEndCond, LoopBB, AfterBB);
//       br i1 %Condition, label %cond_true, label %cond_false
//    Builder.CreateBr(MergeBB);
//       br label %cond_next

/*
 LLVM does require all register values to be in SSA form, it does not require (or permit) memory objects to be in SSA form
 Some compilers try to version memory objects (SSA)
    LLVM does not do this in IR, but rather in Analysis Phases: https://llvm.org/docs/WritingAnLLVMPass.html
    Global variables:
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
 Before we used stack variabels: static std::map<std::string, Value*> NamedValues;
 Whith stack variables:          static std::map<std::string, AllocaInst*> NamedValues;
 

 TODO: LLVM debugging - https://llvm.org/docs/SourceLevelDebugging.html
 
 
 */


// Visualize control flow graph, options:
//   1. llvm-as < t.ll | opt -analyze -view-cfg  # LLVM IR in t.ll
//   2. F->viewCFG()” or “F->viewCFGOnly()  # Add to code or call in debugger


