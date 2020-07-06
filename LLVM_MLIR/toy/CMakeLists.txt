cmake_minimum_required(VERSION 3.17)
message(STATUS "Top, src: ${CMAKE_CURRENT_SOURCE_DIR}, bin: ${CMAKE_CURRENT_BINARY_DIR}")

message(STATUS "CMake version: ${CMAKE_MAJOR_VERSION} ${CMAKE_MINOR_VERSION}")
project(mlir_toy)
# set(CMAKE_VERBOSE_MAKEFILE on)  # Or -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON when creating build files

function(mlir_tablegen subproject_path option file_td file_out)
  # Callers CMAKE_CURRENT_[SOURCE|BINARY]_DIR paths are honored in here
  # message(STATUS "Entered mlir_tablegen, option: ${option}, src: ${CMAKE_CURRENT_SOURCE_DIR}, bin: ${CMAKE_CURRENT_BINARY_DIR}")

  add_custom_command(
    OUTPUT ${file_out}  # Then we let target depend on this output file
    COMMAND mlir-tblgen
      "${option}"
      -I/usr/local/include
      -I"${subproject_path}/include"
      "${CMAKE_CURRENT_SOURCE_DIR}/${file_td}"
      --write-if-changed
      -o "${file_out}"
      -d "${file_out}.d"
 )
endfunction()

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
# -fvisibility-inlines-hidden eliminates linker warning "ld: warning: direct access in function ... to global weak symbol"
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17 -fvisibility-inlines-hidden")
# set_target_properties(<target> PROPERTIES COMPILE_FLAGS <your flags>)
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

# Variable containing massive list of LLVM/MLIR link libraries to avoid undefined symbols
set(MLIR_ALLLIBS "LLVMCore;LLVMSupport;MLIRAffineOps;MLIRAffineToStandard;MLIRAnalysis;MLIRExecutionEngine;MLIRIR;MLIRLLVMIR;MLIRLoopToStandard;MLIRParser;MLIRPass;MLIRStandardOps;MLIRStandardToLLVM;MLIRTargetLLVMIR;MLIRTransforms;MLIRTargetLLVMIRModuleTranslation;MLIRTranslation;LLVMOrcJIT;LLVMExecutionEngine;LLVMRuntimeDyld;LLVMJITLink;LLVMOrcError;LLVMPasses;LLVMX86CodeGen;LLVMAsmPrinter;LLVMDebugInfoDWARF;LLVMCFGuard;LLVMGlobalISel;LLVMSelectionDAG;LLVMCodeGen;LLVMTarget;LLVMX86Desc;LLVMX86Utils;LLVMMCDisassembler;LLVMX86Info;LLVMipo;LLVMBitWriter;LLVMIRReader;LLVMInstrumentation;LLVMLinker;LLVMScalarOpts;LLVMAggressiveInstCombine;LLVMInstCombine;LLVMVectorize;LLVMTransformUtils;LLVMAnalysis;LLVMObject;LLVMBitReader;LLVMMCParser;LLVMMC;LLVMDebugInfoCodeView;LLVMDebugInfoMSF;LLVMTextAPI;LLVMProfileData;MLIRLLVMIR;LLVMAsmParser;MLIRLoopToStandard;LLVMCore;LLVMBinaryFormat;LLVMRemarks;LLVMBitstreamReader;MLIRTransforms;MLIRTransformUtils;MLIRPass;MLIRAnalysis;MLIRAffineOps;MLIRVectorOps;MLIRStandardOps;MLIRIR;MLIRSupport;MLIRLoopOps;LLVMSupport;z;curses;m;LLVMDemangle")

add_subdirectory(Ch1)
add_subdirectory(Ch2)
add_subdirectory(Ch3)
add_subdirectory(Ch4)
add_subdirectory(Ch5)
add_subdirectory(Ch6)
add_subdirectory(Ch7)
