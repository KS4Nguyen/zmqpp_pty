/*!
 * \name     common.c
 * \date	 09.11.2018   File created.
 * \date     21.09.2019   Copied and tested R. Koucha's
 *                        PTY pair example #3 from:
 *                        http://rachid.koucha.free.fr/tech_corner/pty_pdip.html (last update 2014)
 * \author	  ksnguyen
 */

// \ingroup LIBS
// \{
// \brief 	GNU libry includes, licensed under GPL-V3
#include "common.h"
// \}

int do_log = 0;				// if set, logfilepath is set
int use_timestamp = 0;
char logfile[80] = "/tmp/logfile.log";
char logtimeformat[80] = "%F %T";
FILE *lfp = NULL;			// logfile-pointer

#ifndef _LINUX_ERRNO_H
	int errsv = 0;
#else
	int errsv;
#endif

// \ingroup DEBUG
// \{
void printd( const char *msg, ... )
{
  va_list ap, ap2;
  size_t total = 1;
  const char *s;
  char *result;

  va_start(ap, msg);
  va_copy(ap2, ap);

  // Determine how much space we need.
  for ( s=msg; s!=NULL; s=va_arg(ap, const char *) ) {
    total += strlen(s);
  }

  va_end (ap);

  result = (char *)malloc(total);
  if ( result != NULL ) {
    result[0] = '\0';

    // Copy the strings together.
    for ( s=msg; s!=NULL; s=va_arg(ap2, const char *) ) {
      strcat(result, s);
    }
  } else {
    perror("Not enough space to concatenate message.\n");
  }

  va_end (ap2);

  printf( "Debug: %s\n", result );
  free( result );
}
// \}

// \ingroup HELPERS
// \{
int atonum(char *s)
{
    int n;

    while (*s == ' ') {
        s++;
        if (strncmp(s, "0x", 2) == 0 || strncmp(s, "0X", 2) == 0)
                sscanf(s + 2, "%x", &n);
        else if (s[0] == '0' && s[1])
                sscanf(s + 1, "%o", &n);
        else
        sscanf(s, "%d", &n);
    }
    return n;
}


#ifdef _STRING_H
void *combine(void *o1, size_t s1, void *o2, size_t s2)
{
	void *result = malloc (s1 + s2);

	if( result != NULL ) {
		memcpy(memcpy (result, o1, s1), o2, s2);
	}
	return result;
}


char *replace_substring(char *source, const char *replace, const char *part)
{
	char *ptr;
	ptr = strstr(source, replace);
	memcpy(ptr, replace, strlen(replace));
	return ptr;
}


char *strcb(const char *str1, const char *str2)
{
	size_t s1 = strlen(str1);
	size_t s2 = strlen(str2);
	size_t total = s1 + s2;
	void *offset; // Marks end of first string when copied

	// Reserve enough space and concatenate the strings
	char *result = (char*)malloc(total+1);
	if( result != NULL ) {
		offset = memcpy(result, str1, total);
		memcpy((&offset+s1), str2, s2);
		result[total] = '\0'; // Complete string
	}
	else {
		free(result);
		perror("Not enough space for string concatenation.");
		return ( (char*)'\0' );
	}
	//printd3("DEBUG: Sizeof %s is %i\n", str1, (int)s1);
	//printd3("DEBUG: Sizeof %s is %i\n", str2, (int)ss);

	return ( result );
}


char *ccmsg(const char *str, ...)
{
	va_list ap, ap2;
	size_t total = 1;
	const char *s;
	char *result;

	va_start(ap, str);
	va_copy(ap2, ap);

	// Determine how much space we need.
	for ( s=str; s!=NULL; s=va_arg(ap, const char *) ) {
		total += strlen(s);
	}

	va_end (ap);

	result = (char *)malloc(total);
	if ( result != NULL ) {
		result[0] = '\0';

		// Copy the strings together.
		for ( s=str; s!=NULL; s=va_arg(ap2, const char *) ) {
			strcat(result, s);
		}
	}
	else {
		perror("Not enough space to concatenate message.\n");
	}

	va_end (ap2);

	return result;
}


