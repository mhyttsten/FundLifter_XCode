#!/bin/bash

# set -o xtrace  # This prints all commands
echo "" 

if [ $# -ne 3 ]
  then
    echo "Usage: <source_file.td> <target_dir/file> [-gen-op-decls|-gen-op-defs]"
    echo "   Observe, don't specify suffix on target file, it is deduced through 3rd argument"
    exit 1
fi


# echo "Entering with arguments 1: $1, 2: $2, 3: $3"

if [[ ! $1 =~ \.td$ ]]
then
   echo "File needs to have .td suffix"
   exit 1
fi

if [ $3 == "-gen-op-decls" ]
then
   suffix=".h"
elif [ $3 == "-gen-op-defs" ]
then
   suffix=".cpp"
elif [ $3 == "-gen-op-interface-decls" ]
then
   suffix=".h"
elif [ $3 == "-gen-op-interface-defs" ]
then
   suffix=".cpp"
elif [ $3 == "-gen-op-defs" ]
then
   suffix=".cpp"
elif [ $3 == "-gen-rewriters" ]
then
   suffix=""
else
   echo "Expected [-gen-op-decls | -gen-op-defs | -gen-rewriters ] as 3rd argument"
   exit 1
fi


target_file="/tmp/build_tmp/$2$suffix.inc"
target_file_d="$target_file.d"
target_dir=$(dirname $target_file)
[ ! -d $target_dir ] && echo "Creating directory: $target_dir"
mkdir -p $target_dir

CMD="mlir-tblgen \
   $3 \
   -I/usr/local/include \
   -Iinclude \
   $1 \
   --write-if-changed \
   -o ${target_file} \
   -d ${target_file_d}"
echo $CMD
$CMD

   
