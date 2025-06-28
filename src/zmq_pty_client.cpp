#include <iostream>
#include <string>
#include <chrono>

#include <assert.h>

#include <zmqpp/zmqpp.hpp>

#include "unistd.h"

/*
#ifdef _POSIX_
  #include "pthreads.h"
#else
  #include <thread>
#endif
*/

/******************************************************************************
 * @name    main()
 * @brief   Connect to ZMQ socket, send message and listen to server answer.
 ******************************************************************************/

#define VERSION "0.0.1"

const std::string version = VERSION;
int main( int argc, char *args[] )
{
  int  rc = 0;           // return code assertion
  bool rs = false;       // return state (zmqpp::receive return value)

  const std::string pname = args[0];
  std::string ipaddr = "0.0.0.1";
  std::string port = "5555";
  std::string endpoint = "tcp://" + ipaddr + ":" + port;
  //std::string parse;
  
  zmqpp::context context;
  zmqpp::socket_type type = zmqpp::socket_type::reply;
  zmqpp::socket socket( context, type );
  zmqpp::message tx_msg;  // send message
  zmqpp::message rx_buff; // receive buffer

  //vsprintf( endpoint, "tcp://", ipaddr, ":", port );

  socket.connect( endpoint );

  /****************************************************
   * @description    Compose a message from a string
   ****************************************************/

  ///@{

  tx_msg << args[argc];
  socket.send( tx_msg );

  std::string text;

  //int number
  //rx_buff >> text >> number;

  ///@}

  /****************************************************
   * @description    Listen to socket till abort
   ****************************************************/

  ///@{

  while ( 1 ) {
    rs = socket.receive( rx_buff );
    //rs = socket.receive( socket, rx_buff );
      assert( rs );

    rx_buff >> text;
    std::cout << text << std::endl;
  }

  ///@}

  return rc;
}

/******************************************************************************
 * @name    printhelp()
 * @brief   Usage of the program.
 ******************************************************************************/
 
void printhelp()
{
 std::cout << "Usage: zmq_client [-i -p] <message>\n";
 std::cout << "  connects to 0.0.0.1:5555 and sends <message>";
 std::cout << "Options:";
 std::cout << "  -i  IP adress";
 std::cout << "  -p  port";
}

