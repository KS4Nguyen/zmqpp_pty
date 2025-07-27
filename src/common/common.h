/*!
 * \name        common.h
 * \date        29.10.2018     File created.
 * \date        20.06.2019     Added atonum.
 * \date        21.09.2019     Activated PTY pair.
 * \date        23.11.2019     Added char <-> uint8_t conversion functions
 * \date		28.06.2025	   Removed overloaded definition for u8toc()
 * \author      ksnguyen       <sebastian.nguyen@asog-central.de>
 */

/* common.h: system-dependent declarations; include this first.
   Copyright (C) 1996, 2005, 2006, 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _COMMON_HELPERS
#define _COMMON_HELPERS 1

#define COMMON_HELPERS_VERSION 1.1

//#define DEBUG 1

// Assume ANSI C89 headers are available.
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#define INVOKE_STRINGS
	//#undef INVOKE_STRINGS

/*!
 * \note
 * Hence this programs' intention is to run on embedded platforms we avoid
 * implementation depended time- and locals-functions.
 */
//#include <locale.h>

// POSIX headers. If they are not available, we use the substitute provided by
// GNUlibc
#include <stdarg.h>
#include <getopt.h>

// LINUX headers
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>

typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned int      uint32_t;
// \warning  uint64_t conflicts typedeclaration in stdint.h
//typedef unsigned long int uint64_t;

bool verbose;

#define HWCLOCK 1

#ifdef HWCLOCK
	#include <time.h>
#endif

#ifndef NULL
	#define NULL ((void *)0)
#endif

#define PTY_BUFLEN 128

