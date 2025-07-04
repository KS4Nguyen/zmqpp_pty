#include <iostream>
#include <string>
#include <initializer_list> // for printv()

#include <assert.h>
//#include <algorithm> // Required for std::sort
#include "unistd.h"

#include <zmqpp/zmqpp.hpp>
//#include "options.hpp"

#ifndef BUILD_CLIENT_NAME
  #define BUILD_CLIENT_NAME "zmqpp_pty_client"
#endif

#include <thread>
#include <mutex>

#include <atomic>


using namespace std;

#define VERSION "0.0.2"
#define DEBUG 1


/******************************************************************************
 * @name    printhelp()
 * @brief   Usage of the program.
 ******************************************************************************/

void printhelp()
{

   cout << "Usage: zmq_client <Socket-Type> <Endpoint> [Options]\n";
   cout << "  Endpoint:    Format like tcp://127.0.0.1:4242";
   cout << "  Socket-Type: One od the following:";
   cout << "               pub, sub, push, pull, req, res";
   cout << "  Options:";
   cout << "  -v  Verbose mode.";
   cout << "  -h  Print this help.";
}


string pname = NULL;
bool verbose = false;

void printv( initializer_list<string> texts ) {
  if ( true == verbose ) {
    for (const auto& s : texts) {
      cout << s;
    }
    cout << '\n';
  }
}

/*
void printv() {} // Print verbose (varidic template)

template<typename T, typename ... more_text>

void printv( T first_text, more_text ... last_text ) {
  if ( true == verbose ) {
    cout << first_text;
    printv( last_text ... );
    cout << '\n';
  }
}
*/

/******************************************************************************
 * @name    receive()
 * @brief   Receive ZMQ message, thread.
 * @param   *s      Pointer to ZMQ socket, where to grab incomming messages.
 * @param   *buff   Pointer to messae buffer.
 ******************************************************************************/

///@{
atomic<int> new_msg (0);
mutex mtx_get_message; // mutex for all resourcrs shared with recept()

void get_message( zmqpp::socket *s, vector<string> *buff )
{
  zmqpp::message rx_msg;
  string *string_msg;
  string_msg = (string*)&rx_msg; // TODO Check if this is a safe type conversion.

  while ( NULL != s ) {
    if ( true == s->receive(rx_msg, false) ) { // TODO lock socket aka *s.
      mtx_get_message.lock();
      //lock_guard<std::mutex> lock( mtx_get_message );
        buff->push_back( *string_msg );
        new_msg++;
      mtx_get_message.unlock();
      //lock_guard<std::mutex> unlock( mtx_get_message);
    }
  }
}
 ///@}

/******************************************************************************
 * @name    main()
 * @brief   Connect to ZMQ socket, send message and listen to server answer.
 ******************************************************************************/

int main( int argc, char *args[] )
{
  int rc             = 0; // return code assertion
  verbose            = true;
  pname              = args[0];
  string endpoint    = "tcp://127.0.0.1:4242";
  string stype       = "push";

  if ( argc > 0 ) { stype = args[1]; }
  if ( argc > 1 ) { endpoint = args[2]; }
  
  zmqpp::context context;
  zmqpp::socket_type type;
  bool socket_initialized = false;
  
  vector<string> rx_buff;

  zmqpp::message tx_msg;  // send message
  string *tx_msg_string;
  tx_msg_string = (string*)&tx_msg;

  if ( stype == "pub"  ) { type = zmqpp::socket_type::pub; }  else
  if ( stype == "sub"  ) { type = zmqpp::socket_type::sub; }  else
  if ( stype == "push" ) { type = zmqpp::socket_type::push; } else
  if ( stype == "pull" ) { type = zmqpp::socket_type::pull; } else
  if ( stype == "req"  ) { type = zmqpp::socket_type::req; }  else
  //if ( stype == "res"  ) { type = zmqpp::socket_type::res; } else
  {
      cout << "Error: Invalid socket type!" << endl;
      printhelp();
      return ( -2 );
  }
  
  zmqpp::socket socket( context, type );
  socket.bind( endpoint );
  socket.connect( endpoint );
  socket_initialized = true;

  printv( {"Socket initialized at ", endpoint} );

  /****************************************************
   * @description    Compose a message from a string
   ****************************************************/

  ///@{

  //int number
  //rx_buff >> text >> number;

  ///@}

  /****************************************************
   * @description    Listen to socket till abort
   ****************************************************/

  ///@{

  std::thread thr_receive(
      get_message,      // void(*)(socket*, vector*)
      &socket,          // zmqpp::socket*
      &rx_buff          // std::vector<std::string>*
  );

  //thr_receive.detach();
  
  while ( 1 ) {
    if ( new_msg > 0 ) {
      if ( mtx_get_message.try_lock() ) {
        cout << rx_buff[new_msg-1] << endl;

        // Discard latest message
        rx_buff.pop_back();
        new_msg--;

        // TODO Make buffer a FIFO:
        //std::sort( rx_buff.begin(), rx_buff.end() );

        mtx_get_message.unlock();
      }
    }
    std::getline( cin, *tx_msg_string);
    socket.send( tx_msg );
  }
  ///@}

  if ( socket_initialized ) {
    printv( {"Closing socket at ", endpoint} );
    socket.close();
  }

  thr_receive.join();

  printv( {"Terminating (", to_string( rc )} );

  return rc;
}

/**
 * @TODO  Replace send() & receive() with raw data handling
 *

  bool zmqpp::socket::receive_raw  (   char *    buffer,
    size_t &    length,
    int const   flags = normal
  )

  bool zmqpp::socket::send_raw  (   char const *    buffer,
    size_t const    length,
    int const   flags = normal
  )

*/
