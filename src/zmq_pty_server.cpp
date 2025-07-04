#include <cstdio>
#include <iostream>
#include <string>
#include <chrono>

//#include <unistd.h>

#include <zmqpp/zmqpp.hpp>
#include <assert.h>

#include <thread>
//#include "pthreads.h"

//#include "common.h"

#define VERSION "0.1"

using namespace std;

const string version = VERSION;

int main(int argc, char *argv[]) {
  const string endpoint = "tcp://*:4242";
  int major, minor, patch;
  string zmqver;
  
  zmq_version (&major, &minor, &patch);

/*
#ifdef _COMMON_HELPERS
  zmqver = concat( itoc( sizeof( long int ), (long int *)major ), ".", \
                   itoc( sizeof( long int ), (long int *)minor ), ".", \
                   itoc( sizeof( long int ), (long int *)patch ) \
                 );
#else
  #ifdef _STRING_H
  zmqver = to_char(mayor) + "." + to_char(minor) + "." + to_char(patch);
  #else
  zmqver = 0.0.0;
  #endif
#endif
*/
  
  printf ("0MQ version: %d.%d.%d\n", major, minor, patch);

  // initialize the 0MQ context
  zmqpp::context context;

  // generate a pull socket
  zmqpp::socket_type type = zmqpp::socket_type::reply;
  zmqpp::socket socket (context, type);

  // bind to the socket
  socket.bind(endpoint);
  string text;

  while (1) {
    // receive the message
    zmqpp::message message;
    // decompose the message 
    socket.receive(message);

    message >> text;

    //Do some 'work'
    this_thread::sleep_for(chrono::seconds(1));
    cout << "Received Hello" << endl;
    socket.send("World");
  }

}
