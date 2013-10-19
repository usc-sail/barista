barista
=======

Barista is an open-source framework for concurrent speech processing from 
USC-SAIL.

Sexy minions are working 24/7 to bring you the preview release of Barista.

Supported Operating Systems
---------------------------

* Linux
* Mac OS X

Supported Compilers
-------------------

Barista relies on C++11 features, hence we need a recent C++ compiler.
  
* GCC >= 4.7 [GCC-4.8 recommended]
* Clang >= 3.2 [Not tested yet.]

Dependencies
------------

Barista depends on Kaldi (and its dependencies including portaudio), libcppa, 
GraphViz, Boost and pthread (on Linux). We also need CMake for installation. 
Rudimentary scripts for installing barista dependencies (except Boost, pthread 
and CMake) are provided under the tools directory. 
Note that these scripts assume you have GCC-4.8 installed on your machine.
We strongly recommend installing Barista dependencies using the provided 
scripts (especially the one for installing kaldi) along with GCC-4.8.

* cd barista/tools
* ./install_kaldi.sh
* ./install_libcppa.sh
* ./install_graphviz.sh

Installation
------------

Unfortunately Barista build process is still semi-automatic. 
You might need to edit barista/src/CMakeLists.txt file to match your setup 
before building Barista.

* mkdir barista/build
* cd barista/build
* cmake ../src
* make
