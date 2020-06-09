# Content created from these source
#    https://llvm.org/docs/CMake.html
#    https://mlir.llvm.org/getting_started

# OBS: Building lldb target of LLVM does not work with stand-alone command-line tools installed
#    Only X-code installed command-line tools must be used

# All available targets
#   -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi;lldb;clang-tools-extra;libunwind;compiler-rt;lld;mlir;debuginfo-tests;polly" \
# Alternative platforms
#   -DLLVM_TARGETS_TO_BUILD="X86;NVPTX;AMDGPU" \

# brew install cmake
# brew install ninja
# rm -rf  /Library/Developer/CommandLineTools  # Remove stand-alone command-line tools
# sudo xcode-select -s /Applications/Xcode.app/Contents/Developer  # Point to Xcode command-line tools
# git clone https://github.com/llvm/llvm-project.git
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
tar xvf llvm-project-r10-fromgithub.tar  # If we already have a tar ball of GitHub LLVM
mkdir llvm-project/build
cd llvm-project/build
cmake -G Ninja ../llvm \
      -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi;lldb;clang-tools-extra;libunwind;compiler-rt;lld;mlir;debuginfo-tests;polly"
      -DLLVM_BUILD_EXAMPLES=ON \
      -DLLVM_TARGETS_TO_BUILD="X86" \
      -DCMAKE_BUILD_TYPE=Release \
      -DLLVM_ENABLE_ASSERTIONS=ON
cmake --build .
# Optional: Set install prefix at installation time, execute from build directory
#    cmake -DCMAKE_INSTALL_PREFIX=/tmp/llvm -P cmake_install.cmake
# cmake --build . --target install  # Installs onto your system, see below

