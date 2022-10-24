#! /bin/bash

# set -o xtrace

if [ $# -ne 1 ]
  then
    printf "Usage: branch (e.g: r2.6), bazel cmdline (e.g \"-c dbg\"\n"
    exit 1
fi

# Remove any prevous builds
bazel clean --expunge
rm -f /tmp/tf*

printf "Script starting at: $(date)\n"

printf "\nTensorFlow git branch to build: $1\n"
GCC_PATH=$(which gcc)
printf "which gcc: $GCC_PATH\n"
printf "gcc --version: $(gcc --version)\n"
BAZEL_PATH=$(which bazel)
printf "which bazel: $BAZEL_PATH\n"
printf "bazel --version: $(bazel --version)\n"
printf "Python paths: $(ls -l /usr/local/opt/p*)\n"
printf "which python: $(which python)\n"
printf "Default python version: $(python --version)\n"
printf "which python3: $(which python3)\n"
printf "Default python3 version: $(python3 --version)\n"
printf "ls -ld python3 path: $(ls -ld $(which python3))\n"
CLANG_PATH=$(which clang)
printf "which clang: $CLANG_PATH\n"
printf "clang --version: $(clang --version)\n"
XCODE_PATH=$(xcode-select -p)
printf "Xcode toolchain location: $XCODE_PATH\n"
printf "Xcode toolchain clang version: $($XCODE_PATH/usr/bin/clang --version)\n"
printf "PATH\n$PATH\n"
printf "ENV\n$(env)\n"

declare -i timeStart=$(date +"%s")

printf "\nRemoving old venv\n"
cd ~/tf/venvs || { printf 'Expected ~/tf/venvs\n' ; exit 1; }
mv tf_debug tf_debug_$(date +"%y%m%d_%H%M%S") 2>/dev/null
# rm -rf tf_debug

printf "\nCreating new venv\n"
python3 -m venv --system-site-packages tf_debug
printf "\nSourcing new venv\n"
source tf_debug/bin/activate
python3.9 -m pip install --upgrade pip
printf "\nPath to env in tf_debug\n"
printf "   python: $(which python), version: $(python --version)\n"
printf "   pip:    $(which pip), version: $(pip --version)\n"
cd tf_debug

printf "\nInstalling pip packages\n"
# The two below are from installation instructions: https://github.com/tensorflow/docs/blob/master/site/en/install/source.md#macos-1
pip install -U pip numpy wheel 1>/dev/null
pip install -U keras_preprocessing --no-deps 1>/dev/null

# Below are from recommendation by Amit
#   It may be that your dependencies are stale
#   Here is what we install prior to building for MacOS
#   http://google3/third_party/tensorflow/tools/ci_build/release/common.sh.oss?l=158-210&rcl=341915841
#   I think the wheel version may be old
#   Which has that particular assert
#   I'd install everything to be safe
#a = '''
#pip install --upgrade setuptools pip wheel
#pip install 'absl-py ~= 0.10'
#pip install 'astunparse ~= 1.6.3'
#pip install 'flatbuffers ~= 1.12.0'
#pip install 'google_pasta ~= 0.2'
#pip install h5py  # 'h5py ~= 2.10.0'  # **** FAILS for Python 3.9 ****
#pip install 'keras_preprocessing ~= 1.1.2'
#pip install 'numpy ~= 1.19.2'
#pip install 'opt_einsum ~= 3.3.0'
#pip install 'protobuf ~= 3.13.0'
#pip install 'six ~= 1.15.0'
#pip install 'termcolor ~= 1.1.0'
#pip install 'typing_extensions ~= 3.7.4'
#pip install 'wheel ~= 0.35'
#pip install 'wrapt ~= 1.12.1'
#pip install 'gast == 0.3.3'    # We need to pin gast dependency exactly
#pip install grpcio  # 'grpcio ~= 1.32.0'  # **** FAILS for Python 3.9 ****
#pip install 'portpicker ~= 1.3.1'
#pip install 'scipy ~= 1.5.2'
#'''

# Finally, install tensorboard and estimator
# Note that here we want the latest version that matches (b/156523241)
# NOTE: WON'T DO THIS WE'RE BUILDING TF FROM SOURCE HERE
#    pip install --upgrade --force-reinstall 'tb-nightly ~= 2.4.0.a'
#    pip install --upgrade --force-reinstall 'tensorflow_estimator ~= 2.3.0'

printf "\nInstalling TensorFlow source\n"
git clone https://github.com/tensorflow/tensorflow.git ~/tf/venvs/tf_debug/tensorflow
# tar xf ../tensorflow.tar  # If we have a predefined one
cd tensorflow
git checkout $1

printf "\nRunning configure build script\n"
PYTHON_LOC="$(which python)"
PYTHON_LIB_LOC="$(find ~/tf/venvs/tf_debug -name site-packages)"
{ printf "$PYTHON_LOC\n"; printf "$PYTHON_LIB_LOC\n" ; } | ./configure

printf "\nContent of tools/python_bin_path.sh\n"
cat tools/python_bin_path.sh
printf "\n"

printf "\nContent of .tf_configure.bazelrc\n"
cat .tf_configure.bazelrc
printf "\n"

# cp /Users/magnushyttsten/code/source/XcodeProjects/FundLifter/AllAboutCPP/* tensorflow/tensorflow/magnus

printf "\nRunning MHTraceMaker, optional"
# find . -name \*.cc -type f -exec MHTraceMaker TRACE {} \;
# find . -name \*.cpp -type f -exec MHTraceMaker TRACE {} \;
# find . -name \*.h -type f -exec MHTraceMaker TRACE {} \;
# ERROR, NOT SUPPORTED: find . -name \*.c -type f -exec MHTraceMaker TRACE {} \;  # We don't support C files

printf "\nBuilding TensorFlow"
#bazel build --subcommands --verbose_failures --strip=never -c dbg --copt="-fno-inline-functions" \
#     --per_file_copt=external/.*\.cpp@-g0,-DMAGNUS \
#     --per_file_copt=external/.*\.cc@-g0,-DMAGNUS \
#     //tensorflow/tools/pip_package:build_pip_package
#     --jobs 1  # To limit jobs to 1, giving it all memory available
bazel build --subcommands --verbose_failures //tensorflow/tools/pip_package:build_pip_package

printf "\n*********************************\n"
printf "\n*********************************\n"
printf "\n*********************************\n"
printf " $(PWD)\n"
printf "Now building pip package\n"
rm -rf /tmp/tensorflow_pkg
./bazel-bin/tensorflow/tools/pip_package/build_pip_package /tmp/tensorflow_pkg

printf "\n*********************************\n"
printf "\n*********************************\n"
printf "\n*********************************\n"
printf "Now renaming .whl to 10_10\n"
printf "Now pip installing TensorFlow\n"
printf "which pip: $(which pip)\n"
printf "pip version: $(pip --version)\n"
# mv /tmp/tensorflow_pkg/$(ls /tmp/tensorflow_pkg) /tmp/tensorflow_pkg/$(ls /tmp/tensorflow_pkg | sed 's/11_0/10_10/g')
python3.9 -m pip install --force-reinstall /tmp/tensorflow_pkg/*.whl
# Needed, otherwise import tensorflow as tf errors:
# RuntimeError: module compiled against API version 0xe but this version of numpy is 0xd
pip install numpy --upgrade

printf "\n*********************************\n"
printf "\n*********************************\n"
printf "\n*********************************\n"
printf "Done\n"
declare -i timeDuration=$(date +"%s")-$timeStart
printf "Total execution time: $timeDuration seconds\n"
printf "Script ending at: $(date)\n"
printf "\n"
