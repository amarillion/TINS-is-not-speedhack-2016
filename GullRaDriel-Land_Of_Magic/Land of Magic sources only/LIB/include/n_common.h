/**\file n_common.h
 *  common headers and low-level hugly functions & define
 *\author Castagnier Mickael
 *\version 1.0
 *\date 24/03/05
 */

#ifndef __COMMON_FOR_C_IMPLEMENTATION__
#define __COMMON_FOR_C_IMPLEMENTATION__

#ifdef __cplusplus
extern "C"
{
#endif

   /**\defgroup COMMONS COMMONS: generally used headers, defines, timers, allocators,...
     \addtogroup COMMONS
     @{
     */

   /*! feature test macro */
#define __EXTENSIONS__

   /* common headers */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <limits.h>


/*! FORCE_INLINE portable macro */
#if defined( _MSC_VER )
   #define FORCE_INLINE    __forceinline
#elif defined( __linux__ ) || defined( LINUX ) || defined ( __MINGW32__ )
   #define FORCE_INLINE inline __attribute__((always_inline))
#elif defined(SOLARIS)
   #define FORCE_INLINE __attribute__((always_inline))
#endif

   /* cross compability */
#if defined( LINUX ) || defined( SOLARIS ) || defined( AIX )

#include <sys/time.h>
#ifndef NOALLEGRO
#ifndef  ALLEGRO_ALREADY_INCLUDED
   /*! guard for mutliple allegro inclusion. Not that allegro's header would fail, but there would be multiple FONT declaration.*/
#define ALLEGRO_ALREADY_INLUDED 
#include <allegro.h>

   /*! default font for our usage */
   FONT *allegro_font;
#endif
#endif   
#ifdef LINUX
#include <linux/limits.h>
#include <pthread.h>
#endif

#else /* WIN32 */  

#include "windows.h"

#include <time.h>
#ifndef NOALLEGRO
#ifndef  ALLEGRO_ALREADY_INCLUDED
   /*! guard for mutliple allegro inclusion. Not that allegro's header would fail, but there would be multiple FONT declaration.*/
#define ALLEGRO_ALREADY_INLUDED 
#include <allegro.h>
#include <winalleg.h>
   /*! default font for our usage */
   FONT *allegro_font;
#endif
#endif
#include <pthread.h>
   /*#define opendir __mingw_opendir*/

#endif

   /*! define true */
#ifndef true
#define true (1==1)
#endif

   /*! define TRUE */
#ifndef TRUE
#define TRUE true
#endif

   /*! define false */
#ifndef false
#define false (1==0)
#endif

   /*! define FALSE */
#ifndef FALSE
#define FALSE false
#endif

   /*!  returned by N_STRLIST functions to tell the caller that the list is empty */
#ifndef EMPTY
#define EMPTY 2
#endif

   /*! String or "NULL" string for logging purposes */	
#define _str( __PTR ) ((__PTR)?(__PTR):"NULL")
   /*! String or " " string for config purposes */	
#define _strw( __PTR ) ((__PTR)?(__PTR):" ")
   /*! N_STR or "NULL" string for logging purposes */	
#define _nstr( __PTR ) ((__PTR&&__PTR->data)?(__PTR->data):"NULL")

   /*! Malloc Handler to get errors and set to 0 */
#define Malloc( __ptr , __struct , __size ) \
   if ( !(  __ptr  = (  __struct  *)calloc(  __size  , sizeof(  __struct  ) ) ) )   \
   {                                                                                 \
      n_log( LOG_ERR , "( %s *)malloc( %s * sizeof( %d ) ) Error at line %d of %s \n", #__ptr , #__struct , __size , __LINE__ , __FILE__); \
   } 

   /*! Malloca Handler to get errors and set to 0 */
#define Alloca( __ptr , __size ) \
   __ptr = alloca( __size ); \
   if( ! __ptr ) \
   { \
      n_log( LOG_ERR , "%s=alloca( %d ) Error at line %d of %s \n", #__ptr , __size , __LINE__ , __FILE__); \
   } \
   else \
   memset( __ptr , 0 , __size ); 


   /*! Free Handler to get errors */
#define Free( __ptr ) \
   if (  __ptr  )\
   {\
      free(  __ptr  );\
      __ptr  = NULL;\
   }\
   else\
   {\
      n_log( LOG_DEBUG , "Free( %s ) already done or NULL at line %d of %s \n", #__ptr , __LINE__ , __FILE__ );\
   }
   /*! Free Handler without log */
#define FreeNoLog( __ptr )\
   if (  __ptr  )\
   {\
      free(  __ptr  );\
      __ptr  = NULL;\
   }  		


   /*! Realloc Handler to get errors */
#define Realloc( __ptr, __struct , __size )  \
   if( !(  __ptr  = (  __struct  *)realloc(  __ptr , __size  * sizeof(  __struct  ) ) ) )\
   { \
      n_log( LOG_ERR , "( %s *)malloc( %s * sizeof( %d ) ) Error at line %d of %s \n", #__ptr , #__struct , __size , __LINE__ , __FILE__);\
      __ptr = NULL;\
   } 

   /*! Realloc + zero new memory zone Handler to get errors */
#define Reallocz( __ptr, __struct , __old_size , __size )  \
   if ( !(  __ptr  = (  __struct  *)realloc(  __ptr , __size  * sizeof(  __struct  ) ) ) )\
   {\
      n_log( LOG_ERR , "( %s *)malloc( %s * sizeof( %d ) ) Error at line %d of %s \n", #__ptr , #__struct , __size , __LINE__ , __FILE__);\
      __ptr = NULL;\
   }\
   else\
   {\
      if( __size > __old_size )memset( ( __ptr + __old_size ) , 0 , __size - __old_size );\
   }

   /*! macro to assert things */
#define n_assert( __ptr , __ret ) \
   if( !(__ptr) ) \
   { \
      n_log( LOG_DEBUG , "%s is NULL at line %d of %s" , #__ptr ,  __LINE__ , __FILE__ );\
      __ret ; \
   }

   
   /*! init error checking in a function */
#define init_error_check() \
   static int ___error__check_flag = FALSE ;

   /*! error checker type if( !toto ) */
#define ifnull if( ! 

   /*! error checker type if( 0 != toto ) */
#define ifzero if( 0 == 
   
   /*! error checker type if( toto == FALSE )  */
#define iffalse if( FALSE == 

   /*! check for errors */
#define checkerror() if( ___error__check_flag == TRUE ) \
   { n_log( LOG_ERR , "checkerror return false at line %d of %s" , __LINE__ , __FILE__ ); \
   goto error ; \
   }

/*! close a ifwhatever block */
#define endif ){ ___error__check_flag = TRUE ; n_log( LOG_ERR , "First err was at line %d of %s" , __LINE__ , __FILE__ );} 

   /*! pop up errors if any */
#define get_error() \
   (___error__check_flag == TRUE) 	

   /*! shortener for if( a cond b ) a = b  */
#define equal_if( __var_a , __cond , __var_b ) if( (__var_a) __cond (__var_b) ) __var_a = (__var_b) 

   /*! Macro for initializing a rwlock */
#define init_lock( __rwlock_mutex ) \
   pthread_rwlock_init( &(__rwlock_mutex) , NULL )
   /*! Macro for acquiring a read lock on a rwlock mutex */
#define read_lock( __rwlock_mutex ) \
   pthread_rwlock_rdlock( &(__rwlock_mutex) )
   /*! Macro for acquiring a write lock on a rwlock mutex */
#define write_lock( __rwlock_mutex ) \
   pthread_rwlock_wrlock( &(__rwlock_mutex) )
   /*! Macro for releasing read/write lock a rwlock mutex */
#define unlock( __rwlock_mutex ) \
   pthread_rwlock_unlock( &(__rwlock_mutex) )

#if !defined( LINUX ) && !defined( SOLARIS )
   /* typedefine for unsigned category for basic native types */
   /*! shortcut for unsigned int*/
   typedef unsigned int uint;
   /*! shortcut for unsigned long*/
   typedef unsigned long ulong;
   /*! shortcut for unsigned short*/
   typedef unsigned short ushort;
   /*! shortcut for unsigned char*/
   typedef unsigned char uchar;
#endif

   /*! next odd helper */
#define next_odd( __val ) ( (__val)%2 == 0 ) ? (__val) : ( __val + 1 ) 

   /*! Flag for SET something , passing as a function parameter */
#define SET        1234
   /*! Flag for GET something , passing as a function parameter */
#define GET        4321
   /*! Default APP_STATUS Value */
#define DEFAULT    1000
   /*! Value of the state of an application who is running */
#define RUNNING    1001
   /*! Value of the state of an application who want to stop his activity */
#define STOPWANTED 1002
   /*! Value of the state of an application who is stopped */
#define STOPPED    1003
   /*! Value of the state of an application who is paused */
#define PAUSED     1004

   /*! Initialize the random sequence with time */
#define randomize() { srand((unsigned)time(NULL)); rand(); }

#ifndef MIN
   /*! define MIN macro */
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
   /*! define MIN macro */
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

   /*! Timing Structure */
   typedef struct N_TIME
   {
      /*! time since last poll */
      time_t delta;
#if !defined( LINUX ) && !defined( SOLARIS ) && !defined( AIX )
      /*! queryperformancefrequency storage */
      LARGE_INTEGER freq;
      /*! start time W32*/
      LARGE_INTEGER startTime;
      /*! current time W32*/
      LARGE_INTEGER currentTime;
#else
      /*! start time */
      struct timeval startTime;
      /*! start time */
      struct timeval currentTime;
#endif
   } N_TIME;

   /* for the 'press a key to continue' */
   void PAUSE();

   /* Init or restart from zero any N_TIME HiTimer */
   int start_HiTimer( N_TIME *timer );

   /* Poll any N_TIME HiTimer, returning usec */
   time_t get_usec( N_TIME *timer );

   /* Poll any N_TIME HiTimer, returning msec */
   time_t get_msec( N_TIME *timer );

   /* Poll any N_TIME HiTimer, returning sec */
   time_t get_sec( N_TIME *timer );


#ifndef NOALLEGRO
   /* Fill a keyboard buffer */
   void get_keyboard( char *keybuf , int *cur , int min , int max );
#include "lexmenu.h"	
#endif

   /*@}*/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __COMMON_FOR_C_IMPLEMENTATION__ */
