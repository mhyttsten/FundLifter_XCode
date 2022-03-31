# Content created from these source
#    https://llvm.org/docs/CMake.html
#    https://mlir.llvm.org/getting_started

# OBS
#   1. Now it seems to be working, but previously there was a problem working with
#      stand-alone command-line tools installed. They had to be completely removed from system.
#   2. Not sure if this is correct now, but before -DLLVM_ENABLE_RTTI=ON had to be specified
#      (and resulting llvm-config --has-rtti needs to return YES). If not, you would get
#      undefined symbol at link time on SymbolResolver)
#
# All available targets
# CMake options available here: https://llvm.org/docs/CMake.html
# CMake Flags: -G <generator> -S <source_dir> -B <build_dir>
# Errors in LLVM_ENABLE_PROJECTS/_RUNTIMES:
#    libc: No build file for darwin
#    openmp: Removed from runtimes as it's already part of projects
# First do this
#    brew install cmake
#    brew install ninja
#    git clone https://github.com/llvm/llvm-project.git
#    git branch xyz
#    See flags here: https://clang.llvm.org/docs/CommandGuide/clang.html
export CC=/usr/bin/clang  # Not sure if needed
export CXX=/usr/bin/clang++  # Not sure if needed
#export LDFLAGS="-L/usr/local/opt/llvm/lib"
#export CPPFLAGS="-I/usr/local/opt/llvm/include"
export CPLUS_INCLUDE_PATH="/Library/Developer/CommandLineTools/SDKs/MacOSX12.1.sdk/usr/include/c++/v1"
mkdir -p llvm-project/build
mkdir -p llvm-install
cd llvm-project
cmake -G Ninja \
      -S llvm \
      -B build \
      -DLLVM_ENABLE_PROJECTS='clang;clang-tools-extra;cross-project-tests;flang;libclc;lld;lldb;mlir;openmp;polly;pstl' \
      -DLLVM_ENABLE_RUNTIMES='libcxx;libcxxabi;libunwind;compiler-rt;libc' \
      -DCMAKE_INSTALL_PREFIX=/Users/magnushyttsten/products/llvm/llvm-install \
      -DCMAKE_BUILD_TYPE=Debug \
      -DLLVM_ENABLE_RTTI=ON \
      -DLLVM_BUILD_EXAMPLES=ON \
      -DLLVM_TARGETS_TO_BUILD="X86" \
      -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
      -DLLVM_ENABLE_ASSERTIONS=On
cmake --build build --target install