char *concat(const char *str, ...)
{
	va_list ap;
	size_t allocated = 100;
	char *result = (char *)malloc(allocated);

	if ( result != NULL ) {
		char *newp;
		char *wp;
		const char *s;

		va_start (ap, str);

		wp = result;
		for ( s=str; s!=NULL; s=va_arg(ap, const char *)) {
			size_t len = strlen(s);

			// Resize the allocated memory if necessary
			if ( wp + len + 1 > result + allocated ) {
				allocated = (allocated + len) * 2;
				newp = (char *) realloc(result, allocated);
				if ( newp == NULL ) {
					free(result);
					return NULL;
				}
				wp = newp + (wp - result);
				result = newp;
			}

			wp = (char*)memcpy(wp, s, len);
		}

		// Terminate the result string
		*wp++ = '\0';

		// resize memory to the optimal size
		newp = (char*)realloc (result, wp - result);
		if (newp != NULL) {
			result = newp;
		}

		va_end (ap);
		free(result);
	}

	return result;
}
#else


char *to_char(const char *fmt, ...)
{
	int n;
	int size = 100; //
	char *p, *np;
	va_list ap;

	if ( (p = malloc(size)) == NULL ) {
		perror("Memory allocation failed in to_char()");
		return NULL;
	}

	while (1) {
		// Try to print in the allocated space
		va_start(ap, fmt);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);

		// Check error code
		if ( n < 0 ) {
			perror("Type converstion failed reading multistring input to_char()");
			return NULL;
		}

		// If that worked, return the string
		if ( n < size ) {
			return p;
		}

		// Else try again with more space
		size = n + 1; // Precisely what is needed

		if ( (np = realloc (p, size)) == NULL ) {
			perror("Memory reallocation failed in to_char()");
			free(p);
			return NULL;
		}
		else {
			p = np;
		}
	}
}


char *strcb(const char *str1, const char *str2)
{
	// Guess we need no more than 100 chars of space
	int size = 100;
	char *buffer;
	buffer = (char*)malloc(size);
	int nchars;

	if( buffer == NULL ) {
		perror("Memory allocation failed for strcb().");
		free(buffer);
		return NULL;
	}

	// Try to print in the allocated space
	nchars = snprintf(buffer, size, "%s%s", str1, str2);
	if( nchars >= size) {
		//Reallocate buffer now, that we know how much space is needed
		size = nchars + 1;
		buffer = (char*)realloc(buffer, size);
		if( buffer != NULL ) {
			// Try again.
			snprintf (buffer, size, "%s%s", str1, str2);
		}
		else {
			perror("Could not reallocate memory for strcb().");
			free(buffer);
			return NULL;
		}
	}

	// The last call worked, return the string.
	return buffer;
}
#endif


char *itoc(int maxlen, long int *num_to_convert)
{
	char *cv = (char *)malloc(maxlen);
	// Cheat for 'long int' would require wide character target
	unsigned int inval = (unsigned int) *num_to_convert;
	if( cv != NULL ) {
		snprintf(cv, maxlen, "%i", inval);
	}
	else {
		perror("Not enough space for conversion of 'long-int'.");
		free(cv);
	}

	return cv;
}


bool str_find_str( char *haystack, char *needle )
{
  bool ret = false;

  if( strstr( haystack, needle ) != NULL )
    ret = true;

  return( ret );
}


int u8ncmp( uint8_t *u1, uint8_t *u2, size_t u1_len, size_t u2_len, \
            unsigned int num_comp )
{
  int i = 0;
  int cnt = -((int)num_comp);
  int thres;

  uint8_t *pu1 = u1;
  uint8_t *pu2 = u2;

  if ( u2_len > u1_len )
    thres = (int)u1_len;
  else
    thres = (int)u2_len;

  /*!
   * \note
   *         -N := u1 is smaller than u2;
   *         0  := exact match;
   *         +N := u1 is greater than u2;
   */
  while ( i < (int)thres ) {
    if ( (char)*(pu2+i) != (char)*(pu1+i) )
      break;
    i++;
    cnt++;
  }

  return( cnt );
}


