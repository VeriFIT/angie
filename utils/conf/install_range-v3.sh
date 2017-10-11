# to be SOURCEd via install_main.sh
if [ -z ${INSTALL_INVOKE:-} ]; then
	echo "This file is for sourcing from install_main.sh and should not be directly executed"
	exit 1
fi

# Name of the library/package/software
PKG_NAME=range            # DEFAULT: $REPO_NAME
:                         # Github repo
REPO_AUTHOR=ericniebler   # google
REPO_NAME=range-v3        # googletest
BRANCH=                   # DEFAULT: master
:                         # Archive
FORMAT=                   # DEFAULT: tar.gz
ARCHIVE_FILE_NAME=        # DEFAULT: "$REPO_NAME.$FORMAT"
: ARCHIVE_ROOTDIR_NAME=   # DEFAULT: "$REPO_NAME-$BRANCH" if NOT-SET __OR__ EMPTY if NULL/EMPTY
ARCHIVE_WHITELIST=include # OPTIONAL: select files to extract from archive. Gets prefixed with ARCHIVE_ROOTDIR_NAME
:                         # Download Link
LINK=                     # DEFAULT: "https://codeload.github.com/$REPO_AUTHOR/$REPO_NAME/$FORMAT/$BRANCH"
:                         # Misc.
CLEANUP=                  # DEFAULT: OFF
BUILD_CONFIG=             # DEFAULT: debug

build_install() {
	build_install_merge_include
}

####################################################
