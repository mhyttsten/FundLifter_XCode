#!/bin/bash

if [ $# -eq 0 ]
  then
    echo "Need file to compile as argument"
    exit 1
fi

echo $1
mkdir -p build

/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ \
   -DBUILD_EXAMPLES -D_DEBUG -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS \
   -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk/usr/include/libxml2 \
   -Iinclude \
   -Ibuild \
   -I/usr/local/include \
   -fPIC -fvisibility-inlines-hidden -Werror=date-time -Werror=unguarded-availability-new \
   -Wall -Wextra -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wmissing-field-initializers \
   -pedantic -Wno-long-long -Wimplicit-fallthrough -Wcovered-switch-default -Wno-noexcept-type \
   -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -Wstring-conversion -fdiagnostics-color -O3 \
   -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
   -UNDEBUG -fno-exceptions -std=c++14 \
   -MD -MT build/$(basename $1).o -MF build/$(basename $1).o.d \
   -o build/$(basename $1).o \
   -c $1
   