size_t u8toascii( char *out, size_t out_size, uint8_t *in, size_t in_num )
{
  size_t brs = in_num + 1; // calc size one time
  int i = 0;

  // Truncate, if too big for result buffer
  if ( brs > out_size ) {
    brs = out_size;
    #if DEBUG == 1
      printf( "Debug: u8toascii() Truncation to: %u\n", (unsigned int)brs );
    #endif
  }

  for( i=0; i<(int)(brs-1); i++ ) {
    out[i] = (char)in[i];
  }

  out[brs-1] = '\0';

  return( brs );
}


size_t asciitou8( uint8_t *out, size_t out_size, char *in, size_t in_len )
{
  size_t brs = in_len;
  int i = 0;

  if ( brs > out_size ) {
    brs = out_size;
    #if DEBUG == 1
      printf( "Debug: asciitou8() Truncation to: %u.\n", (unsigned int)brs );
    #endif
  }
 
  while( i < (int)brs ) {
    out[i] = (uint8_t)in[i];
    i++;
  }

  return( brs );
}


typedef struct {
  uint8_t num;
  char rep;
} tLut_u8_c;


// uint8_t nibble-char table (look-up table or dictionary, or whatever to name)
static const tLut_u8_c u8nct[] = {
  { 0x00, '0' }, // for HEX to char conversion, valid values start here...
  { 0x01, '1' },
  { 0x02, '2' },
  { 0x03, '3' },
  { 0x04, '4' },
  { 0x05, '5' },
  { 0x06, '6' },
  { 0x07, '7' },
  { 0x08, '8' },
  { 0x09, '9' },
  { 0x0A, 'a' },
  { 0x0B, 'b' },
  { 0x0C, 'c' },
  { 0x0D, 'd' },
  { 0x0E, 'e' },
  { 0x0F, 'f' }, // ...and end here.
  { 0x0A, 'A' }, // regard for upper- and lower-cases character translation
  { 0x0B, 'B' },
  { 0x0C, 'C' },
  { 0x0D, 'D' },
  { 0x0E, 'E' },
  { 0x0F, 'F' }
};


static void u8toc( char *high_nibble_conv, char *low_nibble_conv, uint8_t in )
{
  int i;

  for ( i=0; i<16; i++ ) {
    if ( (0x0F & (in >> 4)) == u8nct[i].num ) {
      *low_nibble_conv = u8nct[i].rep;
      break;
    }
  }

  for ( i=0; i<16; i++ ) {
    if ( (0x0F & in) == u8nct[i].num ) {
      *high_nibble_conv = u8nct[i].rep;
      break;
    }
  }
}


static uint8_t ctou8( char in_char )
{
  int i;
  uint8_t out;

  /*!
   * \note   Using conversion LUT to represent all ASCII symbols not in range
   *         ['0'..'9', 'a'..'f', 'A'..'F'] as 0x00 to close the memory
   *         segmentation error risk.
   */ 
  for ( i=0; i<21 ; i++ ) {
    if ( in_char == u8nct[i].rep ) {
      out = u8nct[i].num;
      break;
    }

    out = 0x00;
  }

  return ( out );
}


uint8_t *snprintu8( uint8_t *out, size_t out_size, char *in, size_t in_size )
{
  int os = 0;         // out-size
  int i, index = 0;   // out/in-indexes
  const int shift = ((int)in_size % 2);
  uint8_t h, l;       // high and low nibble

  // Prevent memory excess violation
  if ( (int)out_size < ((int)in_size/2 + shift) ) {
    if ( 0 == (os = (int)out_size) )
      return ( out ); // catch zero-output
  } else {
    os = (int)in_size/2 + shift; // guard input-buffer access
    #if DEBUG == 1
      printf( "Debug: snprintu8() Output buffer size limitated: %i\n", (int)os );
    #endif 
  }

  /*!
   * \note  Two single chars are translated into one byte aka unsigned HEX.
   *        High/low nibble swapped.
   *
   *                        Keyboard-in:      Intended order:   Decimal:
   *        Byte-position:  -01--23--45--67   --3---2---1---0   --3---2---1---0
   *        Example:         3C  02  A5  01    3C  02  A5  01    60   2 165   1
   *        Example:         C0  2A  50  1*    0C  02  A5  01    12   2 165   1
   */
  for ( i=0; i<os; i++ ) {
    index = in_size - 1 - (i*2);
    l = ctou8( in[index]   );
    h = ctou8( in[index-1] ); // input MSNibble first
    out[i] = (0xF0 & (h<<4)) | (0x0F & l);
  }

  return ( out ); 
}


