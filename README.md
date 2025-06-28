# Introduction

This project is a fork from Github ZMQPP which provides C++ library bindings for
libzmq:

	(ref. A) https://github.com/zeromq/zmqpp

In addition the **zmq_client** and **zmq_server** generate a PTY device in the
Linux userspace:

	/dev/ttyz0
	/dev/ttyz1

## Command-Line Client

The client is built on top of the libzmqpp bindings.


## Command-Line Server

<TODO>


# Installation

C++11 compliant compiler is needed (g++ >= 4.7).

You can install the additional requirements with the installation scripts
located at:

	scripts/

Install dependencies:

	./install_libboost.sh
	./install_zmq_with_libsodium.sh

Install:

	make
	make install

"If the boost unittest framework is installed, check and installcheck can be run
for sanity checking. " (ref. A)

	make check
	make installcheck


## libzmqpp

__ZMQPP__
"The install process will only install headers and the shared object to the
system. The archive will remain in the build directory." (ref. A)

__ZMQ__
Will be installed to:

	/usr/local/lib


# Documentation

"Most of the code is now commented with doxygen style tags, and a basic
configuration file to generate them is in the root directory.

To build the documentation with doxygen use:

	doxygen -u
	doxygen

And the resulting html [...]" can be found at

	docs/html/index.html


## Development Notes

1) Update with latest changes from ZMQPP:

	./sync_fork.h

"
  +-------+
  | ZMQPP |
  +-------+
      |
      | git checkout upstream/master
      |
      v 
+-----------+
| ZMQPP_PTY |
|  (local)  |
+-----------+
      |
      | git add .
      | git commit "<change note>
      | git merge upstream/master
      | git push origin
      |
      v
+-----------+
| ZMQPP_PTY |
| (origin)  |
+-----------+

"

2) Library / Bindings

"This C++ binding for 0mq/zmq is a 'high-level' library that hides most of the
c-style interface core 0mq provides. It consists of a number of header and
source files all residing in the zmq directory, these files are provided under
the MPLv2 license (see LICENSE for details).

They can either be included directly into any 0mq using project or used as a
library. A really basic Makefile is provided for this purpose and will generate
both shared and static libraries. [...]" (ref. A)


3) Linking

"If you ever happen to want to link against installed libraries in a give
directory, LIBDIR, you must either use libtool, and specify the full pathname of
the library, or use the '-LLIBDIR' flag during linking and do at least one of
the following:

	- add LIBDIR to the 'LD_LIBRARY_PATH' environment variable during execution
	- add LIBDIR to the 'LD_RUN_PATH' environment variable during linking
	- use the '-Wl,-rpath -Wl,LIBDIR' linker flag
	- have your system administrator add LIBDIR to '/etc/ld.so.conf'

See any operating system documentation about shared libraries for more
information, such as the ld(1) and ld.so(8) manual pages."


# Licensing

I wish to use the GPL-v3.0 for this Fork, but I am not sure if I am allowed to
do so because the original ZMQPP project was released under MPLv2.

So for now, both the library and the associated command line client are also
released under the

MPLv2 license.

Please see LICENSE for full details.
