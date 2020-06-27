SHPATH=/Users/magnushyttsten/xcode/FundLifter/LLVM

$SHPATH/llvm_mlir_tblgen.sh mlir/ToyCombine.td ToyCombine -gen-rewriters
$SHPATH/llvm_mlir_tblgen.sh include/toy/Ops.td toy/Ops -gen-op-decls
$SHPATH/llvm_mlir_tblgen.sh include/toy/Ops.td toy/Ops -gen-op-defs
$SHPATH/llvm_mlir_tblgen.sh include/toy/ShapeInferenceInterface.td toy/ShapeInferenceOpInterfaces -gen-op-interface-decls
$SHPATH/llvm_mlir_tblgen.sh include/toy/ShapeInferenceInterface.td toy/ShapeInferenceOpInterfaces -gen-op-interface-defs
$SHPATH/llvm_mlir_compile.sh parser/AST.cpp
$SHPATH/llvm_mlir_compile.sh mlir/DeadFunctionEliminationPass.cpp
$SHPATH/llvm_mlir_compile.sh mlir/ToyCombine.cpp
$SHPATH/llvm_mlir_compile.sh mlir/MLIRGen.cpp
$SHPATH/llvm_mlir_compile.sh mlir/ShapeInferencePass.cpp
$SHPATH/llvm_mlir_compile.sh mlir/LowerToLLVM.cpp
$SHPATH/llvm_mlir_compile.sh mlir/LowerToAffineLoops.cpp
$SHPATH/llvm_mlir_compile.sh mlir/Dialect.cpp
$SHPATH/llvm_mlir_compile.sh toyc.cpp

CMD="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ \
   -fPIC -fvisibility-inlines-hidden -Werror=date-time -Werror=unguarded-availability-new \
   -Wall -Wextra -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wmissing-field-initializers \
   -pedantic -Wno-long-long -Wimplicit-fallthrough -Wcovered-switch-default -Wno-noexcept-type \
   -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -Wstring-conversion -fdiagnostics-color -O3 \
   -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
   -Wl,-search_paths_first -Wl,-headerpad_max_install_names \
   -Wl,-force_load /usr/local/lib/libMLIRAffineToStandard.a \
   -Wl,-force_load /usr/local/lib/libMLIRAffineOps.a \
   -Wl,-force_load /usr/local/lib/libMLIRLLVMIR.a \
   -Wl,-force_load /usr/local/lib/libMLIRStandardOps.a \
   /tmp/build_tmp/toyc.cpp.o \
   /tmp/build_tmp/AST.cpp.o \
   /tmp/build_tmp/MLIRGen.cpp.o \
   /tmp/build_tmp/Dialect.cpp.o \
   /tmp/build_tmp/DeadFunctionEliminationPass.cpp.o \
   /tmp/build_tmp/LowerToAffineLoops.cpp.o \
   /tmp/build_tmp/LowerToLLVM.cpp.o \
   /tmp/build_tmp/ShapeInferencePass.cpp.o \
   /tmp/build_tmp/ToyCombine.cpp.o \
   -o /tmp/build_tmp/toyc-ch7 \
   -Wl,-rpath,@loader_path/../lib \
   -L/usr/local/lib \
   -lLLVMCore \
   -lLLVMSupport \
   -lMLIRAffineOps \
   -lMLIRAffineToStandard \
   -lMLIRAnalysis \
   -lMLIRExecutionEngine \
   -lMLIRIR \
   -lMLIRLLVMIR \
   -lMLIRLoopToStandard \
   -lMLIRParser \
   -lMLIRPass \
   -lMLIRStandardOps \
   -lMLIRStandardToLLVM \
   -lMLIRTargetLLVMIR \
   -lMLIRTransforms \
   -lMLIRTargetLLVMIRModuleTranslation \
   -lMLIRTranslation \
   -lLLVMOrcJIT \
   -lLLVMExecutionEngine \
   -lLLVMRuntimeDyld \
   -lLLVMJITLink \
   -lLLVMOrcError \
   -lLLVMPasses \
   -lLLVMX86CodeGen \
   -lLLVMAsmPrinter \
   -lLLVMDebugInfoDWARF \
   -lLLVMCFGuard \
   -lLLVMGlobalISel \
   -lLLVMSelectionDAG \
   -lLLVMCodeGen \
   -lLLVMTarget \
   -lLLVMX86Desc \
   -lLLVMX86Utils \
   -lLLVMMCDisassembler \
   -lLLVMX86Info \
   -lLLVMipo \
   -lLLVMBitWriter \
   -lLLVMIRReader \
   -lLLVMInstrumentation \
   -lLLVMLinker \
   -lLLVMScalarOpts \
   -lLLVMAggressiveInstCombine \
   -lLLVMInstCombine \
   -lLLVMVectorize \
   -lLLVMTransformUtils \
   -lLLVMAnalysis \
   -lLLVMObject \
   -lLLVMBitReader \
   -lLLVMMCParser \
   -lLLVMMC \
   -lLLVMDebugInfoCodeView \
   -lLLVMDebugInfoMSF \
   -lLLVMTextAPI \
   -lLLVMProfileData \
   -lMLIRLLVMIR \
   -lLLVMAsmParser \
   -lMLIRLoopToStandard \
   -lLLVMCore \
   -lLLVMBinaryFormat \
   -lLLVMRemarks \
   -lLLVMBitstreamReader \
   -lMLIRTransforms \
   -lMLIRTransformUtils \
   -lMLIRPass \
   -lMLIRAnalysis \
   -lMLIRAffineOps \
   -lMLIRVectorOps \
   -lMLIRStandardOps \
   -lMLIRIR \
   -lMLIRSupport \
   -lMLIRLoopOps \
   -lLLVMSupport \
   -lz \
   -lcurses \
   -lm \
   -lLLVMDemangle"
echo ""
echo $CMD
$CMD


