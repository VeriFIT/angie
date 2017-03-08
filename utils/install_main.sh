#!/bin/sh

# PREFIX=/home/local
# PATH=$PREFIX/bin:$PATH
# LD_LIBRARY_PATH=$PREFIX/lib
# BOOST_ROOT=/home/local
# CXX=clang++
# export PATH LD_LIBRARY_PATH BOOST_ROOT CXX

# exec bash

usage() {
	echo "Usage: [environment] install_main.sh <name of package>"
	echo "Installs <name of package> into the PREFIXed location"
	echo "Pacakges:"
	echo "   gtest"
	echo "   boost"
	echo "   range-v3"
	echo "   gsl"
	echo "   croaring"
	echo "   export - special, exports to environemtn variables... './ install_main.sh export'"
}
envir() {
	echo "Environment:"
	echo "   PREFIX=$PREFIX"
	echo "   INC_DIR=$INC_DIR"
	echo "   LIB_DIR=$LIB_DIR"
	echo "   BIN_DIR=$BIN_DIR"
	echo "   SHARE_DIR=$SHARE_DIR"
}

set -e # set -o errexit

if [ -z ${1:-} ]; then
	usage
	envir
	exit 1
fi

if [ -z "$WORK_DIR" ]; then
	WORK_DIR="$HOME"
fi

# should be absolute path, or transformed to one
if [ -z "$PREFIX" ]; then
	PREFIX="/usr/local"
fi

if [ -z "$INC_DIR" ]; then
	INC_DIR="$PREFIX/include"
fi

if [ -z "$BIN_DIR" ]; then
	BIN_DIR="$PREFIX/bin"
fi

if [ -z "$LIB_DIR" ]; then
	LIB_DIR="$PREFIX/lib"
fi

if [ -z "$SHARE_DIR" ]; then
	SHARE_DIR="$PREFIX/share"
fi

if [ "${1}" = "export" ]; then
	echo export LIBRARY_PATH="$LIB_DIR:$LD_LIBRARY_PATH" LD_LIBRARY_PATH="$LIB_DIR:$LD_LIBRARY_PATH" CPATH="$INC_DIR:$CPATH" INCLUDE="$INC_DIR:$CPATH" PATH="$BIN_DIR:$PATH"
	exit 0
fi

set -u # set -o nounset

PKG_NAME=               # DEFAULT: $REPO_NAME
REPO_AUTHOR=            # google
REPO_NAME=              # googletest
BRANCH=                 # DEFAULT: master
FORMAT=                 # DEFAULT: tar.gz
ARCHIVE_FILE_NAME=      # DEFAULT: "$REPO_NAME.$FORMAT"
ARCHIVE_WHITELIST=      # OPTIONAL: select files to extract from archive
: ARCHIVE_ROOTDIR_NAME= # DEFAULT:  "$REPO_NAME-$BRANCH" if EMPTY __OR__ EMPTY if NULL/NOT-SET
LINK=                   # DEFAULT: "https://codeload.github.com/$REPO_AUTHOR/$REPO_NAME/$FORMAT/$BRANCH"

install_main() {
	# set def
	# REPO_AUTHOR=google
	# REPO_NAME=googletest
	: ${PKG_NAME:=$REPO_NAME}
	: ${PKG_NAME:?}
	: ${BRANCH:=master}
	: ${FORMAT:=tar.gz} # zip | tar.gz
	: ${ARCHIVE_FILE_NAME:=${REPO_NAME:?}.${FORMAT:?}}
	: ${ARCHIVE_ROOTDIR_NAME=${REPO_NAME:?}-${BRANCH:?}} # set empty if is null, keep is set, generate if empty
	: ${ARCHIVE_WHITELIST:=}
	# LINK="https://github.com/$REPO_AUTHOR/$REPO_NAME/archive/$BRANCH.$FORMAT"
	: ${LINK:="https://codeload.github.com/$REPO_AUTHOR/$REPO_NAME/$FORMAT/$BRANCH"}

	ARCHIVE_WHITELIST_PREFIXED=
	if [ ! -z ARCHIVE_ROOTDIR_NAME ]; then
		for ENTRY in $ARCHIVE_WHITELIST; do
			ARCHIVE_WHITELIST_PREFIXED="$ARCHIVE_ROOTDIR_NAME/$ENTRY"
		done
	else
		ARCHIVE_WHITELIST_PREFIXED="$ARCHIVE_WHITELIST"
	fi

	if check_not_installed; then

		cd $WORK_DIR
		download_unpack
		build_install
		if [ ${CLEANUP:-OFF} != OFF ]; then
			cleanup ## cd .. && rm -rf XXX
		fi
		echo "=================================================="
		echo "Finished!"
		echo "=================================================="
	else
		already_installed
	fi
}

