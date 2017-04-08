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
#    *.hh
#    *.cc
#)

add_definitions(-DNOT_COMPILE_Z3)

set(SOURCE_FILES
    src/AngieType.cc
    src/CfgNodeBase.hh
    src/console.cc
    src/Definitions.hh
    src/DummyOperations.hh
    src/DummyState.hh
    src/enum_flags.h
    src/Exceptions.cc
    src/Exceptions.hh
    src/ForwardNullAnalysis.hh
    src/FrontedValueMapper.hh
    src/General.hh
    src/ICfgNode.hh
    src/IdImpl.hh
    src/IOperation.hh
    src/ISmgVisitor.hh
    src/IState.hh
    src/laboratory.cc
    src/LlvmFrontend.cc
    src/LlvmFrontend.hh
    src/LlvmGlobals.cc
    src/LlvmGlobals.hh
    src/LlvmType.cc
    src/main.cc
    src/MemoryGraphAnalysis.hh
    src/Operation.hh
    src/Smg/Edge.cc
    src/Smg/Edge.hh
    src/Smg/Graph.cc
    src/Smg/Graph.hh
    src/Smg/Object.cc
    src/Smg/Object.hh
    src/Smg/ObjectId.cc
    src/Smg/ObjectId.hh
    src/Smg/Smg_fwd.hh
    src/Smg/Wrappers.cc
    src/Smg/Wrappers.hh
    src/Smg/Wrappers_fwd.hh
    src/Smg.cc
    src/Smg.hh
    src/SmgCrawler.cc
    src/SmgCrawler.hh
    src/SmgPrinter.cc
    src/SmgPrinter.hh
    src/StateStorage.cc
    src/StateStorage.hh
    src/Type.hh
    src/ValueContainerV1.cc
    src/ValueContainerV1.hh
    src/ValueId.cc
    src/ValueId.hh
    src/Values.hh
    src/ValuesZ3.cc
    src/ValuesZ3.hh
)

if(Z3)
    remove_definitions(-DNOT_COMPILE_Z3)
    add_definitions(-DUSE_Z3)
    # set(SOURCE_FILES
    #     ${SOURCE_FILES}
    #     src/ValuesZ3.hh
    #     src/ValuesZ3.cc
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

set_source_files_properties( src/console.cc PROPERTIES COMPILE_FLAGS -fno-rtti)

add_subdirectory(memgraph)

target_link_libraries(${PROJECT_NAME} ${LLVM_LIBS} ${Z3_LIBS} memgraph)

# end
