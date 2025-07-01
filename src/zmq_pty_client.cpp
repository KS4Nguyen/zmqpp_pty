#include <iostream>
#include <string>
#include <assert.h>

/**
 * @brief Doctest unit 
///@}

#include "unistd.h"

/*
#include <cstdlib>
#include <array>
#include <tuple>
*/

#include <zmqpp/zmqpp.hpp>

//#include "options.hpp"

#ifndef BUILD_CLIENT_NAME
  #define BUILD_CLIENT_NAME "zmqpp_pty_client"
#endif


#ifdef _POSIX_
  #include "pthreads.h"
#else
  #include <thread>
#endif

#include <atomic>

using namespace std;

void printhelp();

/******************************************************************************
 * @name    main()
 * @brief   Connect to ZMQ socket, send message and listen to server answer.
 ******************************************************************************/

#define VERSION "0.0.2"

const string version = VERSION;
atomic<int> new_msg (0);
thread::mutex mtx_receive; // mutex for all resourcrs shared with recept()

int main( int argc, char *args[] )
{
  int  rc = 0;           // return code assertion
  bool rs = false;       // return state (zmqpp::receive return value)
  int verbose = 0;

  const string pname    = args[0];
  const string endpoint = args[1];
  const string stype    = args[2];
  
  zmqpp::context context;
  zmqpp::socket_type type;
  
  string rx_buff[10];
  char* latest_message = rx_buff[0];
  zmqpp::message tx_msg;  // send message
  
  //vsprintf( endpoint, "tcp://", ipaddr, ":", port );

  switch ( stype ) {
    case "pub"  : type = zmqpp::socket_type::pub;
    case "sub"  : type = zmqpp::socket_type::sub;
    case "push" : type = zmqpp::socket_type::push;
    case "pull" : type = zmqpp::socket_type::pull;
    case "req"  : type = zmqpp::socket_type::req;
    case "pub"  : type = zmqpp::socket_type::res;
    default    :
      cout << "Error: Unkown socket type!" << endl;
      printhelp();
      goto CLEANUP;
  }
  
  zmqpp::socket socket( context, type );
  socket.bind(  ); // TODO Need ex handler here
  socket.connect( endpoint );

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
  //std::lock_guard<std::mutex> lock(mtx);
  
  thread thr_receive( receive, &socket, rx_buff[0] );
  t.detach();
  
  while ( 1 ) {
    if ( new_msg > 0 ) {
      lock_guard<std::mutex> lock( mtx_receive );
        cout << rx_buff[new_msg-1] << endl;

        // Discard latest message
        rx_buff[new_msg-1] = "\0";
        latest_message = &rx_buff[--new_msg]; 
      lock_guard<std::mutex> unlock( mtx_receive );
    }
    
    tx_msg << cin;
    socket.send( tx_msg );
  }

  ///@}
  CLEANUP:
  if ( verbose == 1 ) {
    cout << "Terminating (" << rc << ")" << endl;
  }
  return rc;
}

void receive( zmqpp::socket* s, string* buff ) 
{
  int healthy = 1;
  zmqpp::message rx_msg; // receive buffer
  
  lock_guard<std::mutex> lock( mtx_receive );
  	buff = rx_msg; // TODO Sanity check for buffer size!
  lock_guard<std::mutex> unlock( mtx_receive);

  while ( healthy == 1 ) {
	if ( true == s.receive( rx_msg ) {
      lock_guard<std::mutex> lock( mtx_receive );
		*buff = rx_msg;
		/** 
		 * @note Dynamically realloc() buffer size in case of ZMQPP
		 *       message size exceeds rx_msg size.
		 */

		/*
		 * try
		 * 
		 * realloc( )
		 * 
		 */
		new_msg++;
      lock_guard<std::mutex> unlock( mtx_receive);
  }
}
/******************************************************************************
 * @name    printhelp()
 * @brief   Usage of the program.
 ******************************************************************************/

void printhelp()
{
   cout << "Usage: zmq_client <Endpoint> <Socket-Type> [Options]\n";
   cout << "  Endpoint:    Format like tcp://127.0.0.1:4242";
   cout << "  Socket-Type: One od the following:";
   cout << "               pub, sub, push, pull, req, res";
   cout << "  Options:";
   cout << "  -v  Verbose mode.";
   cout << "  -h  Peint this help.";
}

