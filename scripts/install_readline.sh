#!/bin/sh

VERSION="8.2"
PNAME="readline"

pdir="$(pwd)"   ## returns to previous working directory
wdir=../libs    ## working directory of Readline build

cd $wdir
if [ 0 -ne $? ]; then
	echo "Error: Directory error. End."
	exit 1
fi

echo "Unpacking $PNAME-$VERSION.tar"

err=1
if [ -f $PNAME-$VERSION.tar ]; then
	tar -xf $PNAME-$VERSION.tar
	err=$?
fi

if [ $err -ne 0 ]; then
	echo "Error: Failed to extract Readline sources. Abort."
	cd $pdir
	exit 2
fi

mv "$PNAME"-master $PNAME-$VERSION
ln -s $PNAME-$VERSION $PNAME

cd $PNAME
./configure
if [ $? -ne 0 ]; then
	echo "Error: Failed to ./configure $PNAME. Abort."
	cd $pdir
	exit 1
fi

make all
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

cd ..
rm -rf ./$PNAME-$VERSION
rm ./$PNAME

cd $pdir

#sudo apt update
#sudo apt install -y readline-common libreadline-dev

exit 0

## EOF ##
