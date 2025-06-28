#include <zmqpp/zmqpp.hpp>
#include <zmq.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <thread>

#include <unistd.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  const std::string endpoint = "tcp://*:5555";
  int major, minor, patch;
  std::string zmqver;
  
  zmq_version (&major, &minor, &patch);
  zmqver = ToString( major ) + "." + ToString( minor ) + "." + ToString( patch );
  //sprintf (zmqver, "%d.%d.%d", major, minor, patch);
  
  printf ("0MQ version: %d.%d.%d\n", major, minor, patch);
  // initialize the 0MQ context
  zmqpp::context context;

  // generate a pull socket
  zmqpp::socket_type type = zmqpp::socket_type::reply;
  zmqpp::socket socket (context, type);

  // bind to the socket
  socket.bind(endpoint);
  std::string text;

  while (1) {
    // receive the message
    zmqpp::message message;
    // decompose the message 
    socket.receive(message);

    message >> text;

    //Do some 'work'
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Received Hello" << std::endl;
    socket.send("World");
  }

}
