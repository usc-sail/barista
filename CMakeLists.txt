# This script is partly based on libcppa cmake script.

###############################################################################
# Setup cmake and check compiler                                              #
###############################################################################

cmake_minimum_required (VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE 0)
project (Barista CXX)

# prohibit in-source builds
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please use "
                        "./configure to choose a build directory and "
                        "initialize the build configuration.")
endif ()

# Additional files used by cmake
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# check for g++ >= 4.7
try_run(ProgramResult
        CompilationSucceeded
        ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH}/get_compiler_version.cpp
        RUN_OUTPUT_VARIABLE CompilerVersion)
if (NOT CompilationSucceeded OR NOT ProgramResult EQUAL 0)
  message(FATAL_ERROR "Cannot determine compiler version")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" AND 
        CompilerVersion VERSION_GREATER 4.6)
else ()  
  message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} ${CompilerVersion}"
                      " is not supported. g++ >= 4.7 required.")
endif ()


###############################################################################
# Set paths                                                                   #
###############################################################################

set(BARISTA_ROOT ${CMAKE_SOURCE_DIR})
set(BARISTA_SRC_DIR ${BARISTA_ROOT}/src)
set(BARISTA_TOOLS_DIR ${BARISTA_ROOT}/tools)

#-----------------------------------------------------------------------------#

# Find Kaldi and its dependencies (Atlas, OpenFst, portaudio)

if (NOT KALDI_ROOT)
  set(KALDI_ROOT ${BARISTA_TOOLS_DIR}/kaldi)
endif()
if (VERBOSE)
  find_package(Kaldi REQUIRED)
else ()
  find_package(Kaldi QUIET REQUIRED)
endif ()
if (KALDI_VERSION)
  if (${KALDI_VERSION} VERSION_LESS "3755")
    message(FATAL_ERROR "Kaldi revision >= 3755 is required.")
  endif()
endif()

# TODO: we should maybe look into kaldi/src/kaldi.mk to figure out ATLAS paths.
if (NOT ATLAS_ROOT)
  set(ATLAS_ROOT ${KALDI_TOOLS_DIR}/ATLAS)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(ENV{ATLAS_DIR} ${ATLAS_ROOT})
  if (VERBOSE)
    find_package(Atlas REQUIRED)
  else ()
    find_package(Atlas QUIET REQUIRED)
  endif ()
else()
  set(ATLAS_INCLUDE_DIR ${ATLAS_ROOT}/include)
endif()

#-----------------------------------------------------------------------------#

# Find libcppa
if (NOT CPPA_ROOT)
  set(CPPA_ROOT ${BARISTA_TOOLS_DIR}/libcppa)
endif ()
if (VERBOSE)
  find_package(LibCPPA REQUIRED)
else ()
  find_package(LibCPPA QUIET REQUIRED)
endif ()
if (NOT ${CPPA_VERSION} VERSION_EQUAL "0.8.1")
  message(FATAL_ERROR "libcppa version 0.8.1 is required.")
endif()

#-----------------------------------------------------------------------------#

# Find GraphViz
if (NOT GRAPHVIZ_ROOT)
  set(GRAPHVIZ_ROOT ${BARISTA_TOOLS_DIR}/graphviz)
endif()
set(ENV{GRAPHVIZ_ROOT} ${GRAPHVIZ_ROOT})
if (VERBOSE)
  find_package(Graphviz REQUIRED)
else ()
  find_package(Graphviz QUIET REQUIRED)
endif ()
if (NOT ${GRAPHVIZ_VERSION} VERSION_GREATER "2.29")
  message(FATAL_ERROR "GraphViz version >= 2.30 is required.")
endif()

#-----------------------------------------------------------------------------#

# Find Boost
if (BOOST_ROOT)
  set(Boost_NO_SYSTEM_PATHS true)
