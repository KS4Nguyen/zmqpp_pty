
#!/bin/sh

pdir="$(pwd)"   ## returns to previous working directory
wdir=../libs    ## working directory of libZMQ build

VERSION="4.3.5" ## fallback version
md5="ae933b1e98411fd7cb8309f9502d2737  zeromq-4.3.5.tar.gz"

cd $wdir
if [ 0 -ne $? ]; then
	echo "Error: Directory error. End."
	exit 1
fi

wget https://github.com/zeromq/libzmq/releases/download/v$VERSION/zeromq-$VERSION.tar.gz
if [ 0 -ne $? ]; then
	echo "Cannot get libZMQ. Trying fallback..."
	VERSION="4.3.5"
fi

echo "Unpacking zeromq-$VERSION.tar.gz"
if [ "$(md5sum zeromq-$VERSION.tar.gz)" != "$md5" ]; then
	echo "Error: MD5 checksum mismatch. Abort."
	cd $pdir
	exit 1
fi

err=1
if [ -f zeromq-$VERSION.tar.gz ]; then
	gunzip zeromq-$VERSION.tar.gz
	err=$?
fi

if [ -f zeromq-$VERSION.tar ]; then
	tar -xf zeromq-$VERSION.tar
	err=$?
fi

if [ $err -ne 0 ]; then
	echo "Error: Failed to extract libZMQ sources. Abort."
	cd $pdir
	exit 2
fi

ln -s zeromq-$VERSION zeromq

cd zeromq

./configure --with-libsodium && make
if [ $? -ne 0 ]; then
        echo "Error: Failed to make libZMQ. Abort."
        cd $pdir
	exit 1
fi

sudo make install
if [ $? -ne 0 ]; then
        echo "Error: Failed to install libZMQ. End."
        cd $pdir
	exit 1
fi

cd ..

sudo ldconfig

rm -rf ./zeromq-$VERSION
rm zeromq

cd $pdir
exit 0

# Git the latest zmq from:
#	git clone https://github.com/zeromq/libzmq.git

## EOF ##
