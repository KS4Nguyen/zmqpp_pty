/**
 * \brief  ZMQPP Server with PTS in Linux filesysyem.
 */

#include <string>
#include <iostream>

#include <zmqpp/zmqpp.hpp>

#ifndef BUILD_CLIENT_NAME
  #define BUILD_CLIENT_NAME "zmqpp_pty_client"
#endif

#include <thread>
#include <mutex>

#include <atomic>

using namespace std;

int main(int argc, char *argv[]) {
  const string port  = args[1];
  const string stype = args[2];
   
  zmqpp::socket_type type;
  bool socket_initialized = false;
   bool verbose = 1;
  
  vector<string> rx_buff;

  zmqpp::message rx_msg;  // send message
  string *rx_msg_string;
  rx_msg_string = (string*)&rx_msg;
   
   const string endpoint = "tcp://*:" + port;
   
  if ( stype == "pub"  ) { type = zmqpp::socket_type::pub; }  else
  if ( stype == "sub"  ) { type = zmqpp::socket_type::sub; }  else
  if ( stype == "push" ) { type = zmqpp::socket_type::push; } else
  if ( stype == "pull" ) { type = zmqpp::socket_type::pull; } else
  if ( stype == "req"  ) { type = zmqpp::socket_type::req; }  else
  //if ( stype == "res"  ) { type = zmqpp::socket_type::res; } else
  {
      cout << "Error: Unknown socket type!" << endl;
      cout << "Terminated.";
      return ( -2 );
  }
   
  // initialize the 0MQ context
  zmqpp::context context;

  // generate a pull socket
  zmqpp::socket socket (context, type);

  // bind to the socket
  cout << "Binding to " << endpoint << "..." << endl;
  socket.bind(endpoint);
  zmqpp::message message;
   
  // receive the message
  while (1) {
     // decompose the message
     socket.receive(message);
     cout << *rx_msg_string << endl;
  }
  if ( verbose )
   cout << args[0] << ": Terminated." << endl;
}