#####################################################################
# System installation, done through: cmake --build . --target install
# Installation of LLVM 10.0.1
#
# Installation encounters many error like below (an .o file in a .a does not have any symbols)
#    /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib:
#       file: /usr/local/lib/libunwind.a(Unwind-EHABI.cpp.o) has no symbols
#
# -- Creating llvm-ranlib
# -- Creating llvm-lib
# -- Creating llvm-dlltool
# -- Creating clang++
# -- Creating clang-cl
# -- Creating clang-cpp
# -- Creating llvm-install-name-tool
# -- Creating llvm-strip
# -- Creating lld-link
# -- Creating ld.lld
# -- Creating ld64.lld
# -- Creating wasm-ld
# -- Creating llvm-readelf
# -- Creating llvm-addr2line
#
# /usr/local/bin/bugpoint
# /usr/local/bin/c-index-test
# /usr/local/bin/clang-10
# /usr/local/bin/clang
# /usr/local/bin/clang-apply-replacements
# /usr/local/bin/clang-change-namespace
# /usr/local/bin/clang-check
# /usr/local/bin/clang-doc
# /usr/local/bin/clang-extdef-mapping
# /usr/local/bin/clang-format
# /usr/local/bin/clang-import-test
# /usr/local/bin/clang-include-fixer
# /usr/local/bin/clang-move
# /usr/local/bin/clang-offload-bundler
# /usr/local/bin/clang-offload-wrapper
# /usr/local/bin/clang-query
# /usr/local/bin/clang-refactor
# /usr/local/bin/clang-rename
# /usr/local/bin/clang-scan-deps
# /usr/local/bin/clang-tidy
# /usr/local/bin/clang-reorder-fields
# /usr/local/bin/clangd
# /usr/local/bin/clangd-xpc-test-client
# /usr/local/bin/darwin-debug
# /usr/local/bin/debugserver
# /usr/local/bin/diagtool
# /usr/local/bin/dsymutil
# /usr/local/bin/find-all-symbols
# /usr/local/bin/git-clang-format
# /usr/local/bin/hmaptool
# /usr/local/bin/llc
# /usr/local/bin/lld
# /usr/local/bin/lldb-server
# /usr/local/bin/lldb-argdumper
# /usr/local/bin/lldb
# /usr/local/bin/lldb-instr
# /usr/local/bin/lldb-vscode
# /usr/local/bin/lli
# /usr/local/bin/llvm-ar
# /usr/local/bin/llvm-as
# /usr/local/bin/llvm-bcanalyzer
# /usr/local/bin/llvm-c-test
# /usr/local/bin/llvm-cat
# /usr/local/bin/llvm-cfi-verify
# /usr/local/bin/llvm-cov
# /usr/local/bin/llvm-config
# /usr/local/bin/llvm-cvtres
# /usr/local/bin/llvm-cxxdump
# /usr/local/bin/llvm-cxxfilt
# /usr/local/bin/llvm-cxxmap
# /usr/local/bin/llvm-lto
# /usr/local/bin/llvm-diff
# /usr/local/bin/llvm-dis
# /usr/local/bin/llvm-dwarfdump
# /usr/local/bin/llvm-dwp
# /usr/local/bin/llvm-elfabi
# /usr/local/bin/llvm-exegesis
# /usr/local/bin/llvm-extract
# /usr/local/bin/llvm-ifs
# /usr/local/bin/llvm-jitlink
# /usr/local/bin/llvm-link
# /usr/local/bin/llvm-lipo
# /usr/local/bin/llvm-lto2
# /usr/local/bin/llvm-mc
# /usr/local/bin/llvm-mca
# /usr/local/bin/llvm-modextract
# /usr/local/bin/llvm-mt
# /usr/local/bin/llvm-nm
# /usr/local/bin/llvm-objcopy
# /usr/local/bin/llvm-objdump
# /usr/local/bin/llvm-opt-report
# /usr/local/bin/llvm-pdbutil
# /usr/local/bin/llvm-profdata
# /usr/local/bin/llvm-rc
# /usr/local/bin/llvm-readobj
# /usr/local/bin/llvm-reduce
# /usr/local/bin/llvm-rtdyld
# /usr/local/bin/llvm-size
# /usr/local/bin/llvm-split
# /usr/local/bin/llvm-stress
# /usr/local/bin/llvm-strings
# /usr/local/bin/llvm-symbolizer
# /usr/local/bin/llvm-tblgen
# /usr/local/bin/llvm-undname
# /usr/local/bin/llvm-xray
# /usr/local/bin/modularize
# /usr/local/bin/mlir-cpu-runner
# /usr/local/bin/mlir-opt
# /usr/local/bin/mlir-tblgen
# /usr/local/bin/mlir-translate
# /usr/local/bin/obj2yaml
# /usr/local/bin/opt
# /usr/local/bin/pp-trace
# /usr/local/bin/sancov
# /usr/local/bin/sanstats
# /usr/local/bin/scan-build
# /usr/local/bin/set-xcode-analyzer
# /usr/local/bin/scan-view
# /usr/local/bin/verify-uselistorder
# /usr/local/bin/yaml2obj
#
# /usr/local/include/c++/v1/
# /usr/local/include/clang
# /usr/local/include/clang-c
# /usr/local/include/lld
# /usr/local/include/lldb
# /usr/local/include/llvm
# /usr/local/include/llvm-c
# /usr/local/include/mlir
# /usr/local/include/mlir/Dialect  # Super-interesting folder of course
# /usr/local/include/mlir-c
# /usr/local/include/polly
#
# /usr/local/lib/clang/10.0.1
# /usr/local/lib/cmake/clang
# /usr/local/lib/cmake/polly
# /usr/local/lib/cmake/llvm
# /usr/local/lib/libc++abi*.dylib
# /usr/local/lib/libc++*.a, *.dylib
# /usr/local/lib/libcblas*.dylb
# /usr/local/lib/libclang*.a, *.dylib
# /usr/local/lib/libClangdXPCLib.dylib
# /usr/local/lib/libfindAllSymbols.a
# /usr/local/lib/liblld*.a, *.dylib
# /usr/local/lib/liblldb.10.0.1.dylib
# /usr/local/lib/liblldb.dylib
# /usr/local/lib/libLLVM*.a
# /usr/local/lib/libLTO.dylib
# /usr/local/lib/libMLIR*.a  # Super-interesting libraries of course
# /usr/local/lib/libRemarks.dylib
# /usr/local/lib/libPolly*.a. *.so, *.dylib
# /usr/local/lib/LLVMPolly.so
# /usr/local/lib/python3.6/site-packages/lldb
# /usr/local/lib/python3.6/site-packages/six.py
# /usr/local/lib/libunwind*.a, *.dylib
#
# /usr/local/libexec/ccc-analyzer
# /usr/local/libexec/c++-analyzer
#
# /usr/local/share/clang/bash-autocomplete.sh
# /usr/local/share/clang/clang-doc-default-stylesheet.css
# /usr/local/share/clang/clang-format-bbedit.applescript
# /usr/local/share/clang/clang-format-diff.py
# /usr/local/share/clang/clang-format-sublime.py
# /usr/local/share/clang/clang-format.el
# /usr/local/share/clang/clang-format.py
# /usr/local/share/clang/clang-include-fixer.el
# /usr/local/share/clang/clang-include-fixer.py
# /usr/local/share/clang/clang-rename.py
# /usr/local/share/clang/clang-rename.el
# /usr/local/share/clang/clang-tidy-diff.py
# /usr/local/share/clang/index.js
# /usr/local/share/clang/run-clang-tidy.py
# /usr/local/share/clang/run-find-all-symbols.py
# /usr/local/share/man/man1/scan-build.1
# /usr/local/share/opt-viewer/opt-diff.py
# /usr/local/share/opt-viewer/opt-stats.py
# /usr/local/share/opt-viewer/opt-viewer.py
# /usr/local/share/opt-viewer/optpmap.py
# /usr/local/share/opt-viewer/optrecord.py
# /usr/local/share/opt-viewer/style.css
# /usr/local/share/scan-build/scanview.css
# /usr/local/share/scan-build/sorttable.js
# /usr/local/share/scan-view/ScanView.py
# /usr/local/share/scan-view/Reporter.py
# /usr/local/share/scan-view/startfile.py
# /usr/local/share/scan-view/FileRadar.scpt
# /usr/local/share/scan-view/GetRadarVersion.scpt
# /usr/local/share/scan-view/bugcatcher.ico

