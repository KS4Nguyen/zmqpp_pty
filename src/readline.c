/* zmqspty.c -- ZMQ Socket PTY */

/*
 * Create a /dev/ptx-zmq1.
 * Create a ZMQ socket.
 * Bidirectional stream of bytes from/to socket (EOL with \0).
 * Interactive terminal mode optional.
 *
 * This source code is based on rdptytest.c from readline 8.2 package by:
 */

/* Author: Bob Rossi <bob@brasko.net>

   Copyright (C) 1987-2022 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library (Readline), a library
   for reading lines of text with interactive input and history editing.      

   Readline is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Readline is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Readline.  If not, see <http://www.gnu.org/licenses/>.
 */
 
/*
 * The source code mentioned above was modified with a ZMQ socket endpoint and
 * no-echo on PTY by:
 */
 
/* Author:	Sebastian Nguyen <sebastian.nguyen86@gmail.com
   Github:	KS4Nguyen https://github.com/KS4Nguyen/zmq_pty

   Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>

   This file is part of the zmq_pty project, a bundle of programs
   for reading/writing lines of text with interactive input, aswell as
   bidirectional data-streaming from terminal device to/from ZMQ stream
   socket.

   zmq_pty is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   zmq_pty is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with zmq_pty.  If not, see <http://www.gnu.org/licenses/>.
 */

#if defined (HAVE_CONFIG_H)
#include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <curses.h>

#include <stdlib.h>
#include <unistd.h>

#include <signal.h>

#if 1	/* LINUX */
#include <pty.h>
#else
#include <util.h>
#endif

#ifdef HAVE_LOCALE_H
#  include <locale.h>
#endif

#ifdef READLINE_LIBRARY
#  include "readline.h"
#else
#  include <readline/readline.h>
#endif

// ZMQ c defines.
#include <czmq.h>
#define MESSAGE_SIZE 4096
#define CSIZE 16



// Master/Slave PTY used to keep readline off of stdin/stdout.
static int masterfd = -1;
static int slavefd;

void sigint( int s );
void sigwinch( int s );
static int user_input();
static int readline_input();
static void rlctx_send_user_command( char *line );

/* The terminal attributes before calling tty_cbreak */
static struct termios save_termios;
static struct winsize size;
static enum { RESET, TCBREAK } ttystate = RESET;

/* tty_cbreak: Sets terminal to cbreak mode. Also known as noncanonical mode.
 *    1. Signal handling is still turned on, so the user can still type those.
 *    2. echo is off
 *    3. Read in one char at a time.
 *
 * fd    - The file descriptor of the terminal
 * 
 * Returns: 0 on success, -1 on error
 */
int tty_cbreak( int fd )
{
	struct termios buf;
	int ttysavefd = -1;

	if(tcgetattr(fd, &save_termios) < 0)
	return -1;

	buf = save_termios;
	buf.c_lflag &= ~(ECHO | ICANON);
	buf.c_iflag &= ~(ICRNL | INLCR);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	#if defined (VLNEXT) && defined (_POSIX_VDISABLE)
		buf.c_cc[VLNEXT] = _POSIX_VDISABLE;
	#endif

	#if defined (VDSUSP) && defined (_POSIX_VDISABLE)
		buf.c_cc[VDSUSP] = _POSIX_VDISABLE;
	#endif

	//nable flow control; only stty start char can restart output
	#if 0
		buf.c_iflag |= (IXON|IXOFF);
		#ifdef IXANY
			buf.c_iflag &= ~IXANY;
		#endif
	#endif

	// Disable flow control; let ^S and ^Q through to pty
	buf.c_iflag &= ~(IXON|IXOFF);
	#ifdef IXANY
		buf.c_iflag &= ~IXANY;
	#endif

	if(tcsetattr(fd, TCSAFLUSH, &buf) < 0)
		return -1;

	ttystate = TCBREAK;
	ttysavefd = fd;

	// set size
	if(ioctl(fd, TIOCGWINSZ, (char *)&size) < 0)
		return -1;

	#ifdef DEBUG
		err_msg("%d rows and %d cols\n", size.ws_row, size.ws_col);   
	#endif

	return (0);   
}

int tty_off_xon_xoff( int fd )
{
	struct termios buf;
	int ttysavefd = -1;

	if(tcgetattr(fd, &buf) < 0)
		return -1;

	buf.c_iflag &= ~(IXON|IXOFF);

	if(tcsetattr(fd, TCSAFLUSH, &buf) < 0)
		return -1;

	return 0;   
}

/* tty_reset: Sets the terminal attributes back to their previous state.
 * PRE: tty_cbreak must have already been called.
 * 
 * fd    - The file descrioptor of the terminal to reset.
 * 
 * Returns: 0 on success, -1 on error
 */
int tty_reset(i nt fd) 
{
	if(ttystate != TCBREAK)
		return (0);

	if(tcsetattr(fd, TCSAFLUSH, &save_termios) < 0)
		return (-1);

	ttystate = RESET;

	return 0;   
}

