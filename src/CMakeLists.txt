cmake_minimum_required (VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE 0)

# Set the compiler here before the project command. This way CMake uses the
# correct compiler at configuration time.
# libcppa requires GCC >= 4.7 or Clang >= 3.2. Not tested with Clang yet.
set (CMAKE_CXX_COMPILER "g++-4.8")

# This call will use the above set compiler at configuration time.
# We set compiler/linker flags afterwards since this call overwrites them.
project (Barista CXX)


######################
# Set compiler flags #
######################

# 1. Copy the flags set by CXXFLAGS in KALDI_ROOT/src/kaldi.mk
# 2. Add "-std=c++11 -fpermissive"
# 3. Add "-Wno-unused-local-typedefs" to silence the warnings about OpenFst.

###### Typical flags for Linux
IF(CMAKE_SYSTEM_NAME STREQUAL Linux)
  set (CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -msse -msse2 -fPIC -Wall -Wno-sign-compare -Winit-self -DKALDI_DOUBLEPRECISION=0 -DHAVE_POSIX_MEMALIGN -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_ATLAS -g -std=c++11 -fpermissive -Wno-unused-local-typedefs")
ENDIF()

###### Typical flags for Mac OS X
IF(APPLE)
  set (CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -msse -msse2 -fPIC -Wall -Wno-sign-compare -Winit-self -DKALDI_DOUBLEPRECISION=0 -DHAVE_POSIX_MEMALIGN -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_CLAPACK -flax-vector-conversions -g -std=c++11 -fpermissive -Wno-unused-local-typedefs")
ENDIF()

set_source_files_properties(decoder/faster-online-decoder.cc PROPERTIES COMPILE_FLAGS -O3)

####################
# Set linker flags #
####################

# 1. Copy the flags set by LDFLAGS and LDLIBS in KALDI_ROOT/src/kaldi.mk
# 2. Copy the frameworks set by SHARED_FLAGS in KALDI_ROOT/tools/portaudio/Makefile

###### Typical flags for Linux
IF(CMAKE_SYSTEM_NAME STREQUAL Linux)
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
ENDIF()

###### Typical flags for Mac OS 
IF(APPLE)
  set (CMAKE_EXE_LINKER_FLAGS 
    "${CMAKE_EXE_LINKER_FLAGS} -framework Accelerate -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Carbon -g")
ENDIF()


#############
# Set paths #
#############

set(BARISTA_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Change the following path settings according to your local setup
set(CPPA_INCLUDE ${BARISTA_ROOT}/tools/libcppa/include)
set(CPPA_LIB ${BARISTA_ROOT}/tools/libcppa/lib)

set(GRAPHVIZ_INCLUDE ${BARISTA_ROOT}/tools/graphviz/include/graphviz)
set(GRAPHVIZ_LIB ${BARISTA_ROOT}/tools/graphviz/lib)

set(KALDI_ROOT ${BARISTA_ROOT}/tools/kaldi)
set(KALDI_SRC ${KALDI_ROOT}/src)

set(ATLAS_INCLUDE ${KALDI_ROOT}/tools/ATLAS/include)
# set(ATLAS_LIB ${KALDI_ROOT}/tools/ATLAS/build/lib)
set(ATLAS_LIB /usr/lib)

set(OPEN_FST_INCLUDE ${KALDI_ROOT}/tools/openfst/include)
set(OPEN_FST_LIB ${KALDI_ROOT}/tools/openfst/lib)

set(PORT_AUDIO_INCLUDE ${KALDI_ROOT}/tools/portaudio/install/include)
set(PORTAUDIO_LIB ${KALDI_ROOT}/tools/portaudio/install/lib)

##############################################################################
# If you are on Mac OS X, you probably don't need to change anything beyond  #
# this point. If you are on Linux, you might have to change the extra libs   #
# added at the bottom of this file depending on your setup.                  #
##############################################################################

link_directories(
  ${ATLAS_LIB} ${CPPA_LIB} ${GRAPHVIZ_LIB} ${OPEN_FST_LIB} ${PORTAUDIO_LIB}
)

include_directories(
  . ${KALDI_SRC} ${ATLAS_INCLUDE} ${CPPA_INCLUDE} ${GRAPHVIZ_INCLUDE} 
  ${OPEN_FST_INCLUDE} ${PORT_AUDIO_INCLUDE}
)

add_library(barista-base STATIC 
  base/module-base.cc
)

add_library(barista-io STATIC 
  io/file-list-reader.cc io/pcm-reader.cc
  io/command-line-interface.cc io/portaudio-reader.cc
  io/vector-writer.cc io/matrix-writer.cc
)

add_library(barista-feat STATIC 
  feat/compute-mfcc-feats.cc feat/apply-cmvn.cc feat/add-deltas.cc 
)

add_library(barista-decoder STATIC 
  decoder/faster-online-decoder.cc
)

add_library(barista-gmm STATIC 
  gmm/gmm-decode-faster-online.cc
)

add_executable(barista bin/barista.cc)

target_link_libraries(barista 
  barista-base barista-io barista-feat barista-decoder barista-gmm
  ${KALDI_SRC}/online/kaldi-online.a  
  ${KALDI_SRC}/decoder/kaldi-decoder.a
  ${KALDI_SRC}/feat/kaldi-feat.a
  ${KALDI_SRC}/gmm/kaldi-gmm.a
  ${KALDI_SRC}/hmm/kaldi-hmm.a
  ${KALDI_SRC}/lat/kaldi-lat.a
  ${KALDI_SRC}/matrix/kaldi-matrix.a
  ${KALDI_SRC}/transform/kaldi-transform.a
  ${KALDI_SRC}/tree/kaldi-tree.a
  ${KALDI_SRC}/util/kaldi-util.a
  ${KALDI_SRC}/base/kaldi-base.a
  ${OPEN_FST_LIB}/libfst.a
  dl m pthread
  cppa gvc cgraph
  ${PORTAUDIO_LIB}/libportaudio.a
)

################################################
# Set extra libraries needed for Linux systems #
################################################

# 1. Copy the libs listed under LDLIBS in KALDI_ROOT/src/kaldi.mk 
# 2. Copy the libs listed under LIBS and DLL_LIBS in KALDI_ROOT/tools/portaudio/Makefile

IF(CMAKE_SYSTEM_NAME STREQUAL Linux)
  target_link_libraries(barista 
    lapack cblas atlas f77blas 
    rt #asound jack
  )
ENDIF()
