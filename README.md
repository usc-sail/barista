# barista

Barista is an open-source framework for concurrent speech processing 
developed and maintained by the 
[Signal Analysis and Interpretation Lab](http://sail.usc.edu) (SAIL) at USC.

## Supported Operating Systems

* Linux
* Mac OS X

## Supported Compilers

We need a recent C++ compiler since barista relies on C++11 features.
  
* G++ >= 4.7 [G++-4.8 recommended]
* Clang >= 3.2 [Not tested]

Here are the instructions for installing G++-4.8 on Ubuntu 12.04 LTS.

    add-apt-repository ppa:ubuntu-toolchain-r/test
    apt-get update
    apt-get install g++-4.8

## Dependencies

Barista depends on [Kaldi](http://kaldi.sourceforge.net) (and its dependencies
including portaudio), [libcppa](https://github.com/Neverlord/libcppa), 
[GraphViz](http://www.graphviz.org) and [Boost](http://www.boost.org).

Before installing barista, make sure you have CMake, git, svn, aclocal, 
libtoolize (or glibtoolize), zlib, libatlas and Boost installed on your 
system. If you are missing any of these tools, you can use a package manager to
install them. Here are the instructions for installing these packages on
Ubuntu 12.04 LTS.

    apt-get install build-essential git subversion cmake automake libtool
    apt-get install zlib1g-dev libboost-all-dev libatlas-dev libatlas-base-dev 

Rudimentary scripts for installing Kaldi, libcppa and GraphViz are provided 
under the tools directory. Note that these scripts assume you have G++-4.8 
installed on your machine.

    cd barista/tools
    ./install_kaldi.sh 4      # 4 jobs will be run in parallel (make -j 4) 
    ./install_libcppa.sh      # if not specified, number of jobs defaults to 1
    ./install_graphviz.sh

## Installation

Unfortunately Barista build process is still semi-automatic. If the following
instructions fail for some reason, you might need to edit 
`barista/src/CMakeLists.txt` to match your setup before building barista. 
You will likely need to copy some flags from Kaldi installation scripts. See 
`barista/src/CMakeLists.txt` for details.

    mkdir barista/build
    cd barista/build
    cmake ../src
    make