#ifdef __cplusplus
	extern "C" {
#endif

/*!
 * \brief	"The job type contains information about a job, which is a set of
 * 			subprocesses linked together with pipes. The process type holds
 * 			information about a single subprocess. [Following] are the relevant
 * 			data structure declarations."
 * 				[GNU-libc ReferenceManual, chapter 28 "Job Control"]
 */
typedef struct process
{
	struct process *next;	// Next process in pipeline
	char **argv;			// For exec, pointer to strings list of arguments
	pid_t pid;				// Process ID of process replaced by programm exec
	//pid_t pid_exec;		// Process ID of executed program
	char completed;			// 'true' if process has completed
	char stopped;			// 'true' if process has stopped
	int status;				// Reported status value
} process; // A process is a single process.

// TODO: Evaluate if the concept of 'job' control is downward vertical consistent
//       to the implementation of 'process'.
typedef struct job
{
	struct job *next;		// Next active job
	char *command;			// Command line, used for messages
	process *first_process;	// List of processes in this job
	pid_t pgid;				// Process group ID
	char notified;			// 'true' if user told about stopped job
#ifdef _TERMIOS_H
	struct termios tmodes;	// Saved terminal modes
#endif
	int stdin, stdout, stderr;
} job; // A job is a pipeline of processes.

/*!
 * \note	If not brought from program using this library, avoiding strings in
 * 			all cases to ensure better portability.
 */
#ifdef _STRING_H

/*!
 * \name    printd()
 * \brief   Prints just like printf() with starting "Debug: ".
 */
#ifdef DEBUG
  #if DEBUG == 1
    void printd( const char *msg, ... );
  #else
    #define printd( ) ( ; )
  #endif
#endif

/*!
 * \name    atonum()
 * \brief   Converts a character-array to integer number.
 * \return  Converted integer.
 */
int atonum(char *s);

/*!
 * \name    combine()
 * \brief   Allocates new memory and combines two data set in a single set
 * \return  Pointer to new memory location
 */
void *combine(void *o1, size_t s1, void *o2, size_t s2);

/*!
 * \name    Replace_substring()
 * \brief   Replacing a substring within a std::string using memcpy().
 * \return  Pointer to new generated string.
 */
char *replace_substring(char *source, const char *replace, const char *part);

/*!
 * \name	combine_msg()
 * \return	The concatenated message strings given with any number of arguments
 * \note	For message print and logging
 */
char *ccmsg(const char *str, ...);

/*!
 * \name	concat()
 * \param	Multistring concatenation
 * \return	Concatenated string
 * \note	For widestrings, memory optimized
 */
char *concat(const char *str, ...);
#endif

/*!
 * \name	strcb()
 * \brief	Avoiding excessive memory access character-array combining by using
 * 			one register near CPU (depending on target architecture).
 * \param	First string str1
 * \param	Second string str2 that is appended to str1
 * \return	Concatenation
 */
char *strcb(const char *str1, const char *str2);

/*!
 * \name	itoc()
 * \param	Integer to convert to string with size limitation
 * \return	Character array, converted from integer pararameter
 */
char *itoc(int maxlen, long int *num_to_convert);

/*!
 * \name	to_char()
 * \param	Format from which to convert following arguments to string (char*)
 * \return	Formatted string
 */
char *to_char(const char *fmt, ...);

/*!
 * \name           str_find_str()
 * \brief          Search for a string-pattern within another string.  
 * \param   [IN]   *haystack   The string to search within.
 * \param   [IN]   *needle     Searchpattern.
 * \return         'true' if found.
 */
bool str_find_str( char *haystack, char *needle );

/*!
 * \name            u8ncmp()
 * \brief           The function compares the first (at most) n bytes of u1 and
 *                  u2.
 *
 * \param   [in]    *u1        First uint8_t array.
 * \param   [in]    *u2        Second uint8_t array.
 * \param   [in]    u1_len     Number of uint8_t elements in u1[].
 * \param   [in]    u2_len     Number of uint8_t elements in u2[].
 * \return          It returns an integer less than, equal to, or greater than
 *                  zero if u1 is found, respectively, to be less than, to
 *                  match, or be greater than u2.
 * \note    It is a helper function for rematch(), just for uint8_t's. That is
 *          why num_comp is not of type size_t
 */
int u8ncmp( uint8_t *u1, uint8_t *u2, size_t u1_len, size_t u2_len, \
            unsigned int num_comp );

/*!
 * \name            asciitou8()
 * \brief           Convert a char-array like "01af" to "\x0\x1\xA\xF".
 *
 * \param   [OUT]   *out      Pointer to uint8_t array to convert to.
 * \param   [in]    out_size  The buffer capacity (number of half-bytes Rest will be filled with
 *                            zeros aka 0x00.
 * \param   [in]    *in       The input array of chars [0..9 and A..F] like this:
 *                            "C140" will result in 0xC 0x1 0x4 0x0.
 * \param   [in]    in_len    String-length (terminating '\0' not counted).
 *
 * \note    It is a helper function for rematch(), just for uint8_t's. That is
 */
size_t asciitou8( uint8_t *out, size_t out_size, char *in, size_t in_len );

size_t u8toascii( char *out, size_t out_size, uint8_t *in, size_t in_num );

/*!
 * \name            snprintu8()
 * \brief           Characters representing hexadecimal numbers are translated
 *                  to according unsigned numbers.
 * \note            Two characters will be combined to one single byte like
 *                  this: (char*)A5 --> (uint8_t*)0xA5.
 * \param  [OUT]    *out      Pointer to buffer to store the translated bytes.
 * \param  [IN]     out_size  Memory size of the buffer.
 * \param  [IN]     *in       Input string.
 * \param  [IN]     in_size   Number of characters to translate.
 * \return          Adress of the *out buffer.
 */  
uint8_t *snprintu8( uint8_t *out, size_t out_size, char *in, size_t in_size );

/*!
 * \name            u8nprints()
 * \brief           Translates a buffer of uint8_t numbers to a string, that
 *                  may contain characters of [0..9, a..f)
 * \param  [out]    *out      The buffer to put the translated string into.
 * \param  [in]     out_size  The size of the out-buffer, that should even and
 *                            at least 2 times as big as the in-buffer of uint8_t
 *                            numbers. If smaller translation is truncated to
 *                            out_size.
 * \param  [in]     *in       Input-buffer of uint8_t numbers.
 * \param  [in]     in_num    Amount of numbers that should be converted.
 *                            There is no error-handling implementation for the
 *                            case, that in_num is higher than the actual size
 *                            of the in-buffer!
 * \return  Returns the address of *out.
 */
char *u8nprints( char *out, size_t out_size, uint8_t *in, size_t in_num );

static void u8toc( char *high_nibble_conv, char *low_nibble_conv, uint8_t in );

#if __cplusplus >= 201103L
	/* C++11-Code */
	static char u8toc( uint8_t u8 );
//#else
	/* older C++ (98/03) */
#endif

static uint8_t ctou8( char in_char );

/*!
 * \name             print_hex_buffer()
 * \brief            Print a message buffer casted to uint8_t aka HEX to stdout.
 *
 * \param  [OUT]     *s       The stream to print out, usually the STDOUT.
 * \param  [in]      *buff    The buffer, the raw-data is to be taken from.
 * \param  [in]      amount   The number of bytes to convert to, respectively
 *                            to print out as unsigned int := uint8_t.
 */
void print_hex_buffer( FILE *s, void *buff, size_t amount );

void die(const char *s, void *err_code);

/*!
 * \name	s_sleep()
 * \param	Do nothing for <msecs>
 */
void s_sleep(int msecs);

/*!
 * \name	timestamp()
 * \param	Unformatted time struct or nothing
 * \param   Format to print in logfiles and STDOUT (same like shell command
 * 			'date' is used)
 * \return	Pointer to formatted time string value. If no format is set by call
 * 			with NULL, a default formatted string is printed in form
 * 			yymmddHHMMSS
 * \brief	The imestamp formatting is quite odd, but when analysing logfiles
 * 			filtered and automated text parsing can achieved muche more easier.
 * 			Logfile entries will be like:
 * 			  181128154540 [ERROR] cantest.sh CAN1/2 Loopback message lost.
 * 			One can filter the generated logfile e.g.:
 * 			  cat ls1021a_20181128110005.log | grep [ERROR] | awk '{printf $1}'
 */
char *timestamp(time_t *unformatted_time, const char *format);

/*!
 * \name	create_logfile()
 * \param	Path and name where the logfile should be created. Opens existing
 * 			logfile and appends to it, if so.
 * \return	See definitions below
 */
#define LOG_APPEND_EXISTING		0	// TODO: Check if logfile appending is done.
#define LOG_NEW_CREATED			0
#define LOG_ERROR_CREATE		1
#define LOG_ERROR_CLOSE			2
int create_logfile(char *logfilepath);

/*!
 * TODO:
 * \param	fp1 is the filepointer where to append to.
 * \param	fp2 is the filepointer of file that will be deleted after appending.
 * \brief	This mechanism is used to ensure temporar logfiles after each test
 * 			cycle. In former versions of PQsoftware additional files for error
 * 			counters where created. Everything got messed-up if you run a PQ-
 * 			software twice and forgot to delete these error counting files.
 * \return	TODO:
 */
int catdel_logfile(FILE *fp1, FILE *fp2);

/*!
 * \name	close_logfile
 * \brief	One might close a logfil and create a new one if test cycles should
 * 			be seperated to different logfiles
 * \return	Returns '0' if closed successfully
 */
int close_logfile(void);

/*!
 * \name	log_do()
 * \param	Message will be written to logfile (*lfp), if logging enabled
 */
void log_do(const char *message);

/*!
 * \name	log_info()
 * \param 	Log message and print it
 * \note	Uses log_do()
 */
void log_info(const char *message);

/*!
 * \name	log_error()
 * \param	Logs the message with preceding [ERROR] string
 * \note	Uses log_do()
 */
void log_error(const char *message);

/*!
 * \name	die()
 * \brief	The exit handler function
 * \note	Do not use this in POSIX threaded programs. There must be a thread-
 * 			safe implementation where all processes and forks as to be cleaned
 * 			correctly!
 */
// \note	When running POSIX threads die() function must have to be a handler.
#define handle_error_en(errsv, msg) \
        do { errno = errsv; perror(msg); exit(EXIT_FAILURE); } while (0)

/*
#ifndef _PTHREAD_H
  void die(const char *s, void *err_code);
#else
	#define die(x, y) handle_error_en(y, x)
#endif
*/
#include <wait.h>
/*!
 * \name	sigchld_handler()
 * \brief	Use unistd.h function waitpid() to get the status from all child
 *	 		processes that have terminated, without ever waiting. This function
 *	 		is designed to be a handler for SIGCHLD, the signal that indicates
 *	 		that at least one child process has terminated.
 * \note	Usually <stddef.h> is invoked for MT-save usage according to POSIX.
 */
void sigchld_handler(int signum, int *mutexed_pid, int *mutexed_status_of_pid);


/*!
 * TODO: Function not tested!
 * \name	process_start()
 * \brief	Execution of a command
 * \param	Pointer to process object
 * \return	The status of process
*/
#define SHELL "/bin/sh"
#define DEBUG_CMD "./test_ps.sh"
int process_start(process *ps, char **command, char **arguments);

/*!
 * \note	Declare something like job *first_job = NULL; as the active jobs are
 *			linked into a list. This is its head.
 */

/*!
 * \name	job_is_stopped()
 * \param	Pointer of job, which might be stopped.
 * \return	'true' if all processes in the job have stopped or completed.
 */
int job_is_stopped(job *j);

/*!
 * \name	job_is_completed()
 * \param	Pointer of job, whose status is to be checked for completion.
 * \return	'true' if all processes in the job have completed.
 */
int job_is_completed(job *j);

#ifdef __cplusplus
	}
#endif

#endif /* _COMMON_HELPERS */

