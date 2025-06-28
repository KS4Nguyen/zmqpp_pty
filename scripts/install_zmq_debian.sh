#!/bin/sh
err = 0

echo "deb http://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/ ./" >> /etc/apt/sources.list
err = $?

wget https://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/Release.key -O- | sudo apt-key add
err = $?

sudo apt update
sudo apt install -y build-essential
sudo apt install -y libzmq3-dev

sudo ldconfig

exit $err
