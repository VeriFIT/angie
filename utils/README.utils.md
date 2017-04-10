How to use
----------
```sh
# for help:
sh utils/install_main.sh

# A)
# set installation prefix, select any path, $HOME/local, $PWD/dep, ...
export PREFIX=$HOME/local
# or set all concrete dirs for includes, libs, bins, see help
export INC_DIR=$HOME/local/include

# B)
# install packages
sh utils/install_main.sh <package-name>

# C) 
# boostrap installed packages into your environment
# a) via exporting LIBRARY_PATH, CPATH, ... variables known to compilers
# this command will print the export commands to stdout
sh utils/install_main.sh export
# after checking the output, execute it in current shell
$(sh utils/install_main.sh export)
# b) '-D'set or export these paths co CMake
export CMAKE_PREFIX_PATH=$PREFIX
# or just yours include path
export CMAKE_INCLUDE_PATH=$HOME/local/include
```
