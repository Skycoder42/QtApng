#!/bin/sh
# $1 libpng version
set -e

pngver=${1:?You need to specify a libpng version, e.g. 1.6.31}

prefixdir=$(pwd)/libpng

pushd $(mktemp -d)

# download libpng
wget "https://downloads.sourceforge.net/sourceforge/libpng/libpng-${pngver}.tar.xz"
tar xf "libpng-${pngver}.tar.xz"

# download apng patch
wget "https://downloads.sourceforge.net/sourceforge/libpng-apng/libpng-${pngver}-apng.patch.gz"
gunzip "libpng-${pngver}-apng.patch.gz"

# apply apng patch
cd libpng-${pngver}
patch -Np1 -i "../libpng-${pngver}-apng.patch"

export CPPFLAGS=-fPIC $CPPFLAGS
./configure --enable-static --disable-shared --prefix="$prefixdir"
make check
make install

popd

echo "CONFIG += libpng_static" >> .qmake.conf
