Angie project
=============

![alt](https://cdn.travis-ci.org/images/favicon-076a22660830dc325cc8ed70e7146a59.png)
[![Travis CI](https://api.travis-ci.org/VeriFIT/angie.svg?branch=master)](https://travis-ci.org/VeriFIT/angie)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/4edca1b2f3u0bxhb?svg=true)](https://ci.appveyor.com/project/VeriFIT/angie)


Angie is a static analysis and verification framework, specifically for methods based on abstract interpretation.

- The current goal is to implement SMG abstraction
  [extended paper]( http://www.fit.vutbr.cz/research/groups/verifit/tools/predator/FIT-TR-2012-04.pdf ), 
  [original paper]( http://link.springer.com/chapter/10.1007/978-3-642-38856-9_13 )
- Inspired by [Predator](https://github.com/kdudka/predator)
- Built primarily on top of LLVM but meant to be frontend-independent

The program is licensed under GNU LGPLv3+ and makes use of 
[LLVM](http://llvm.org), 
[Boost](http://boost.org), 
[Range-v3](https://github.com/ericniebler/range-v3) 
\[[Range-v3-VS2015](https://github.com/microsoft/Range-V3-VS2015)\], 
[GSL](https://github.com/Microsoft/GSL) 
\[[GSL-lite](https://github.com/microsoft/Range-V3-VS2015)\]
\[[V11-GSL](https://github.com/viboes/GSL)\],
[MemGraph](https://github.com/Moouseer/MemGraph)
\[[stripped fork](https://github.com/michkot/MemGraph)\]
and possibly also 
[Z3](https://github.com/Z3Prover/z3), 
[CRoaring](https://github.com/RoaringBitmap/CRoaring). 
All those are to my best knowledge distributed under compatible licenses.

Proud user of:
- https://github.com/petervanderdoes/gitflow-avh/

Build & Run
-----------
```sh
# set to your executables
COPT=opt
CLANGXX=clang++
CLANG=clang
export COPT CLANGXX CLANG
(bash ./compile-inputs-to-ll.sh <DIRECTORY_WITH_SOURCE_FILES>; mkdir tmp; cd tmp; cmake ../; make)
# ./tmp/angie --help
./tmp/angie -f <FILE_TO_ANALYSE>
```

Dependencies install
--------------------
First of all, `memgraph` submodule must be initialized and checked out:

```sh
git submodule init
git submodule update
```

For simple deployment, `install_*` helpers in `utils` directory will handle installation of CMake built or include-only libraries. Main motivation was repetitive fixing of Travis-CI and custom *nix server deployment.
Currently needed dependencies to get Angie compiled:
```sh
# installation prefix for script, select any path, $HOME/local, ...
export PREFIX=$PWD/dependencies
# install missing libraries
sh utils/install_main.sh gtest
sh utils/install_main.sh boost
# install range-v3-vs2015 [MS VC14+ workaround support] or range-v3 [clang/gcc ISO C++14]
sh utils/install_main.sh range-v3-vs2015
# install gsl [C++14] or gsl-lite [C++11, custom fork with macros disabled]
sh utils/install_main.sh gsl-lite
# export or '-D'set CMAKE_PREFIX_PATH or CMAKE_INCLUDE_PATH
export CMAKE_PREFIX_PATH=$PREFIX
```

Contribution and credits
-------
See `CREDITS.txt`

License
-------
See `LICENSE.txt`