endif ()
find_package(Boost REQUIRED)
if (NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found.")
endif ()


###############################################################################
# Set compiler and linker flags                                               #
###############################################################################
#                                                                             #
# How to set compiler flags:                                                  #
# 1. Copy the flags set by CXXFLAGS in KALDI_ROOT/src/kaldi.mk                #
# 2. Add "-std=c++11 -fpermissive"                                            #
# 3. Add "-Wno-unused-local-typedefs" to silence the warnings about OpenFst.  #
#                                                                             #
# How to set linker flags:                                                    #
# 1. Copy the flags set by LDFLAGS and LDLIBS in KALDI_ROOT/src/kaldi.mk      #
# 2. Copy the frameworks set by SHARED_FLAGS in                               #
#    KALDI_ROOT/tools/portaudio/Makefile                                      #
#                                                                             #
# How to set portaudio dependencies for Linux systems:                        #
# 1. Copy the libs listed under LIBS and DLL_LIBS in                          #
#    KALDI_ROOT/tools/portaudio/Makefile                                      #
###############################################################################

# Extract platform dependent portaudio dependencies from the libtool library
execute_process(COMMAND grep "^dependency_libs" ${PORTAUDIO_LIBTOOL_FILE}
                COMMAND sed "s/.*=//; s/'//g; s/^ *//; s/ *$//;"
                COMMAND tr -d "\n"
                OUTPUT_VARIABLE DEPENDENCY_LIBS)

execute_process(COMMAND grep "^inherited" ${PORTAUDIO_LIBTOOL_FILE}
                COMMAND sed "s/.*=//; s/'//g; s/^ *//; s/ *$//;"
                COMMAND tr -d "\n"
                OUTPUT_VARIABLE DEPENDENCY_FLAGS)

set (CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -msse -msse2 -fPIC -Wall -DKALDI_DOUBLEPRECISION=0 -DHAVE_POSIX_MEMALIGN -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -Winit-self -Wno-unused-local-typedefs -Wno-sign-compare -flax-vector-conversions -fpermissive -g -std=c++11")

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${DEPENDENCY_FLAGS}")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Typical flags for Linux
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_ATLAS")
  
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Typical flags for Mac OS X
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_CLAPACK")
  
  set (CMAKE_EXE_LINKER_FLAGS 
    "${CMAKE_EXE_LINKER_FLAGS} -framework Accelerate")

else ()
  message(FATAL_ERROR "Barista does not support ${CMAKE_SYSTEM_NAME}.")
endif()

set_source_files_properties(src/decoder/faster-online-decoder.cc PROPERTIES COMPILE_FLAGS -O3)


###############################################################################
# Add targets (You probably don't need to change anything beyond this point)  #
###############################################################################

# link_directories(${OPENFST_LIB_DIR} ${CPPA_LIB_DIR})

include_directories(
  ${BARISTA_SRC_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GRAPHVIZ_INCLUDE_DIRS}
  ${CPPA_INCLUDE_DIR}
  ${KALDI_SRC_DIR}
  ${OPENFST_INCLUDE_DIR}
  ${ATLAS_INCLUDE_DIR}
  ${PORTAUDIO_INCLUDE_DIR}
)

add_library(barista-base STATIC 
  src/base/module-base.cc
)

add_library(barista-io STATIC 
  src/io/file-list-reader.cc src/io/pcm-reader.cc
  src/io/command-line-interface.cc src/io/portaudio-reader.cc
  src/io/vector-writer.cc src/io/matrix-writer.cc
)

add_library(barista-feat STATIC 
  src/feat/compute-mfcc-feats.cc src/feat/apply-cmvn.cc src/feat/add-deltas.cc
  src/feat/filter.cc
)

add_library(barista-decoder STATIC 
  src/decoder/faster-online-decoder.cc
)

add_library(barista-gmm STATIC 
  src/gmm/gmm-decode-faster-online.cc
)

add_executable(barista src/bin/barista.cc)

target_link_libraries(barista 
  barista-base barista-io barista-feat barista-decoder barista-gmm
  ${CPPA_LIBRARY}
  ${GRAPHVIZ_GVC_LIBRARY} ${GRAPHVIZ_CGRAPH_LIBRARY}
  ${KALDI_LIBRARIES}
  ${ATLAS_LIBRARIES}  # empty on mac os x
  ${OPENFST_LIBRARY}
  ${PORTAUDIO_LIBRARY}
  dl m pthread
  ${DEPENDENCY_LIBS}  # empty on mac os x
)


###############################################################################
# Installation Setup                                                          #
###############################################################################

# install executables and libraries
install(TARGETS 
  barista 
  barista-base barista-io barista-feat barista-decoder barista-gmm
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib  
  ARCHIVE DESTINATION lib
)

# install header files
install(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.h")

# process cmake_uninstall.cmake.in
configure_file("${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

# add uninstall target
add_custom_target(uninstall
                  COMMAND ${CMAKE_COMMAND} -P
                  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

