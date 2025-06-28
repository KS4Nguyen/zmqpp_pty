 #!/bin/sh

LIBSODIUM_VER="1.0.18"
PWD="$(pwd)"

cd ../libs/
tar -xf libsodium-$LIBSODIUM_VER.tar

if [ $? -ne 0 ]; then
    echo "Error: Cannot find libsodium-$LIBSODIUM_VER. Abort."
    exit -1
fi

cd libsodium-$LIBSODIUM_VER/

./autogen.sh 
./configure 
make check
sudo make install 
sudo ldconfig

cd $PWD
exit 0
