#! /bin/sh

# Content created from these source
#    https://llvm.org/docs/CMake.html
#    https://mlir.llvm.org/getting_started
# Not sure if this is correct any more, but:
#   1. Previously, there has been a problem working with
#      stand-alone command-line tools installed. They had to be completely removed from system.
#   2. Previously, -DLLVM_ENABLE_RTTI=ON had to be specified
#      (and resulting llvm-config --has-rtti needs to return YES). If not, you would get
#      undefined symbol at link time on SymbolResolver)
#   3. When building all packages
#         -DLLVM_ENABLE_PROJECTS='clang;clang-tools-extra;cross-project-tests;flang;libclc;lld;lldb;mlir;openmp;polly;pstl' \
#         -DLLVM_ENABLE_RUNTIMES='libcxx;libcxxabi;libunwind;compiler-rt;libc' \
#      You can get errors (so then remove them)
#         libc: No build file for darwin
#         openmp: Removed from runtimes as it's already part of projects
#      Also, if you get error couldn't include cstdio/cstlib include file you need to set flag
#         export CPLUS_INCLUDE_PATH="/Library/Developer/CommandLineTools/SDKs/MacOSX12.1.sdk/usr/include/c++/v1"
#         But I belive it should be set after configuration, but before build, otherwise config breaks down
#   3. These could be good to be aware of, but they don't seem to be needed
#      CMake flags: https://llvm.org/docs/CMake.html
#      CLang flags: https://clang.llvm.org/docs/CommandGuide/clang.html
#      CMake flags: -G <generator> -S <source_dir> -B <build_dir>
#      export CC=/usr/bin/clang  # Not sure if needed
#      export CXX=/usr/bin/clang++  # Not sure if needed
#      export LDFLAGS="-L/usr/local/opt/llvm/lib"
#      export CPPFLAGS="-I/usr/local/opt/llvm/include"
#
# MAIN BUILD SCRIPT FOR LLVM/MLIR, ASSUMING $HOME/products/llvm-base exists
# Optionally do this
#   brew install cmake
#   brew install ninja
#   brew update
#   brew upgrade  # Will upgrade all your bottles, install cmake, ninja is enough
#   Manually update the links
cd $HOME/products/llvm-base
git clone https://github.com/llvm/llvm-project.git
mv llvm-project llvm-project-$(date +"%y%m%d")
rm -f llvm-current
ln -s llvm-project-$(date +"%y%m%d") llvm-current
cd llvm-current
# git branch xyz
mkdir build
mkdir install
export MLIR_BIN=$HOME/products/llvm-base/llvm-current/install/bin:$HOME/products/llvm-base/llvm-current/install/examples
export MLIR_SRC=$HOME/products/llvm-base/llvm-current/mlir
export PATH=$MLIR_BIN:$PATH

cmake -G Ninja \
      -S llvm \
      -B build \
      -DLLVM_ENABLE_PROJECTS='mlir' \
      -DLLVM_ENABLE_RUNTIMES='' \
      -DCMAKE_INSTALL_PREFIX=$HOME/products/llvm-base/llvm-current/install \
      -DCMAKE_BUILD_TYPE=Debug \
      -DLLVM_ENABLE_RTTI=ON \
      -DLLVM_BUILD_EXAMPLES=ON \
      -DLLVM_TARGETS_TO_BUILD="X86" \
      -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
      -DLLVM_ENABLE_ASSERTIONS=On
cmake --build build --target install

