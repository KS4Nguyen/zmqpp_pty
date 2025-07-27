/******************************************************************************
 * @author  KS_Nguyen <sebastian.nguyen86@gmail.com
 * @version 2025-07-25  v0.2
 *                      Added the PTY support - finally.
 ******************************************************************************/
//
#include <cstdio>
#include <iostream>
#include <string>

// ZMQ c++ high-level API
#include <zmqpp/zmqpp.hpp>


// Threading and timing
#include <assert.h>
#include <thread>
#include <chrono>

// POSIX headers for PTY
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <pty.h>
//#include <pty/pty.h>
#include <string>

#define VERSION "0.2"


using namespace std;

const string version = VERSION;


/******************************************************************************
 * @name    printv()
 * @brief   Verbose print when 'verbose' is 'true'.
 ******************************************************************************/

///@{
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
 * @name    open_pty()
 * @brief   Opens a pseudo-terminal (PTY) and returns the master file
 *          descriptor.
 * @param   slave_path Reference to a string that will hold the path in the
 *          file system to the slave end of the PTY.
 * @return  The file descriptor for the master end of the PTY. Returns -1 on
 *          failure.
 ******************************************************************************/
/*
int open_pty( std::string &slave_path ) {
  int master_fd;
  char slavename[127];
  slavename[0] = '\0'; // Initialize the slave name buffer

  // Open a new pseudo-terminal (PTY)
  if ( openpty( &master_fd, nullptr, slavename, nullptr, nullptr ) < 0 ) {
    throw std::runtime_error( "open_pty() Failed to open PTY" );
  }

  slave_path = slavename;

  return master_fd;
}
*7

/******************************************************************************
 * @name    main()
 * @brief   ZMQ server that listens for messages and processes them.
 ******************************************************************************/

int main( int argc, char *argv[] ) {
  const string pname   = argv[0];          // program name
  int rc               = 0;                // return code assertion
  string endpoint      = "tcp://*:4242";   // default endpoint
  string pty_slave     = "/dev/pts/zmqpp"; // default PTY slave path
  string pty_master    = "/dev/pts/ptmx";  // default PTY master path
  int    pty_slave_fd  = -1;
  int    pty_master_fd = -1;

  verbose = true;

  // Verbose print program information
  if ( verbose == true ) {
    int major, minor, patch;
    string zmqver;
    zmq_version( &major, &minor, &patch );

    printf( "%s v%s\n", pname.c_str(), version.c_str() );
    printf( "0MQ version: %d.%d.%d\n", major, minor, patch );
  }

  // Initialize the 0MQ context and socket first before creating the PTY in f.s.
  zmqpp::context context;
  zmqpp::socket_type type = zmqpp::socket_type::pull;
    const string stype = "pull";
  zmqpp::socket worker( context, type );

  //worker.connect( "tcp://broker:4242" );

  // Set worker non-blocking mode due to PTY usage
  // TODO: worker.set( zmqpp::socket_option::non_blocking, true );

  try {
    worker.bind( endpoint );
  } catch ( const zmqpp::exception &e ) {
    cerr << "Error: Could not bind to " << endpoint << endl;
    cerr << "Reason:\n" << e.what() << endl;
    return -2;
  }

  printv( {"Socket (", stype, ") bound to ", endpoint} );

  // Create a pseudo-terminal (PTY)
  try {
    //pty_master_fd = open_pty( pty_slave );
	//if ( pty_master_fd < 0 ) { rc = pty_master_fd; }

	char *pty_slave_c;
	pty_slave_c = (char*) &pty_slave;

    // POSIX PTY pair
    rc = openpty( &pty_master_fd, nullptr, pty_slave_c, nullptr, nullptr );
	//rc = pty_pair_init( &pty_master_fd, &pty_slave_fd, pty_slave_c, \
                        (int) pty_slave.length(), nullptr, O_RDWR | O_NOCTTY );
  } catch ( const std::exception &e ) {
    cerr << "Error: Failed to open PTY: " << e.what() << endl;
    return -1;
  } catch ( ... ) {
    cerr << "Error: Failed to open PTY. Unknown exception." << endl;
    return -2;
  }

  printv( {"PTY master FD=", to_string(pty_master_fd)} );

  // TODO: termios and winsize structures can be used to set terminal attributes

  /**
   * @note	TODO: Using forkpty() to create a PTY and fork a child process.
   *        This is useful for running a shell or other interactive program
   *        in the PTY.
   */
  //extern int forkpty( int *__amaster, char *__name, \
  //                    const struct termios *__termp, \
  //                    const struct winsize *__winp) __THROW;

  // Event loop to handle incoming messages
  while ( rc == 0 ) {
    zmqpp::message msg;

    if ( worker.receive(msg) ) {
      /**
       * @brief Append each frame's data to the string.
       * @note  Point to frame container data (ANSI C99 pointers inherits the
       *        static_cast by referencing to struct's address).
       * @note  From CPP-reference manual c++23:
       *        for (T thing = foo(); auto& x : thing.items()) { ... }
       */

      //for ( auto &frame : msg ) {
      //  data.append( static_cast<const char*>(frame.data()), frame.size() );
      //}

      string data; // gather the payload in a single string
      size_t num_frames = msg.parts(); // the number of frames in message

      printv( {"Worker received ", to_string(num_frames), " frames."} );

      // Explicitly iterate over each frame in the message due to notes above
      for ( size_t i = 0; i < num_frames; ++i ) {
        /*
         * @brief  Access each frame in the message.
         * @note   zmqpp::message::get() is used to access the frame at index i.
         */

        string part;

        msg.extract(i, part);
        data += part;
      }

      /**
       * @brief TODO: When the messages are not ueed multiple times we can
       *              extract the data directly from the message.
       * @note  Difference between get() vs. pop(): Copy vs. Consume
       *
       * string data;
       * zmqpp::frame frame;
       *
       * while ( msg.pop(frame) ) {
       *   data.append( static_cast<const char*>(frame.data()), frame.size() );
       * }
       */

      printv( {"Complete message: ", data} );

      // Write to PTY
      ssize_t n = write( pty_slave_fd, data.data(), data.size() ); // unistd.h

      if ( n < 0 ) {
        rc = (int)n;
        cerr << "Error: Failed to write to PTY " << pty_slave << endl;
      }
    }

    zmqpp::poller poll;
    poll.add( worker );

    // Non-blocking socket and poller ensure to not get blocked permanently
    poll.poll(10);
  }

  close( pty_slave_fd );
  return rc;
}


/**
 * @note	sys/types.h	Basic POSIX type definitions for system processing.
 *			pid_t, uid_t, gid_t, mode_t, dev_t, ino_t
 *			off_t File offset
 */

