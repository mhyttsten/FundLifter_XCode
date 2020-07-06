#!/bin/bash

if [ $# -eq 0 ]
  then
    echo "Need file to compile as argument"
    exit 1
fi

echo ""
[ ! -d "/tmp/build_tmp" ] && echo "Creating directory: /tmp/build_tmp"
mkdir -p /tmp/build_tmp

CMD="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ \
   -DBUILD_EXAMPLES -D_DEBUG -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS \
   -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk/usr/include/libxml2 \
   -Iinclude \
   -I/tmp/build_tmp \
   -I/usr/local/include \
   -fPIC -fvisibility-inlines-hidden -Werror=date-time -Werror=unguarded-availability-new \
   -Wall -Wextra -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wmissing-field-initializers \
   -pedantic -Wno-long-long -Wimplicit-fallthrough -Wcovered-switch-default -Wno-noexcept-type \
   -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -Wstring-conversion -fdiagnostics-color -O3 \
   -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
   -UNDEBUG -fno-exceptions -std=c++14 \
   -MD -MT /tmp/build_tmp/$(basename $1).o -MF /tmp/build_tmp/$(basename $1).o.d \
   -o /tmp/build_tmp/$(basename $1).o \
   -c $1"
echo $CMD
$CMD

   
