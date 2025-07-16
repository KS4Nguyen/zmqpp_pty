#include <iostream>
#include <string>
#include <initializer_list> // for printv()

#include <assert.h>
//#include <algorithm> // Required for std::sort
#include "unistd.h"

#include <zmqpp/zmqpp.hpp>
//#include "options.hpp"

#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

#define VERSION          "0.0.2"

#define DEBUG            1
#define SUPPORT_RAW_DATA 0


using namespace std;


/******************************************************************************
 * @name  length_to_null()
 * @brief Find the first NULL in a string,
 *        Indeed each string is NULL terminated.
 * @note  '\0' (Hex-0, ASCII 00).
 ******************************************************************************/

size_t length_to_null( const std::string& s ) {
  size_t i = 0;
  while ( i < s.size() && s[i] != '\0' ) {
    i++;
  }
  return i;
}


/******************************************************************************
 * @name  length_to_lf()
 * @brief Find a linefeed in a string, if existing. If none is in there, return
 *        the string length itself.
 * @note  '\n' is the linefeed (LF, ASCII 10).
 *        By the way, get an ASCII code of a character with:
 *
 *        char c = '\0';
 *        int code = static_cast<unsigned char>(c);  // code == 0
 ******************************************************************************/

size_t length_to_lf( const std::string& s ) {
  size_t i = 0;
  while ( i < s.size() && s[i] != '\n' ) {
    i++;
  }
  return i;
}


void printd( initializer_list<string> texts ) {
  #ifdef DEBUG
    #if ( DEBUG == true )
      for ( const auto& s : texts ) {
        cout << s;
      }
      cout << '\n';
    #endif
  #endif
  ;
}


/******************************************************************************
 * @name    printhelp()
 * @brief   Usage of the program.
 ******************************************************************************/

void printhelp()
{

   cout << "Usage: zmq_client <Socket-Type> <Endpoint> [Options]\n\n";
   cout << "  Endpoint:    Format tcp://127.0.0.1:4242 (default endpoint)\n\n";
   cout << "  Socket-Type: One of the following:\n";
   cout << "               pub, sub, push, pull, pair\n\n";
   cout << "  Options:\n";
   cout << "  -v  Verbose mode.\n";
   cout << "  -h  Print this help.\n";
}


/******************************************************************************
 * @name    printv()
 * @brief   Verbose print when 'verbose' is 'true'.
 ******************************************************************************/

///@{
string pname = "none";
bool verbose = false;

void printv( initializer_list<string> texts )
{
  if ( verbose == true ) {
    for (const auto& s : texts) {
      cout << s;
    }
    cout << '\n';
  }
}
///@}


/******************************************************************************
 * @name    get_socket_endpoint()
 * @brief   Can be used to check if socket is connected.
 * @return  int64_t endpoint. When 0 no active socket connection.
 ******************************************************************************/

int64_t get_endpoint( zmqpp::socket socket )
{
   int64_t ep = 0;
   //string ep;
   //size_t len;

   socket.get( zmqpp::socket_option::last_endpoint, ep );
   //ep.resize(len);
   //socket.getsockopt(ZMQ_LAST_ENDPOINT, ep.data(), &len);
   //ep.resize(len);

   // TODO Convert int64_t endpoiint to string aka "x.x.x.x" address.
   return( ep );
}


/******************************************************************************
 * @name    poll_messages()
 * @brief   Receive ZMQ message, thread.
 * @param   *s      Pointer to ZMQ socket, where to grab incomming messages.
 * @param   *buff   Pointer to messae buffer.
 ******************************************************************************/

///@{
#define RX_SIZE 1024

atomic<int> new_msg (0);
mutex mtx_poll_messages; // mutex for all resourcrs shared with recept()

