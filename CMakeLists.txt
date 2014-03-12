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

# TODO: we should have a FindCPPA.cmake script which will find these.
set(CPPA_INCLUDE_DIR ${CPPA_ROOT}/include)
set(CPPA_LIB_DIR ${CPPA_ROOT}/lib)  

# TODO: we should have a FindKaldi.cmake script which will find these.
set(KALDI_SRC_DIR ${KALDI_ROOT}/src)
set(KALDI_TOOLS_DIR ${KALDI_ROOT}/tools)

# TODO: we should look into kaldi/src/kaldi.mk to figure out ATLAS paths.
set(ATLAS_INCLUDE_DIR ${KALDI_TOOLS_DIR}/ATLAS/include)
# set(ATLAS_LIB_DIR ${KALDI_TOOLS_DIR}/ATLAS/build/lib)
set(ATLAS_LIB_DIR /usr/lib)

set(OPENFST_INCLUDE_DIR ${KALDI_TOOLS_DIR}/openfst/include)
set(OPENFST_LIB_DIR ${KALDI_TOOLS_DIR}/openfst/lib)

set(PORTAUDIO_INCLUDE_DIR ${KALDI_TOOLS_DIR}/portaudio/install/include)
set(PORTAUDIO_LIB_DIR ${KALDI_TOOLS_DIR}/portaudio/install/lib)


# Prevent falling back to system paths when using a custom Boost prefix.
if (BOOST_ROOT)
  set(Boost_NO_SYSTEM_PATHS true)
endif ()
find_package(Boost REQUIRED)

if (GRAPHVIZ_ROOT)
  set(ENV{GRAPHVIZ_ROOT} ${GRAPHVIZ_ROOT})
endif ()
find_package(Graphviz REQUIRED)


# By default, "make install" will make the RPATH of installed files empty.
# We need to add CPPA_LIB_DIR back to RPATH to avoid dynamic loading problems.
set(CMAKE_INSTALL_RPATH ${CPPA_LIB_DIR})


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
###############################################################################


set (CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -msse -msse2 -fPIC -Wall -DKALDI_DOUBLEPRECISION=0 -DHAVE_POSIX_MEMALIGN -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -Winit-self -Wno-unused-local-typedefs -Wno-sign-compare -flax-vector-conversions -fpermissive -g -std=c++11")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Typical flags for Linux
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_ATLAS -pthread")

  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Typical flags for Mac OS X
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_CLAPACK")

  set (CMAKE_EXE_LINKER_FLAGS 
    "${CMAKE_EXE_LINKER_FLAGS} -framework Accelerate -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Carbon -g")

else ()
  message(FATAL_ERROR "Barista does not support ${CMAKE_SYSTEM_NAME}.")
endif()

set_source_files_properties(src/decoder/faster-online-decoder.cc PROPERTIES COMPILE_FLAGS -O3)


###############################################################################
# Add targets                                                                 #
###############################################################################
# If you are on Mac OS X, you probably don't need to change anything beyond   #
# this point. If you are on Linux, you might have to change the extra libs    #
# added depending on your ATLAS and PORTAUDIO setup.                          #
###############################################################################

link_directories(${ATLAS_LIB_DIR} ${OPENFST_LIB_DIR} ${CPPA_LIB_DIR})

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
  cppa
  ${GRAPHVIZ_GVC_LIBRARY} ${GRAPHVIZ_CGRAPH_LIBRARY}
  ${KALDI_SRC_DIR}/online/kaldi-online.a  
  ${KALDI_SRC_DIR}/decoder/kaldi-decoder.a
  ${KALDI_SRC_DIR}/feat/kaldi-feat.a
  ${KALDI_SRC_DIR}/gmm/kaldi-gmm.a
  ${KALDI_SRC_DIR}/hmm/kaldi-hmm.a
  ${KALDI_SRC_DIR}/lat/kaldi-lat.a
  ${KALDI_SRC_DIR}/matrix/kaldi-matrix.a
  ${KALDI_SRC_DIR}/transform/kaldi-transform.a
  ${KALDI_SRC_DIR}/tree/kaldi-tree.a
  ${KALDI_SRC_DIR}/util/kaldi-util.a
  ${KALDI_SRC_DIR}/base/kaldi-base.a
  ${OPENFST_LIB_DIR}/libfst.a
  ${PORTAUDIO_LIB_DIR}/libportaudio.a
  dl m pthread
)


###############################################################################
# Set extra libraries needed for Linux systems                                #
###############################################################################
# How to set additional libraries:                                            #
# 1. Copy the libs listed under LDLIBS in KALDI_ROOT/src/kaldi.mk             #
# 2. Copy the libs listed under LIBS and DLL_LIBS in                          #
#    KALDI_ROOT/tools/portaudio/Makefile                                      #
###############################################################################

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  target_link_libraries(barista 
    lapack cblas atlas f77blas 
    rt #asound jack
  )
endif()


###############################################################################
# Installation Setup                                                          #
###############################################################################

# install targets
install(TARGETS 
  barista 
  barista-base barista-io barista-feat barista-decoder barista-gmm
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib  
  ARCHIVE DESTINATION lib
)

# install includes
install(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.h")

# process cmake_uninstall.cmake.in
configure_file("${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

# add uninstall target
add_custom_target(uninstall
                  COMMAND ${CMAKE_COMMAND} -P
                  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

