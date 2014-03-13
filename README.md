# barista

Barista is an open-source framework for concurrent speech processing 
developed and maintained by the 
[Signal Analysis and Interpretation Lab](http://sail.usc.edu) (SAIL) at USC.

## Supported Operating Systems

* Linux
* Mac OS X

## Supported Compilers

* g++ >= 4.7 [g++-4.8 recommended]

We need a recent C++ compiler since barista relies on C++11 features. When 
C++11 support is activated, clang does not support linking with libstdc++. The 
only option is to link with libc++ which moves the stuff under `std::tr1` 
namespace to `std` namespace. Since Kaldi and OpenFst use some stuff from 
`std::tr1` namespace, they have to be linked with libstdc++. Hence, barista
can not link against Kaldi and OpenFst using clang with C++11 support. Support
for clang will be added if/when this issue is resolved.

Installing g++-4.8 on Ubuntu 12.04 LTS using apt-get:

    add-apt-repository ppa:ubuntu-toolchain-r/test
    apt-get update
    apt-get install g++-4.8

Installing g++-4.8 on Mac OS X using Homebrew:

    brew tap homebrew/versions
    brew update
    brew install gcc48

## Dependencies

Barista depends on [Kaldi](http://kaldi.sourceforge.net) (and its dependencies
including portaudio), [libcppa](https://github.com/Neverlord/libcppa), 
[GraphViz](http://www.graphviz.org) and [Boost](http://www.boost.org).

Before installing barista, make sure you have CMake, git, svn, aclocal,
libtoolize (or glibtoolize), zlib, libatlas, Boost and GraphViz (>= 2.30)
installed on your system. If you are missing any of these tools, you can use a
package manager to install them.

Installing missing packages on Ubuntu 12.04 LTS using apt-get:

    apt-get install build-essential git subversion cmake automake libtool
    apt-get install zlib1g-dev libatlas-dev libatlas-base-dev libboost-all-dev
    
    # This PPA has a recent GraphViz package (>=2.30) for Ubuntu 12.04
    sudo add-apt-repository ppa:xba/graphviz
    apt-get update
    # If you have problems installing GraphViz, see below
    apt-get install graphviz graphviz-dev

Installing missing packages on Mac OS X using Homebrew:

    brew install cmake automake libtool boost graphviz

We provide a Makefile for installing Kaldi, libcppa and GraphViz under
`barista/tools` directory (recommended). If you already have a recent version
of Kaldi trunk (revision >= 3755) installed at another location, you may skip
installing Kaldi and provide this location to barista configure script using
`--with-kaldi` option. Barista depends on the `kaldi-online` library and the
portaudio installation provided by Kaldi. If you choose to use an existing
Kaldi installation, make sure you have that library and portaudio installed.
Similarly, if you already have a libcppa V0.8.1 installation that was compiled
with g++ >= 4.7, you may skip installing libcppa and provide this location to
barista configure script using `--with-libcppa` option. If for some reason you
cannot install GraphViz (>=2.30) to a standard location using the system
package manager, you can install it under `barista/tools` directory using this
Makefile. If you do so, don't forget to add `--with-graphviz=tools/graphviz`
option when calling the barista configure script.

    cd barista
    make -C tools -j 4 kaldi                # 4 jobs will be run in parallel
    make -C tools -j 4 libcppa CXX=g++-4.8  # g++ >= 4.7 required for libcppa
    make -C tools -j 4 graphviz             # if not already installed

This Makefile uses checkmarks to avoid reinstalling things that were already
successfully installed in case one of the later installation steps fails. If
you want to rerun earlier steps, say for reinstalling with a different
compiler, don't forget to call the relevant clean target first. For instance,
for rebuilding Kaldi:

    make -C tools kaldi/clean
    make -C tools -j 4 kaldi

## Installation

If the following instructions fail for some reason, you might need to edit
`barista/CMakeLists.txt` to match your setup before building barista. You will
likely need to copy some flags from Kaldi makefiles. See
`barista/CMakeLists.txt` for details.

    ./configure --prefix=`pwd` --with-compiler=g++-4.8
    make -j 4
    make install

Above given configuration will build barista under `barista/build-g++-4.8` and
install everything under `barista` root directory. If you omit the `--prefix`
option, barista will be installed under `/usr/local/`. If you want to build
with a Kaldi or libcppa installation that is not under `barista/tools`, you
can use `--with-kaldi` and `--with-libcppa` options to specify custom paths.
Similarly, if you have Boost or GraphViz installed at a non-standard location,
you can use `--with-boost` and `--with-graphviz` options to specify those. By
default configure will search standard system folders for Boost and Graphviz.
Finally, if you have ATLAS installed at a non-standard location, you can use
`--with-atlas` option (Linux only). See `./configure --help` for details.

## Upgrade

To upgrade your barista installation:

    git pull                                # stop here if already up-to-date
    make -C tools -j 4 kaldi                # upgrade Kaldi if necessary
    make -C tools -j 4 libcppa CXX=g++-4.8  # upgrade libcppa if necessary
    ./configure --prefix=`pwd` --with-compiler=g++-4.8
    make -j 4
    make install

The calls for upgrading Kaldi and libcppa will not do anything unless they 
need to be upgraded to a newer version to support new barista functionality.
