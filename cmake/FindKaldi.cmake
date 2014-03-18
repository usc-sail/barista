# - Try to find Kaldi
# Once done this will define
#
#  KALDI_FOUND      - system has Kaldi
#  KALDI_SRC_DIR    - Kaldi src directory
#  KALDI_TOOLS_DIR  - Kaldi tools directory
#  KALDI_LIBRARIES  - link these to use Kaldi
#  KALDI_VERSION    - Kaldi revision number

if (KALDI_SRC_DIR AND KALDI_TOOLS_DIR AND KALDI_LIBRARIES)
  set(KALDI_FOUND TRUE)
else (KALDI_SRC_DIR AND KALDI_TOOLS_DIR AND KALDI_LIBRARIES)

  find_path(KALDI_SRC_DIR base/kaldi-common.h ${KALDI_ROOT}/src)
  find_path(KALDI_TOOLS_DIR install_portaudio.sh ${KALDI_ROOT}/tools)
  
  if (KALDI_SRC_DIR AND KALDI_TOOLS_DIR)
    if (NOT Kaldi_FIND_QUIETLY)
      message (STATUS "Kaldi root: ${KALDI_ROOT}")
    endif (NOT Kaldi_FIND_QUIETLY)
    
    if(IS_DIRECTORY ${KALDI_ROOT}/.svn)
      execute_process(COMMAND svnversion -n ${KALDI_ROOT}
                      OUTPUT_VARIABLE KALDI_VERSION)
      message(STATUS "Kaldi revision: ${KALDI_VERSION}")
    else ()
      message("Kaldi root is not an svn checkout. Kaldi revision unknown.")
    endif ()
    
  else (KALDI_SRC_DIR AND KALDI_TOOLS_DIR)
    message ("Could not find Kaldi root at ${KALDI_ROOT}")
  endif (KALDI_SRC_DIR AND KALDI_TOOLS_DIR)
  
  if (KALDI_SRC_DIR)
    
    find_library(KALDI_BASE_LIBRARY base/kaldi-base.a ${KALDI_SRC_DIR})
    find_library(KALDI_DECODER_LIBRARY decoder/kaldi-decoder.a ${KALDI_SRC_DIR})
    find_library(KALDI_FEAT_LIBRARY feat/kaldi-feat.a ${KALDI_SRC_DIR})
    find_library(KALDI_GMM_LIBRARY gmm/kaldi-gmm.a ${KALDI_SRC_DIR})
    find_library(KALDI_HMM_LIBRARY hmm/kaldi-hmm.a ${KALDI_SRC_DIR})
    find_library(KALDI_LAT_LIBRARY lat/kaldi-lat.a ${KALDI_SRC_DIR})
    find_library(KALDI_MATRIX_LIBRARY matrix/kaldi-matrix.a ${KALDI_SRC_DIR})
    find_library(KALDI_ONLINE_LIBRARY online/kaldi-online.a ${KALDI_SRC_DIR})
    find_library(KALDI_TRANSFORM_LIBRARY transform/kaldi-transform.a ${KALDI_SRC_DIR})
    find_library(KALDI_TREE_LIBRARY tree/kaldi-tree.a ${KALDI_SRC_DIR})
    find_library(KALDI_UTIL_LIBRARY util/kaldi-util.a ${KALDI_SRC_DIR})
    
    foreach(LIBNAME KALDI_BASE_LIBRARY KALDI_DECODER_LIBRARY KALDI_FEAT_LIBRARY KALDI_GMM_LIBRARY KALDI_HMM_LIBRARY KALDI_LAT_LIBRARY KALDI_MATRIX_LIBRARY KALDI_ONLINE_LIBRARY KALDI_TRANSFORM_LIBRARY KALDI_TREE_LIBRARY KALDI_UTIL_LIBRARY)
      if(${LIBNAME})
        SET(KALDI_LIBRARIES ${KALDI_LIBRARIES} ${${LIBNAME}} )
      else(${LIBNAME})
        message("${LIBNAME} not found.")
        set(KALDI_MISSING_LIBRARY TRUE)
      endif (${LIBNAME})
    endforeach(LIBNAME)
    
    if (KALDI_LIBRARIES)
      if (NOT Kaldi_FIND_QUIETLY)
        message (STATUS "Kaldi libraries: ${KALDI_LIBRARIES}")
      endif (NOT Kaldi_FIND_QUIETLY)
    endif (KALDI_LIBRARIES)
    
  endif(KALDI_SRC_DIR)
  
  if (KALDI_TOOLS_DIR)
    
    find_path(OPENFST_INCLUDE_DIR fst/fstlib.h ${KALDI_TOOLS_DIR}/openfst/include)
    if (OPENFST_INCLUDE_DIR)
      if (NOT Kaldi_FIND_QUIETLY)
        message(STATUS "OpenFst include: ${OPENFST_INCLUDE_DIR}")
      endif (NOT Kaldi_FIND_QUIETLY)
    else (OPENFST_INCLUDE_DIR)
      message ("Openfst header files not found at ${KALDI_TOOLS_DIR}/openfst/include")
    endif (OPENFST_INCLUDE_DIR)
    
    find_library(OPENFST_LIBRARY NAMES libfst.a PATHS ${KALDI_TOOLS_DIR}/openfst/lib)
    if (OPENFST_LIBRARY)
      if (NOT Kaldi_FIND_QUIETLY)
        message(STATUS "OpenFst library: ${OPENFST_LIBRARY}")
      endif (NOT Kaldi_FIND_QUIETLY)
    else (OPENFST_LIBRARY)
      message ("Openfst library not found at ${KALDI_TOOLS_DIR}/openfst/lib")
    endif (OPENFST_LIBRARY)
    
    find_path(PORTAUDIO_INCLUDE_DIR portaudio.h ${KALDI_TOOLS_DIR}/portaudio/install/include)
    if (PORTAUDIO_INCLUDE_DIR)
      if (NOT Kaldi_FIND_QUIETLY)
        message(STATUS "PortAudio include: ${PORTAUDIO_INCLUDE_DIR}")
      endif (NOT Kaldi_FIND_QUIETLY)
    else (PORTAUDIO_INCLUDE_DIR)
      message ("Portaudio header files not found at ${KALDI_TOOLS_DIR}/portaudio/install/include")
    endif (PORTAUDIO_INCLUDE_DIR)
    
    find_library(PORTAUDIO_LIBRARY NAMES libportaudio.a PATHS ${KALDI_TOOLS_DIR}/portaudio/install/lib)
    if (PORTAUDIO_LIBRARY)
      if (NOT Kaldi_FIND_QUIETLY)
        message(STATUS "PortAudio library: ${PORTAUDIO_LIBRARY}")
      endif (NOT Kaldi_FIND_QUIETLY)
    else (PORTAUDIO_LIBRARY)
      message ("PortAudio library not found at ${KALDI_TOOLS_DIR}/portaudio/install/lib")
    endif (PORTAUDIO_LIBRARY)
    
    find_file(PORTAUDIO_LIBTOOL_FILE NAMES libportaudio.la PATHS ${KALDI_TOOLS_DIR}/portaudio/install/lib)
    if (PORTAUDIO_LIBTOOL_FILE)
      if (NOT Kaldi_FIND_QUIETLY)
        message(STATUS "PortAudio libtool file: ${PORTAUDIO_LIBTOOL_FILE}")
      endif (NOT Kaldi_FIND_QUIETLY)
    else (PORTAUDIO_LIBTOOL_FILE)
      message ("PortAudio libtool (.la) file not found at ${KALDI_TOOLS_DIR}/portaudio/install/lib")
    endif (PORTAUDIO_LIBTOOL_FILE)
    
    if (OPENFST_INCLUDE_DIR AND OPENFST_LIBRARY AND 
        PORTAUDIO_INCLUDE_DIR AND PORTAUDIO_LIBRARY AND PORTAUDIO_LIBTOOL_FILE)
      set(KALDI_DEPENDENCIES_FOUND TRUE)
    endif()
    
  endif (KALDI_TOOLS_DIR)
  
  if (KALDI_SRC_DIR AND KALDI_TOOLS_DIR AND NOT KALDI_MISSING_LIBRARY
      AND KALDI_DEPENDENCIES_FOUND)
    set(KALDI_FOUND TRUE)
  endif ()

endif (KALDI_SRC_DIR AND KALDI_TOOLS_DIR AND KALDI_LIBRARIES)

if(Kaldi_FIND_REQUIRED AND NOT KALDI_SRC_DIR AND NOT KALDI_TOOLS_DIR)
  message(FATAL_ERROR "Could not find Kaldi.")
endif()

if(Kaldi_FIND_REQUIRED AND KALDI_MISSING_LIBRARY)
  message(FATAL_ERROR "Could not find some of the required Kaldi libraries.")
endif()

if(Kaldi_FIND_REQUIRED AND NOT KALDI_DEPENDENCIES_FOUND)
  message(FATAL_ERROR "Could not find some of the required Kaldi dependencies.")
endif()