void poll_messages( zmqpp::socket& s, vector<string> &buff, bool stdin )
{
  string msg_str;
  size_t msg_size = 0;

  zmqpp::poller poller;
  poller.add( s );
  const bool can_send = true;
  if( true == stdin ) { poller.add( 1 ); }

  #if( SUPPORT_RAW_DATA == 1 )
    char msg_buff[RX_SIZE];
    memset( msg_buff, 0, RX_SIZE );  // Ensure nulls
  #else
    zmqpp::message msg_zmq;
  #endif

  while ( s ) {
    poller.check_for( s, ( stdin ) ? zmqpp::poller::poll_in : zmqpp::poller::poll_none );

    if( stdin == true )
    {
      poller.check_for( stdin, ( can_send ) ? zmqpp::poller::poll_in : zmqpp::poller::poll_none );
    }

    if ( poller.poll() ) {
      #if( SUPPORT_RAW_DATA == 1 )
        try {
          printd( {"poll_message() Waiting for message."} );
          //zmqpp::recv( socket, message );
          s.receive_raw( &msg_buff[0], msg_size, 0 );
          #if DEBUG == 1
            cout << "msg_buff[] size (" << msg_size << ")" << endl;
          #endif
        }
        catch ( const std::exception& e ) {
          // Defauit exception handler
          printd( {"poll_message() Exception: ", e.what()} );
          break;
        }
        catch (...) {
          // Catch what() exceptions
          printd( {"poll_message() Unknown exception!"} );
          break;
        }

        msg_str = msg_buff;

        // Determine message size
        msg_size = 0;
        while ( msg_size < RX_SIZE ) {
          if ( msg_buff[msg_size] == '\0' ) { break; }
        }

        // Empty buffer again
        memset( msg_buff, 0, RX_SIZE );

      #else
        s.receive( msg_zmq, true );

        // Read as a string
        msg_zmq >> msg_str;

        // @note:
        // In c++ there are
        // (1)  C-Strings (arrays of char, null-terminated)
        // (2)  std::string (abstract container, keeps-track of the length)
        msg_size = length_to_null( msg_str );
        cout << "[" << msg_size << "]" << msg_str << endl;
      #endif

      if ( msg_size > 0 ) {
        //mtx_poll_messages.lock();
        lock_guard<std::mutex> lock( mtx_poll_messages );
          buff.push_back( msg_str );
          new_msg++;
        //mtx_poll_messages.unlock();
        lock_guard<std::mutex> unlock( mtx_poll_messages);
      }
      //while( s.has_more_parts() ); // TODO Check for more data incomming.
    }
  }
}
///@}


/******************************************************************************
 * @name    main()
 * @brief   Connect to ZMQ socket, send message and listen to server answer.
 ******************************************************************************/

///@{
int main( int argc, char **argv )
{
  int rc             = 0; // return code assertion
  verbose            = true;
  pname              = argv[0];

  string endpoint    = "tcp://localhost:4242";
  string stype       = "push";

  if ( argc > 1 ) { stype = argv[1]; }
  if ( argc > 2 ) { endpoint = argv[2]; }

  printv( {pname, " v", VERSION} );

  bool socket_initialized = false;
  zmqpp::socket_type type;
  zmqpp::context context;

  if ( stype == "pub"  ) { type = zmqpp::socket_type::publish; }   else
  if ( stype == "sub"  ) { type = zmqpp::socket_type::subscribe; } else
  if ( stype == "push" ) { type = zmqpp::socket_type::push; }      else
  if ( stype == "pull" ) { type = zmqpp::socket_type::pull; }      else
  if ( stype == "pair" ) { type = zmqpp::socket_type::pair; }      else
  {
      cout << "Error: Unknown socket type!" << endl;
      printhelp();
      return ( -1 );
  }

  // Prepare socket
  zmqpp::socket socket( context, type );
  zmqpp::socket *socket_ptr = &socket;
  if ( type == zmqpp::socket_type::subscribe ) { socket.subscribe( "" ); }
  if ( type == zmqpp::socket_type::pull ) { socket.bind( endpoint ); }
  // TODO Check if "bind" is allowed to all s-types!

  socket.connect( endpoint );

  if ( socket_ptr ) {
    socket_initialized = true;
    printv( {"Socket initialized at ", endpoint} );
  } else {
    cout << "Error: Could not connect to socket " << endpoint << endl;
    return ( -1 );
  }

  // Initialize RX-Buffer.
  vector<string> rx_buff;

  // Initialize TX-Message
  zmqpp::message tx_msg;
  string tx_msg_str;// TODO Use zmqpp::message::add_raw()

  this_thread::sleep_for( chrono::milliseconds(100) );
  
  // Listen to socket and reply
  #if( DEBUG == 1 )
    poll_messages( *socket_ptr, ref(rx_buff), true );
  #else
    std::thread thr_receive(
      poll_messages,      // void(*)(socket*, vector*)
      std::ref( socket ), // Reference to zmqpp::socket
      std::ref( rx_buff ) // Reference to std::vector<std::string>
    );
    //thr_receive.detach();
  #endif

  while ( 1 ) {
    if ( new_msg > 0 ) {
      if ( mtx_poll_messages.try_lock() ) {
        cout << rx_buff[new_msg-1] << endl;

        // Discard latest message
        rx_buff.pop_back();
        new_msg--;

        /**
         * @note	TODO Use bufer as FIFO or create FIFO class.
         *			  std::sort( rx_buff.begin(), rx_buff.end() );
         */

        mtx_poll_messages.unlock();
      }
    }

    // Read terminal file descripter input
    printd( {">>>"} );
    std::getline( cin, tx_msg_str );
    tx_msg << tx_msg_str;
    // Prepare ZMQ message raw-data
    //tx_msg.add_raw( tx_msg_string, sizeof( *tx_msg_string );

    // Send ZMQ message on socket
    socket.send( tx_msg );

  }

  // TODO Implement abort condition/signal.
  if ( socket_initialized ) {
    printv( {"Closing socket at ", endpoint} );
    socket.close();
  }

  #if( DEBUG < 1 )
    thr_receive.join();
  #endif

  printv( {"Terminating (", to_string( rc )} );

  return rc;
}
///@}


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
