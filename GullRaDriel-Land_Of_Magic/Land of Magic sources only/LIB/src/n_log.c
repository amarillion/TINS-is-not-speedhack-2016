/*\file n_log.c
 * generic logging system
 *\author Mickael Castagnier>
 *\date 2013-03-12
 */

#include "n_log.h"

/*! static global maximum wanted log level value */
static int LOG_LEVEL = NOLOG ;

/*! static FILE handling if logging to file is enabled */
static FILE *log_file = NULL ;


/*!\fn void set_log_level( int log_level )
 *\brief Set the global log level value ( static int LOG_LEVEL )
 *\param log_level Log level value. Supported: JRNLONLY,NOLOG,LOG_NOTICE/INFO/ERR/DEBUG
 */
void set_log_level( const int log_level )
{
   LOG_LEVEL = log_level ;
} /* set_log_level() */ 



/*!\fn int get_log_level( )
 *\brief Get the global log level value
 *\return static int LOG_LEVEL
 */
int get_log_level()
{
   return LOG_LEVEL ;
} /* get_log_level() */



/*!\fn int set_log_file( char *file )
 *\brief Set the logging to a file instead of stderr
 *\return TRUE or FALSE
 */
int set_log_file( char *file )
{	
   n_assert( file , return FALSE );

   if( !log_file )
      log_file = fopen( file , "w" );
   else
   {
      fclose( log_file );
      log_file = fopen( file , "w" );
   }

   n_assert( log_file , return FALSE );

   return TRUE ;
} /* set_log_file */



/*!\fn FILE *get_log_file()
 *\brief return the current log_file
 *\return a valid FILE handle or NULL 
 */
FILE *get_log_file()
{
   return log_file ;
} /*get_log_level() */



/*!\fn void _n_log( int level , const char *file , const char *func , int line , const char *format , ... ) 
 *\brief Logging function. log( level , const char *format , ... ) is a macro around _log
 *\param level Logging level 
 *\param file File containing the emmited log
 *\param func Function emmiting the log
 *\param line Line of the log
 *\param format Format and string of the log, printf style
 */
void _n_log( int level , const char *file , const char *func , int line , const char *format , ... ) 
{
   va_list args ;

   FILE *out = NULL ;

   if( !log_file )
      out = stderr ;
   else
      out = log_file ;

   int log_level = get_log_level();

   if( log_level != SYSJRNL ) 
   {
      switch( level )
      {
         case LOG_DEBUG:
            if( log_level >= LOG_DEBUG ) 
            { 
               fprintf( out , "DEBUG:%ld %s->%s:%d " , time( NULL ) , file , func , line  ); 
               va_start (args, format);
               vfprintf( out, format , args ); 
               va_end( args );
               fprintf( out, "\n" ); 
            }	 
            break ;
         case LOG_ERR:
            if( log_level >= LOG_ERR ) 
            { 
               fprintf( out , "ERROR: %s->%s:%d " , file , func , line ); 
               va_start (args, format);
               vfprintf( out, format , args ); 
               va_end( args );
               fprintf( out, "\n" ); 
            }
            break ;
         case LOG_INFO:
            if( log_level >= LOG_INFO ) 
            { 
               fprintf( out , "INFO: %s->%s:%d " , file , func , line ); 
               va_start (args, format);
               vfprintf( out, format , args ); 
               va_end( args );
               fprintf( out , "\n" ); 
            }
            break ;
         case LOG_NOTICE:
            if( log_level >= LOG_NOTICE ) 
            { 
               fprintf( out , "NOTICE: %s->%s:%d " , file , func , line ); 
               va_start (args, format);
               vfprintf( out, format , args ); 
               va_end( args );
               fprintf( out, "\n" ); 
            }
            break ;
         case LOG_WARNING:
            if( log_level >= LOG_WARNING ) 
            { 
               fprintf( out , "WARNING: %s->%s:%d " , file , func , line ); 
               va_start (args, format);
               vfprintf( out, format , args ); 
               va_end( args );
               fprintf( out, "\n" ); 
            }
            break ;
         case LOG_CRIT:
            if( log_level >= LOG_CRIT ) 
            { 
               fprintf( out , "CRITICAL: %s->%s:%d " , file , func , line ); 
               va_start (args, format);
               vfprintf( out, format , args ); 
               va_end( args );
               fprintf( out, "\n" ); 
            }
            break ;
         case LOG_ALERT:
            if( log_level >= LOG_ALERT ) 
            { 
               fprintf( out , "ALERT: %s->%s:%d " , file , func , line ); 
               va_start (args, format);
               vfprintf( out, format , args ); 
               va_end( args );
               fprintf( out, "\n" ); 
            }
            break ;
         case LOG_EMERG:
            if( log_level >= LOG_EMERG ) 
            { 
               fprintf( out , "EMERG: %s->%s:%d " , file , func , line ); 
               va_start (args, format);
               vfprintf( out, format , args ); 
               va_end( args );
               fprintf( out, "\n" ); 
            }
            break ;
         case NOLOG:
            break;
         default:
            fprintf( out , "UNKNOW_LOG_LEVEL: %s->%s:%d " , file , func , line ); 
            va_start (args, format);
            vfprintf( out, format , args ); 
            va_end( args );
            fprintf( out, "\n" ); 
            break ;
      }
   }
#if defined( LINUX ) || defined( SOLARIS )
   else 
   { 
      char *syslogbuffer = NULL ;
      va_start (args, format);
      vasprintf( &syslogbuffer , format , args ); 
      va_end( args );

#ifndef NOSYSJRNL
      if( level == LOG_INFO ) 
         sysjrnl( LOG_INFO , "%s->%s:%d %s" , file , func , line , syslogbuffer ); 
      if( level == LOG_NOTICE ) 
         sysjrnl( LOG_NOTICE , "%s->%s:%d %s" , file , func , line , syslogbuffer ); 
      if( level == LOG_WARNING ) 
         sysjrnl( LOG_WARNING , "%s->%s:%d %s" , file , func , line , syslogbuffer ); 
      if( level == LOG_ERR ) 
         sysjrnl( LOG_ERR  , "%s->%s:%d %s" , file , func , line , syslogbuffer ); 
      if( level == LOG_CRIT ) 
         sysjrnl( LOG_CRIT  , "%s->%s:%d %s" , file , func , line , syslogbuffer ); 
      if( level == LOG_ALERT ) 
         sysjrnl( LOG_ALERT  , "%s->%s:%d %s" , file , func , line , syslogbuffer ); 
      if( level == LOG_EMERG ) 
         sysjrnl( LOG_EMERG  , "%s->%s:%d %s" , file , func , line , syslogbuffer ); 
#endif
      Free( syslogbuffer );   
   }
#endif
}  /* _n_log( ... ) */



