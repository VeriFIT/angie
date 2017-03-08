Angie project
=============

![alt](https://cdn.travis-ci.org/images/favicon-076a22660830dc325cc8ed70e7146a59.png)
[![Travis CI](https://api.travis-ci.org/verifit/angie.svg?branch=master)](https://travis-ci.org/verifit/angie)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/4edca1b2f3u0bxhb?svg=true)](https://ci.appveyor.com/project/verifit/angie)


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
and possibly also 
[Z3](https://github.com/Z3Prover/z3), 
[GSL](https://github.com/Microsoft/GSL), 
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
(bash ./compile-inputs-to-ll.sh; mkdir tmp; cd tmp; cmake ../; make; cd .. ; ./tmp/ng)
```

Contribution and credits
-------
See `CREDITS.txt`

License
-------
See `LICENSE.txt`
