#!/bin/sh
set -e

# Requires libsodium!

PWD=$(pwd)
cd ../libs


# Now install ZMQPP
git clone -v --progress git://github.com/zeromq/zmqpp.git
cd zmqpp
make
make check
sudo make install
make installcheck

rm -rf ./zmqpp

sudo ldconfig
cd $PWD

exit 0