/*!\fn open_safe_logging( TS_LOG **log , char *pathname , char *opt )
 *\brief Open a thread-safe logging file
 *\param log A TS_LOG handler
 *\param pathname The file path (if any) and name
 *\param opt Options for opening (please never forget to use "w")
 *\return TRUE on success , FALSE on error , -1000 if already open
 */
int open_safe_logging( TS_LOG **log , char *pathname , char *opt ) 
{

   if( (*log) ){

      if( (*log) -> file )
         return -1000; /* already open */

      return FALSE;
   }

   if( !pathname ) return FALSE; /* no path/filename */

   Malloc( (*log) , TS_LOG , 1 );

   if( !(*log) )
      return FALSE;

   pthread_mutex_init( &(*log) -> LOG_MUTEX , NULL );

   (*log) -> file = fopen( pathname , opt );

   if( !(*log) -> file )
      return FALSE;

   return TRUE;

}/* open_safe_logging(...) */



/*!\fn write_safe_log( TS_LOG *log , char *pat , ... )
 *\brief write to a thread-safe logging file
 *\param log A TS_LOG handler
 *\param pat Pattern for writting (i.e "%d %d %s")
 *\return TRUE on success, FALSE on error
 */
int write_safe_log( TS_LOG *log , char *pat , ... ) 
{
   /* argument list */
   va_list arg;
   char str[2048];

   if( !log )
      return FALSE;

   va_start( arg , pat );

   snprintf( str , sizeof(str), pat, arg );

   va_end( arg );

   pthread_mutex_lock( &log -> LOG_MUTEX );

   fprintf( log -> file , "%s" , str );

   pthread_mutex_unlock( &log -> LOG_MUTEX );

   return TRUE;

} /* write_safe_log( ... ) */



/*!\fn close_safe_logging( TS_LOG *log )
 *\brief close a thread-safe logging file
 *\param log A TS_LOG handler
 *\return TRUE on success, FALSE on error
 */
int close_safe_logging( TS_LOG *log )
{
   if( !log )
      return FALSE;

   pthread_mutex_destroy( &log -> LOG_MUTEX );

   fclose( log -> file );

   return TRUE;

}/* close_safe_logging( ...) */
