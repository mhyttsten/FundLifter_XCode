/*
 *** Chapter 2: https://mlir.llvm.org/docs/Tutorials/Toy/Ch-2/
 LLVM: Fixed set of predefined types, and (low-level) instructions
    This is a big lowering, causing different front-ends to reimplement significant infrastructure
 MLIR is designed for flexibility, having few pre-defined instructions or types
    I supportes extensibility through Dialects, grouping concepts under a unique namespace
 
 Operations: Core unit of abstraction and computation (similar to LLVM instructions)
    Can represent all core IR structures of LLVM (instructions, globals, modules)
    %t_tensor = "toy.transpose"(%arg0)  // return dialect.name(args)
       {inplace = true}  // constant data available in operation
       : (tensor<2x3xf64>) -> tensor<3x2xf64>  // argument & return types
       loc("example/file/path":12:1)  // mandatory

 $ mlir-opt  // Tests compiler passes
    --help
    --print-debuginfo  // Includes source locations
 */




#include <stdio.h>

int main(int argc, char* argv[]) {
  printf("Hello from MLIR_MagnusSummary\n");
}