char *u8nprints( char *out, size_t out_size, uint8_t *in, size_t in_num )
{
  int i;
  size_t brs = in_num; // bytes reserved for storing translated characters

  // Truncate, if too big for result buffer
  if ( out_size < brs ) {
    brs = out_size;
    #if DEBUG == 1
      printf( "Debug: u8nprints() Truncation to: %u\n", (unsigned int)brs );
    #endif
  }

  for( i=0; i<((int)brs); i++ ) {
    /*!
     * \note    Order like this:
     *          i=0: 0x74    out[0]:  0x04
     *                       out[1]:  0x07
     *          i=1: 0x5A    out[2]:  0x0A
     *                       out[3]:  0x05
     */
    u8toc( &out[2*i+1], &out[2*i], in[i] );
  }

  return( out ); 
}


void print_hex_buffer( FILE *s, void *buff, size_t amount )
{
  int i;
  char *conv = NULL;
  char *in = (char*)buff;

  if ( NULL == (conv = (char*)malloc( 2*amount )) )
    die( "Not enough memory for conversion buffer!", &errno );

  //u8nprints( conv, (2*amount+1), (uint8_t*)buff, amount );
  for ( i=0; i<(int)amount; i++ ) {
    u8toc( &conv[2*i+1], &conv[2*i], in[i] );
  }

  fprintf( stdout, "%s", conv );

  free( conv );
}
// \}


// \ingroup GNULIBC
// \{
#ifdef HWCLOCK
void s_sleep(int msecs)
{
	usleep( ((unsigned int)msecs * 1000) );
}


#define TFORMAT_MAX_SIZE	( (size_t) 20 )
char *timestamp(time_t *unformatted_time, const char *format)
{
   static struct tm tm;
   static char ctime[TFORMAT_MAX_SIZE]; // Size limitted formatted string
   char *p_ctime = ctime;

   if ( NULL != unformatted_time ) {
	   // Take time reference argument
	   tm = *localtime(unformatted_time);
   }
   else {
	   // Take current time
	   time_t now = time(NULL);
	   tm = *localtime(&now);
   }

   if ( 0 == strftime(p_ctime, TFORMAT_MAX_SIZE, format, &tm) ) {
	   perror("Timeformat exceeded maximum size.");
   }

   return ( ctime );
}
#else


/*!
 * \note	For convenient usage in others functions of this library, next must
 *			have been defined.
 */
void s_sleep(int msecs)
{
	;
}


char timestamp(time_t *unformatted_time, char *format)
{
	return ( (const char)"" );
}
#endif
// \}


// \ingroup LOGGING
// \{
int create_logfile(char *logfilepath)
{
	if ( logfilepath != NULL ) {
		strcpy((char*)&logfile[0], logfilepath);
	}

	lfp = fopen(logfile, "w+"); // logfile-pointer signals existing logfile

	if ( lfp == NULL ) {
		perror("Could not create logfile.");
		errsv = 1;
		return LOG_ERROR_CREATE;
	}
	else {
		do_log = 1;
		return LOG_NEW_CREATED;
	}
}


int catdel_logfile(FILE *fp1, FILE *fp2)
{
	return LOG_APPEND_EXISTING;
}


