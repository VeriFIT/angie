#!/bin/bash 
# Absolute path to this script, e.g. /home/user/bin/foo.sh
SCRIPT=$(readlink -f "$0")
# Absolute path this script is in, thus /home/user/bin
SCRIPTPATH=$(dirname "$SCRIPT")
cd $SCRIPTPATH

# supply different values if you wish to override
: ${COPT:=opt}
: ${CLANG:=clang}
: ${CLANGXX:=clang++}

# for predator testcasses
export CPATH=$SCRIPTPATH

# $CLANGXX -S -g -emit-llvm input1.cpp -o - | $COPT -lowerswitch -S -o input1.ll
# $CLANGXX -S -g -emit-llvm input2.cpp -o - | $COPT -lowerswitch -S -o input2.ll
# $CLANGXX -S -g -emit-llvm input3.cpp -o - | $COPT -lowerswitch -S -o input3.ll
# $CLANGXX -S -g -emit-llvm input3.cpp -o - | $COPT -lowerswitch -mem2reg -S -o input3m.ll
# $CLANGXX -S -g -emit-llvm input-int-conv.cpp -o - | $COPT -lowerswitch -S -o input-int-conv.ll
# $CLANGXX -S -g -emit-llvm input-logop.cpp -o - | $COPT -lowerswitch -S -o input-logop.ll

# the following set-up skips -mem2reg and -simplecfg for retaining switch-less, phi-less output
passes_normal="-lowerswitch -globalopt -demanded-bits -branch-prob -inferattrs -ipsccp -dse -loop-simplify -scoped-noalias -barrier -adce -memdep -licm -globals-aa -rpo-functionattrs -basiccg -loop-idiom -forceattrs -early-cse -instcombine -sccp "
passes_leave_deadcode="-lowerswitch -globalopt -demanded-bits -branch-prob -inferattrs -ipsccp      -loop-simplify -scoped-noalias -barrier       -memdep -licm -globals-aa -rpo-functionattrs -basiccg -loop-idiom -forceattrs                         -sccp "

directory=examples
ext=.c
deadcode="\[dead\]"
for ll_file in $(ls $directory/*$ext); do
	echo $ll_file
	filen=$(basename $ll_file $ext)
	dirn=$(dirname $ll_file)
	if grep -q -P $deadcode <<<$filen; then
		passes=$passes_leave_deadcode
	else
		# because some of the extra passes introduce ConstExpr into code, temporarily disable them
		# passes=$passes_normal
		passes=$passes_leave_deadcode
	fi
	echo "$CLANG -S -g -emit-llvm $dirn/$filen.c -o - | $COPT $passes -S -o $dirn/$filen.ll"
	$CLANG -S -g -emit-llvm $dirn/$filen.c -o - | $COPT $passes -S -o $dirn/$filen.ll
done
