
#!/bin/bash

#if [ $# -eq 0 ]
#  then
#    echo "Need file to compile as argument"
#    exit 1
#fi

#if [[ $1 != *.td ]]
#  then
#     echo "File needs to have .td suffix"
#     exit 1
#fi

echo $1
# mkdir -p build  # Don't fail if exists, also created recursively if multiple paths

# Create directory if it doesn't exist
[ ! -d $target_dir ] && echo "Creating directory: $target_dir"

test_file="include/testing/include/Hello.td"
target_file=${1%.td}  # Remove TD suffix
echo $target_file
only_path_1=$(echo $target_file | cut -d "/" -f1)  # Extract first path part
echo $only_path
if [ $only_path_1 == "include" ]  # If first path part is "include"
  then
     target_file=${target_file#*/}  # Then take everything except "include/"
fi
echo $target_file
target_dir=${target_file%/*}   # Take everything but ending "/Hello.td"
echo $target_dir
if [ $target_dir != "" ]
  then
     echo "target_dir was empty"
fi
target_file=build/${target_file}  # Prepend "build/"
echo $target_file

