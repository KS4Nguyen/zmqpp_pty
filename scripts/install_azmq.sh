#!/bin/sh

pdir="$(pwd)"   ## returns to previous working directory
wdir=../libs    ## working directory of libZMQ build

PNAME="azmq"
VERSION="1.0.3"

cd $wdir
if [ 0 -ne $? ]; then
	echo "Error: Directory error. End."
	exit 1
fi

err=1
if [ -f $PNAME-$VERSION.tar ]; then
	tar -xf $PNAME-$VERSION.tar
	err=$?
fi

if [ $err -ne 0 ]; then
	echo "Error: Failed to extract azmq sources. Abort."
	cd $pdir
	exit 2
fi

ln -s $PNAME-$VERSION $PNAME

cd $PNAME
mkdir build && cd build
cmake ..
make && make test
if [ $? -ne 0 ]; then
        echo "Error: Failed to make $PNAME. Abort."
        cd $pdir
	exit 1
fi

sudo make install
if [ $? -ne 0 ]; then
        echo "Error: Failed to install $PNAME. End."
        cd $pdir
	exit 1
fi

cd ../../
rm -rf ./$PNAME/
rm -rf ./$PNAME-$VERSION/

cd $pdir
exit 0

## EOF ##
