#!/bin/bash
# This script attempts to install Kaldi

njobs=${1:-1}

# Install instructions for Kaldi

if ! which svn >&/dev/null; then
   echo "This script requires you to first install svn";
   exit 1;
fi

echo "**** Installing Kaldi"

(
  svn co svn://svn.code.sf.net/p/kaldi/code/trunk kaldi
  if [ ! -e kaldi ]; then
    echo "**** Download of Kaldi failed"
    exit 1
  else
    cd kaldi/tools
    cp ../../*.patch* .
    patch --verbose -N -p0 < makefile.patch
    make -j $njobs
    patch --verbose -N -p0 < install_portaudio.patch
    ./install_portaudio.sh
    cd ../src
    ./configure
    sed -i.bk 's/\-rdynamic//g; s/g++/g++-4.8/g;' kaldi.mk
    grep -ilr 'make_pair<' * | xargs -I@ sed -i.bk 's/make_pair\</pair</g' @
    make -j $njobs depend
    make -j $njobs
    make -j $njobs online
    cd ../..
  fi
)
ok_kaldi=$?
if [ $ok_kaldi -eq 0 ]; then
  echo "**** Kaldi install successful"
else
  echo "**** Kaldi install failed"
fi
