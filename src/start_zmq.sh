#!/bin/sh

#set -e

make clean
make zmq_pty_server
make zmq_pty_client

./zmq_pty_server pull tcp://127.0.0.1:4242 &
#./zmq_pty_client push tcp://127.0.0.1:4242

echo $?