already_installed() {
	echo "Already installed - found $INC_DIR/$PKG_NAME"
}

check_not_installed() {
	# test by includes
	[ ! -f "$INC_DIR/$PKG_NAME" ]
}

download_unpack() {
	set -x
	curl -L -o $ARCHIVE_FILE_NAME $LINK
	mkdir -p $PKG_NAME-src
	cd $PKG_NAME-src
	tar -xzf ../$ARCHIVE_FILE_NAME $ARCHIVE_WHITELIST_PREFIXED
	set +x
	cd ..
}

cleanup() {
	rm -rf $PKG_NAME-src
	rm -rf $PKG_NAME-build
}

build_install_cmake() {
	mkdir -p $PKG_NAME-build
	cd $PKG_NAME-build
	set -x
	cmake -DCMAKE_INSTALL_PREFIX=$PREFIX "$@" "../$PKG_NAME-src/$ARCHIVE_ROOTDIR_NAME"
	cmake --build . --target install --config ${BUILD_CONFIG:-debug}
	set +x
	cd ..
}

build_install_to_include() {
	cd $PKG_NAME-src
	# mkdir -p $INC_DIR/$PKG_NAME
	set -x
	mv -t $INC_DIR $ARCHIVE_WHITELIST_PREFIXED
	set +x
}

build_install_merge_include() {
	cd $PKG_NAME-src
	# mkdir -p $INC_DIR/$PKG_NAME
	set -x
	for ENTRY in $ARCHIVE_WHITELIST_PREFIXED; do
		mv -t $INC_DIR $ENTRY/*
	done
	set +x
}

check_not_installed() {
	[ ! -d "$INC_DIR/$PKG_NAME" ]
}

####################################################

# Implement this!!!

# build_install() {
# 	# build_install_cmake
# 	# build_install_include
# }

####################################################

export INSTALL_INVOKE=YES
. $(dirname $0)/install_$1.sh

mkdir -p $PREFIX $INC_DIR $BIN_DIR $LIB_DIR $SHARE_DIR

envir

install_main

# case "$1" in
# 	uninstall)
# 		echo "Uninstalling git-flow from $PREFIX"
# 		if [ -d "$BINDIR" ]; then
# 			for script_file in $SCRIPT_FILES $EXEC_FILES; do
# 				echo "rm -vf $BINDIR/$script_file"
# 				rm -vf "$BINDIR/$script_file"
# 			done
# 			rm -rf "$DOCDIR"
# 		else
# 			echo "The '$BINDIR' directory was not found."
# 		fi
# 		exit
# 		;;
# 	help)
# 		usage
# 		exit
# 		;;
# 	install)
# 		if [ -z $2 ]; then
# 			usage
# 			exit
# 		fi
# 		echo "Installing git-flow to $BINDIR"
# 		if [ -d "$REPO_NAME" -a -d "$REPO_NAME/.git" ]; then
# 			echo "Using existing repo: $REPO_NAME"
# 		else
# 			echo "Cloning repo from GitHub to $REPO_NAME"
# 			git clone "$REPO_HOME" "$REPO_NAME"
# 		fi
# 		cd "$REPO_NAME"
# 		git pull
# 		cd "$OLDPWD"
# 		case "$2" in
# 			stable)
# 				cd "$REPO_NAME"
# 				git checkout master
# 				cd "$OLDPWD"
# 				;;
# 			develop)
# 				cd "$REPO_NAME"
# 				git checkout develop
# 				cd "$OLDPWD"
# 				;;
# 			*)
# 				usage
# 				exit
# 				;;
# 		esac
# 		install -v -d -m 0755 "$PREFIX/bin"
# 		install -v -d -m 0755 "$DOCDIR/hooks"
# 		for exec_file in $EXEC_FILES; do
# 			install -v -m 0755 "$REPO_NAME/$exec_file" "$BINDIR"
# 		done
# 		for script_file in $SCRIPT_FILES; do
# 			install -v -m 0644 "$REPO_NAME/$script_file" "$BINDIR"
# 		done
# 		for hook_file in $HOOK_FILES; do
# 			install -v -m 0644 "$hook_file" "$DOCDIR/hooks"
# 		done
# 		exit
# 		;;
# 	*)
# 		usage
# 		exit
# 		;;
# esac