/* **************************************************************** 
 *
 * /dev/pts-zmq-1
 * ZMQ socket
 * 
 * ****************************************************************/

int main( stdargs[] *args )
{
	register int i, verbose;

	int val = 0;
	verbose = 1;

	// Create message cache
	const int c_csize = (int)(MESSAGE_SIZE * CSIZE * sizeof (char *)); 
	char* cache = xmalloc ( c_csize );
	// Initialize as empty
	for (i=0; i<c_csize; i++) {
		cache[i * sizeof (char *)] = (char *)NULL;
	}

	#ifdef HAVE_SETLOCALE
		setlocale( LC_ALL, "" );
	#endif

	val = openpty( &masterfd, &slavefd, NULL, NULL, NULL );
	if ( val == -1 )
		return -1;

	val = tty_off_xon_xoff( masterfd );
	if (val == -1)
		return -1;

	signal( SIGWINCH, sigwinch );
	signal( SIGINT, sigint );

	val = init_readline( slavefd, slavefd );
	if (val == -1)
		return -1;

	val = tty_cbreak( STDIN_FILENO );
	if (val == -1)
		return -1;

	val = main_loop();

	CLEANUP:

	xfree( cache[0] );  // xfree (cache);
	tty_reset( STDIN_FILENO );

	if (val == -1)
		return -1;
	return 0;
}

void sigint( int s )
{
	tty_reset (STDIN_FILENO);
	close (masterfd);
	close (slavefd);
	printf ("\n");
	exit (0);
}

void sigwinch( int s )
{
	rl_resize_terminal ();
}

static int user_input()
{
	int size;
	const int MAX = 1024;
	char *buf = (char *)malloc(MAX+1);

	size = read (STDIN_FILENO, buf, MAX);
	if (size == -1)
		return -1;

	size = write (masterfd, buf, size);
	if (size == -1)
		return -1;

	return 0;
}

static int readline_input()
{
	const int MAX = 1024;
	char *buf = (char *)malloc(MAX+1);
	int size;

	size = read (masterfd, buf, MAX);
	if (size == -1)
	{
		free( buf );
		buf = NULL;
		return -1;
	}

	buf[size] = 0;

	// Display output from readline
	if ( size > 0 )
		fprintf(stderr, "%s", buf);

		free( buf );
		buf = NULL;
		return 0;
	}

static void rlctx_send_user_command(char *line)
{
  /* This happens when rl_callback_read_char gets EOF */
  if ( line == NULL )
    return;
    
  if (strcmp (line, "exit") == 0) {
  	tty_reset (STDIN_FILENO);
  	close (masterfd);
  	close (slavefd);
  	printf ("\n");
	exit (0);
  }
  
  /* Don't add the enter command */
  if ( line && *line != '\0' )
    add_history(line);
}

static void custom_deprep_term_function ()
{
}

static int 
init_readline (int inputfd, int outputfd) 
{
  FILE *inputFILE, *outputFILE;

  inputFILE = fdopen (inputfd, "r");
  if (!inputFILE)
    return -1;

  outputFILE = fdopen (outputfd, "w");
  if (!outputFILE)
    return -1;

  rl_instream = inputFILE;
  rl_outstream = outputFILE;

  /* Tell readline what the prompt is if it needs to put it back */
  rl_callback_handler_install("(rltest):  ", rlctx_send_user_command);

  /* Set the terminal type to dumb so the output of readline can be
   * understood by tgdb */
  if ( rl_reset_terminal("dumb") == -1 )
    return -1;

  /* For some reason, readline can not deprep the terminal.
   * However, it doesn't matter because no other application is working on
   * the terminal besides readline */
  rl_deprep_term_function = custom_deprep_term_function;

  using_history();
  read_history(".history"); 

  return 0;
}

static int 
main_loop(void)
{
  fd_set rset;
  int max;
    
  max = (masterfd > STDIN_FILENO) ? masterfd : STDIN_FILENO;
  max = (max > slavefd) ? max : slavefd;

  for (;;)
    {
      /* Reset the fd_set, and watch for input from GDB or stdin */
      FD_ZERO(&rset);
        
      FD_SET(STDIN_FILENO, &rset);
      FD_SET(slavefd, &rset);
      FD_SET(masterfd, &rset);

      /* Wait for input */
      if (select(max + 1, &rset, NULL, NULL, NULL) == -1)
        {
          if (errno == EINTR)
             continue;
          else
            return -1;
        }

      /* Input received through the pty:  Handle it 
       * Wrote to masterfd, slave fd has that input, alert readline to read it. 
       */
      if (FD_ISSET(slavefd, &rset))
        rl_callback_read_char();

      /* Input received through the pty.
       * Readline read from slavefd, and it wrote to the masterfd. 
       */
      if (FD_ISSET(masterfd, &rset))
        if ( readline_input() == -1 )
          return -1;

      /* Input received:  Handle it, write to masterfd (input to readline) */
      if (FD_ISSET(STDIN_FILENO, &rset))
        if ( user_input() == -1 )
          return -1;
  }

  return 0;
}

/* ## EOF ## */
