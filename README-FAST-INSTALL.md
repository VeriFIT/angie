Angie - Fast Install Instructions
=============
```sh
# clone repository

# init & sync submodules
git submodule init 
git submodule update
# directory ./memgraph must contain files now!

# LLVM install is not covered -- install version 3.9.x now!

# create directory for angie && change to that directory

export PREFIX=$PWD/dependencies
export CMAKE_PREFIX_PATH=$PREFIX

# locally install non-LLVM dependencies
sh utils/install_main.sh gtest
sh utils/install_main.sh boost
sh utils/install_main.sh range-v3-vs2015
sh utils/install_main.sh gsl-lite

# compile Angie and examples
COPT=opt
CLANGXX=clang++
CLANG=clang
export COPT CLANGXX CLANG
(mkdir tmp; cd tmp; cmake ../; make)
bash ./compile-inputs-to-ll.sh examples

# ./tmp/angie --help
# ./tmp/angie -f examples/02_heap_01.ll
./tmp/angie -f <FILE_TO_ANALYSE>
```
