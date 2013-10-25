#!/bin/bash
# This script attempts to install libcppa

njobs=${1:-1}

# Install instructions for libcppa

if ! which git >&/dev/null; then
   echo "This script requires you to first install git";
   exit 1;
fi

echo "**** Installing libcppa"

(
  git clone git://github.com/Neverlord/libcppa.git

  if [ ! -e libcppa ]; then
    echo "**** Download of libcppa failed"
    exit 1
  else
    cd libcppa
    patch --verbose -N -p0 < ../libcppa.patch
    ./configure --prefix=`pwd` --with-gcc=g++-4.8
    make -j $njobs
    make install
    cd ..
  fi
)

ok_libcppa=$?
if [ $ok_libcppa -eq 0 ]; then
  echo "**** libcppa install successful"
else
  echo "**** libcppa install failed"
fi

