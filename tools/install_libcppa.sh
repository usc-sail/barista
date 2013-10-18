#!/bin/bash
# This script attempts to install libcppa

# (1) Install instructions for libcppa

if ! which git >&/dev/null; then
   echo "This script requires you to first install git";
   exit 1;
fi

echo "****(1) Installing libcppa"

(
  git clone git://github.com/Neverlord/libcppa.git

  if [ ! -e libcppa ]; then
    echo "****download of libcppa failed."
    exit 1
  else
    cd libcppa
    ./configure --prefix=`pwd` --with-gcc=g++-4.8
    make -j 8
    make install
    cd ..
  fi
)

ok_libcppa=$?
if [ $ok_libcppa -eq 0 ]; then
  echo "libcppa install successful."
else
  echo "****libcppa install failed."
fi

