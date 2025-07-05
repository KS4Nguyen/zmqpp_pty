#!/bin/sh
set -e

make clean
make zmq_pty_server
make zmq_pty_client

./zmq_pty_server
./zmq_pty_client req tcp://127.0.0.1:4242