int close_logfile(void)
{
	if ( do_log ) {
		if ( lfp != NULL ) {
			fflush(lfp);
			if ( fclose(lfp) ) {
				perror("Failed closing logfile!");
				errsv = 1;
				return LOG_ERROR_CLOSE;
			}
		}
		else {
			perror("Logfile pointer not derived, but logging was enabled!");
			errsv = 1;
			return LOG_ERROR_CLOSE;
		}
	}

	do_log = 0;
	return 0;
}


void log_do(const char *message)
{
	if ( do_log ) {
		if ( use_timestamp ) {
			fprintf(lfp, "%s %s\n", timestamp(NULL, logtimeformat), message);
			errsv = errno;
		}
		else {
			fprintf(lfp, "%s\n", message);
			errsv = errno;
		}

		if ( errsv ) {
			// TODO: This error message appears when child process cannot write
			//		 logfile, but why? It must have been inheritated file des-
			//       criptors of its parent process.
			perror( strcb(strcb("Failed writing logfile! [", itoc(sizeof(long int), \
			(long int*)&errsv)), "]") );
		}
	}
}


void log_info(const char *message)
{
  extern bool verbose;
	if ( true == verbose )
	  printf( "%s\n", message);
	log_do(message);
}


void log_error(const char *message)
{
	const char *err_msg = strcb("[ERROR]", message);
	printf("%s", err_msg);
	log_do(err_msg);
}
// \}


// \ingroup SIGNAL_HANDLERS
// \{
#ifndef _PTHREAD_H
void die(const char *s, void *err_code)
{
	if ( NULL != err_code ) {
		errsv = *(int *)err_code;
		printf("[ERROR] %d %s\n", errsv, s);
		exit( errsv );
	}
	else
	{
		printf("[ERROR] %s\n", s);
		exit( errsv );
	}
}
#endif


void sigchld_handler(int signum, int *mutexed_pid, int *mutexed_status_of_pid)
{
	int pid, status, serrno;
#ifdef _PTHREAD_H
	pthread_mutex_t mutex_external_flags;
#endif
	serrno = errno;
	while( 1 ) {
		pid = waitpid(WAIT_ANY, &status, WNOHANG);
		if( pid < 0 ) {
			perror("sigchld_handler() internal error when executing waitpid()");
			break;
		}
		if( pid == 0 ) {
			break;
		}

#ifdef _PTHREAD_H
		pthread_mutex_lock(&mutex_external_flags);
#endif
		// \note Alternativ usage for notice_termination(pid, status);:
		*mutexed_pid = pid;
		*mutexed_status_of_pid = status;
#ifdef _PTHREAD_H
		pthread_mutex_unlock(&mutex_external_flags);
#endif
	}

	errno = serrno;
}
// \}


// \ingroup GNULIBC
// \{
int process_start(process *ps, char **command, char **arguments)
{
	int statusupdate = 0;
	ps->pid = fork();

	if ( ps->pid == 0 ) {
		// This is the child process. Execute the shell command.
		execl(SHELL, SHELL, "-c", *command, *arguments, NULL);
		// TODO: Try This: execl(command, arguments, NULL);
		perror(strcb("Failed to start execution of ", *command));
		_exit( EXIT_FAILURE );
	}
	else if ( ps->pid < 0 ) {
		// The fork failed. Report failure.
		statusupdate = -1;
	}
	else {
		// This is the parent process. Wait for the child to complete.
		if ( waitpid(ps->pid, &(ps->status), 0) != ps->pid ) {
			statusupdate = -1;
		}
	}

	return statusupdate;
}


//int process_status();
//int process_abort();


int job_is_stopped(job *j)
{
	process *p;
	// Asking every process in job-chain till we find one which is still running.
	for ( p = j->first_process; p; p = p->next ) {
		if ( !p->completed && !p->stopped ) {
			return ( 0 );
		}
	}
	return ( 1 );
}


int job_is_completed(job *j)
{
	process *p;
	// At least one process not completed yet means its job isn't completed too.
	for ( p = j->first_process; p; p = p->next ) {
		if ( !p->completed ) {
			return ( 0 );
		}
	}
	return ( 1 );
}
// \}
