#!/bin/bash
# This script attempts to install graphviz

# (1) Install instructions for graphviz

if ! which wget >&/dev/null; then
   echo "This script requires you to first install wget";
   exit 1;
fi

echo "****(1) Installing graphviz"

(
  wget http://www.graphviz.org/pub/graphviz/stable/SOURCES/graphviz-2.30.1.tar.gz

  if [ ! -e graphviz-2.30.1.tar.gz ]; then
    echo "****download of graphviz failed."
    exit 1
  else
    tar xzf graphviz-2.30.1.tar.gz
    ln -s graphviz-2.30.1 graphviz
    cd graphviz
    ./configure --prefix=`pwd` --without-x CC=gcc-4.8 CXX=g++-4.8
    make -j 8
    make install
    cd ..
  fi
)
ok_graphviz=$?
if [ $ok_graphviz -eq 0 ]; then
  echo "graphviz install successful."
else
  echo "****graphviz install failed."
fi
