cmake_minimum_required(VERSION 2.8)
project(angie)

# for ValuesZ3 Z3prover
set(Z3 0)

# packages
# LLVM
find_package(LLVM 3.9 REQUIRED CONFIG)
message(STATUS "LLVM version ${LLVM_PACKAGE_VERSION}")

# Boost
find_package(Boost 1.62 REQUIRED)

# gtest
find_package(GTest)

# range-v3
find_path(range_v3_DIR range/v3)
if(NOT range_v3_DIR STREQUAL "range_v3_DIR-NOTFOUND")
    set(range_v3_INCLUDE_DIRS "${range_v3_DIR}")
    message(STATUS "range-v3 location ${range_v3_DIR}")
else()
    message(WARNING "Could NOT find range-v3")
endif()

# GSL
find_path(GSL_DIR gsl)
if(NOT GSL_DIR STREQUAL "GSL_DIR-NOTFOUND")
    set(GSL_INCLUDE_DIRS "${GSL_DIR}")
    message(STATUS "GSL location ${GSL_DIR}")
else()
    message(WARNING "Could NOT find GSL")
endif()

if(Z3)
    # find_package(Z3)
endif()


# compiler directives

set(USE_SANITIZERS OFF)
set(CMAKE_VERBOSE_MAKEFILE off)

# set(GLOBALFLAGS -fPIC)
if (USE_SANITIZERS AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(SANIT_FLAGS "-fsanitize=address -fsanitize=leak -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
else()
    set(SANIT_FLAGS "")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -o3")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SANIT_FLAGS} -g")

#file(GLOB SOURCE_FILES
#    *.hpp
#    *.cpp
#)

add_definitions(-DNOT_COMPILE_Z3)

set(SOURCE_FILES
    src/Analysis.hpp
    src/AngieType.cpp
    src/CfgNodeBase.hpp
    src/console.cpp
    src/Definitions.hpp
    src/DummyOperations.hpp
    src/DummyState.hpp
    src/enum_flags.h
    src/Exceptions.cpp
    src/Exceptions.hpp
    src/ForwardNullAnalysis.hpp
    src/FrontedValueMapper.hpp
    src/General.hpp
    src/ICfgNode.hpp
    src/IdImpl.hpp
    src/IOperation.hpp
    src/ISmgVisitor.hpp
    src/IState.hpp
    src/laboratory.cpp
    src/LlvmFrontend.cpp
    src/LlvmFrontend.hpp
    src/LlvmGlobals.cpp
    src/LlvmGlobals.hpp
    src/LlvmType.cpp
    src/main.cpp
    src/MemoryGraphAnalysis.hpp
    src/Operation.hpp
    src/OsUtils.hpp
    src/OsUtils.cpp
    src/Smg/Abstraction.hpp
    src/Smg/Edge.cpp
    src/Smg/Edge.hpp
    src/Smg/Graph.cpp
    src/Smg/Graph.hpp
    src/Smg/Object.cpp
    src/Smg/Object.hpp
    src/Smg/ObjectId.cpp
    src/Smg/ObjectId.hpp
    src/Smg/Smg_fwd.hpp
    src/Smg/Wrappers.cpp
    src/Smg/Wrappers.hpp
    src/Smg/Wrappers_fwd.hpp
    src/Smg.cpp
    src/Smg.hpp
    src/SmgCrawler.cpp
    src/SmgCrawler.hpp
    src/SmgPrinter.cpp
    src/SmgPrinter.hpp
    src/SmgUtils.hpp
    src/StateStorage.cpp
    src/StateStorage.hpp
    src/Type.hpp
    src/ValueContainerV1.cpp
    src/ValueContainerV1.hpp
    src/ValueId.cpp
    src/ValueId.hpp
    src/Values.hpp
    src/ValuesZ3.cpp
    src/ValuesZ3.hpp
)

if(Z3)
    remove_definitions(-DNOT_COMPILE_Z3)
    add_definitions(-DUSE_Z3)
    # set(SOURCE_FILES
    #     ${SOURCE_FILES}
    #     src/ValuesZ3.hpp
    #     src/ValuesZ3.cpp
    # )
endif()

# targets

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
# dirty dirty dirty way -> assumes BINARY DIR is something like SRC_DIR/build etc.
add_custom_target(tests 
    DEPENDS ${PROJECT_NAME} 
    WORKING_DIRECTORY "../" 
    COMMAND "bash" "./compile-inputs-to-ll.sh" 
    COMMAND "${PROJECT_BINARY_DIR}/${PROJECT_NAME}" "-t")

# includes

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Z3_INCLUDE_DIRS})
include_directories(${range_v3_INCLUDE_DIRS})
include_directories(${GSL_INCLUDE_DIRS})

# libs linking

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(LLVM_LIBS core irreader support)

set_source_files_properties( src/console.cpp PROPERTIES COMPILE_FLAGS -fno-rtti)

add_subdirectory(memgraph)

target_link_libraries(${PROJECT_NAME} ${LLVM_LIBS} ${Z3_LIBS} memgraph)

# end
