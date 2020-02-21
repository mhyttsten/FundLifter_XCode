#include "../include/KaleidoscopeJIT.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
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
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;
using namespace llvm::orc;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value *> NamedValues;
static std::unique_ptr<legacy::FunctionPassManager> TheFPM;
static std::unique_ptr<KaleidoscopeJIT> TheJIT;

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
  TheFPM->add(createInstructionCombiningPass());  // Peephole & bit_twiddling optimizations
  TheFPM->add(createReassociatePass());  // Reassociate expressions
  TheFPM->add(createGVNPass());  // Eliminate common subexpressions
  TheFPM->add(createCFGSimplificationPass());  // Opt. control-flow (delete dead code, etc)
  TheFPM->doInitialization();

  const int argCount = 1;
  const std::string functionName = "__foo";
  std::vector<Type *> Doubles(argCount, Type::getDoubleTy(TheContext));
  FunctionType *FT_1DParam_1DReturn = FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);
  Function *F = Function::Create(FT_1DParam_1DReturn, Function::ExternalLinkage, functionName, TheModule.get());
  BasicBlock *BB = BasicBlock::Create(TheContext, "entry", F);
  Builder.SetInsertPoint(BB);

  Value *parameter0 = F->args().begin();  // Get 1st parameter
  Value *piConstant = ConstantFP::get(TheContext, APFloat(3.14));
  Value *VAddtmp0 = Builder.CreateFAdd(parameter0, piConstant, "addtmp");

  Function *Fsin = Function::Create(FT_1DParam_1DReturn, Function::ExternalLinkage, "sin", TheModule.get());
  std::vector<Value *> ArgsV;
  ArgsV.push_back(ConstantFP::get(TheContext, APFloat(1.57)));
  Value *Vcallsin = Builder.CreateCall(Fsin, ArgsV, "callsin");
  
  Value *RetVal = Builder.CreateFAdd(VAddtmp0, Vcallsin, "addtmp");

  Builder.CreateRet(RetVal);
  verifyFunction(*F);  // Validates IR code
  TheFPM->run(*F);  // Optional, optimizes according to TheFPM configuration
  printf("Here is the generated IR code\n");
  F->print(errs());

  auto H = TheJIT->addModule(std::move(TheModule));  // Code cannot be added to TheModule after this
  auto ExprSymbol = TheJIT->findSymbol(functionName);
  assert(ExprSymbol && "Function not found");
  double (*FP)(double) = (double (*)(double))(intptr_t)cantFail(ExprSymbol.getAddress());
  fprintf(stderr, "Evaluated to %f\n", FP(3.14));
  TheJIT->removeModule(H);  // Delete function from JIT space

  

  
  return 0;
}
