/*\file n_log.h
 * generic log system 
 *\author Mickael Castagnier
 *\date 2013-04-15 
 */
#ifndef __LOG_HEADER_GUARD__ 
#define __LOG_HEADER_GUARD__

#ifdef __cplusplus
extern "C"
{
#endif

/**\defgroup LOG LOGGING: logging to console, to file, to syslog under *x
   \addtogroup LOG 
  @{
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include "n_common.h"

#if !defined(NOSYSJRNL) && ( defined( LINUX ) || defined( SOLARIS ) )
#include <sysjrnl.h>
#endif

   /*! no log output */
#define NOLOG -1

   /*! syslog journal output if available (*nix only) */
#define SYSJRNL -2

#if defined( LINUX ) || defined( SOLARIS )

#include <syslog.h> 
#include <pthread.h>

#ifdef SOLARIS
#include <sys/varargs.h>
#else
#include <stdarg.h>
#endif

#else

#include <stdarg.h>
#include <pthread.h>

   /*! system is unusable */
#define LOG_EMERG       0       
   /*! action must be taken immediately */
#define LOG_ALERT       1       
   /*! critical conditions */
#define LOG_CRIT        2       
   /*! error conditions */
#define LOG_ERR         3       
   /*! warning conditions */
#define LOG_WARNING     4       
   /*! normal but significant condition */
#define LOG_NOTICE      5       
   /*! informational */
#define LOG_INFO        6       
   /*! debug-level messages */
#define LOG_DEBUG       7       

#endif



   /*! Logging function wrapper to get line and func */
#define n_log( __LEVEL__ , ... ) \
   do \
   { \
      _n_log( __LEVEL__ , __FILE__ , __func__ , __LINE__ , __VA_ARGS__ ); \
   }while( 0 )


   /*! ThreadSafe LOGging structure */
   typedef struct TS_LOG
   {
      /*! mutex for thread-safe writting */
      pthread_mutex_t LOG_MUTEX;
      /*! File handler */
      FILE *file;
   } TS_LOG;


   /* Set global log level. Possible values: NOLOG , NOTICE , VERBOSE , ERROR , DEBUG */
   void set_log_level( const int log_level );
   /* Return the global log level. Possible values: NOLOG , NOTICE , VERBOSE , ERROR , DEBUG */
   int get_log_level(); 
   /* set a file as standard log output */
   int set_log_file( char *file );
   /* get the log gile name if any */
   FILE *get_log_file();
   /* Full log function. Muste be wrapped in a MACRO to get the correct file-func-line informations */
   void _n_log( int level , const char *file , const char *func , int line , const char *format , ... ) ;


   /* Open a thread-safe logging file */
   int open_safe_logging( TS_LOG **log , char *pathname , char *opt );
   /* write to a thread-safe logging file */
   int write_safe_log( TS_LOG *log , char *pat , ... );
   /* close a thread-safe logging file */
   int close_safe_logging( TS_LOG *log );
   
/*@}*/

#ifdef __cplusplus
}
#endif
#endif   
