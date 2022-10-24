#!/bin/sh

cd $HOME/products/tf-base
rm -rf venv
python3 -m venv --system-site-packages venv
source venv/bin/activate
python3.9 -m pip install --upgrade pip

pip3 install -U --user pip numpy wheel packaging six requests 1>/dev/null
pip3 install -U --user keras_preprocessing --no-deps 1>/dev/null

git clone https://github.com/tensorflow/tensorflow.git
mv tensorflow tf-$(date +"%y%m%d")
rm -f tf-current
ln -s tf-$(date +"%y%m%d") tf-current
cd tf-current
# git checkout $1

printf "\nRunning configure build script\n"
PYTHON_LOC="$(which python)"
PYTHON_LIB_LOC="$(find ~/products/tf-base/venv -name site-packages)"
{ printf "$PYTHON_LOC\n"; printf "$PYTHON_LIB_LOC\n" ; } | ./configure
# printf "\nContent of .tf_configure.bazelrc\n"
# cat .tf_configure.bazelrc
# printf "\n"

# printf "\nRunning MHTraceMaker, optional"
# PATH env variable must contain: MHTRACER_ENABLE
find . -name \*.cc -type f -exec AutoCPPTracer TRACE {} \;
find . -name \*.cpp -type f -exec AutoCPPTracer TRACE {} \;
find . -name \*.h -type f -exec AutoCPPTracer TRACE {} \;
# ERROR, NOT SUPPORTED: find . -name \*.c -type f -exec MHTraceMaker TRACE {} \;  # We don't support C files

printf "\nBuilding TensorFlow"
#bazel build --subcommands --verbose_failures --strip=never -c dbg --copt="-fno-inline-functions" \
#     --per_file_copt=external/.*\.cpp@-g0,-DMAGNUS \
#     --per_file_copt=external/.*\.cc@-g0,-DMAGNUS \
#     //tensorflow/tools/pip_package:build_pip_package
#     --jobs 1  # To limit jobs to 1, giving it all memory available
# bazel build --subcommands --verbose_failures --copt="-fno-inline-functions" //tensorflow/tools/pip_package:build_pip_package
bazel clean --expunge
rm -rf /var/tmp/_bazel_magnushyttsten/
# bazel build --verbose_failures --copt="-fno-inline-functions" //tensorflow/tools/pip_package:build_pip_package
bazel build --verbose_failures //tensorflow/tools/pip_package:build_pip_package

#printf "\n*********************************\n"
#printf " $(PWD)\n"
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
