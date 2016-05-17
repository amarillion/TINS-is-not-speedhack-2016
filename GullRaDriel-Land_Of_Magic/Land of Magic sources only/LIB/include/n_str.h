/**\file n_str.h
 *  N_STR and string function declaration
 *\author Castagnier Micka�l
 *\version 2.0
 *\date 05/02/14
 */

#ifndef N_STRFUNC
#define N_STRFUNC

#ifdef __cplusplus
extern "C"
{
#endif

	/**\defgroup N_STR N_STR: replacement to char *strings with dynamic resizing and boundary checking
	  \addtogroup N_STR 
	  @{
	  */

#ifdef LINUX
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>

#include "n_common.h"
#include "n_log.h"
#include "n_list.h"

/*! local strdup */
#define local_strdup( __src_ )\
	({ \
	 char *str = NULL ;\
	 int len = strlen( (__src_) );\
	 Malloc( str , char , len + 5 );\
	 if( !str )\
	 { \
	 n_log( LOG_ERR , "Couldn't allocate %d byte for duplicating \"%s\"" , (__src_) );\
	 }\
	 else\
	 {\
	 for( int it = 0 ; it <= len ; it ++ )\
	 {\
	 str[ it ] = (__src_)[ it ] ;\
	 }\
	 }\
	 str ;\
    })


/*! Abort code to sped up pattern matching. Special thanks to Lars Mathiesen <thorinn@diku.dk> for the ABORT code.*/
#define WILDMAT_ABORT -2
/*! What character marks an inverted character class? */
#define WILDMAT_NEGATE_CLASS '^'
/*! Do tar(1) matching rules, which ignore a trailing slash? */
#undef WILDMAT_MATCH_TAR_PATTERN


/*! Macro to quickly realloc a n_str */
#define resize_nstr( __nstr , new_size ) \
	({ \
	 if( __nstr && __nstr -> data ) \
	 { \
	 Reallocz( __nstr -> data , char , __nstr -> length , __nstr -> length + needed ); \
	 __nstr -> length += needed ; \
	 } \
	 })



/*! Macro to quickly allocate and sprintf to a new n_str */
#define strprintf( __n_var , ... ) \
	({ \
	 if( !__n_var ) \
	 { \
	 NSTRBYTE needed = 0 ; \
	 needed = snprintf( NULL , 0 ,  __VA_ARGS__ ); \
	 Malloc( __n_var , char , needed + 2 );\
	 if( __n_var )\
	 { \
	 snprintf( __n_var , needed + 1 , __VA_ARGS__ ); \
	 } \
	 } \
	 else \
	 { \
	 n_log( LOG_ERR , "%s is already allocated." , "#__n_var" ); \
	 } \
	 __n_var ; \
	 })

#define nstrprintf( __nstr_var , ... ) \
	({ \
	 if( __nstr_var ) \
	 { \
      free_nstr( &__nstr_var ); \
    }\
	 NSTRBYTE needed = 0 ; \
	 needed = snprintf( NULL , 0 ,  __VA_ARGS__ ); \
	 __nstr_var = new_nstr( needed + 2 ); \
	 if( __nstr_var )\
	 { \
	 snprintf( __nstr_var -> data , needed + 1 , __VA_ARGS__ ); \
	 __nstr_var -> written = needed ; \
	 } \
	 __nstr_var ; \
	 })


#define nstrprintf_cat( __nstr_var , ... ) \
	({ \
	 if( __nstr_var ) \
	 { \
	 NSTRBYTE needed = 0 ; \
	 needed = snprintf( NULL , 0 ,  __VA_ARGS__ ); \
	 if( ( __nstr_var -> written + needed ) >= __nstr_var -> length ) \
	 { \
	 resize_nstr( __nstr_var , __nstr_var -> length + needed + 2 ); \
	 } \
	 snprintf( __nstr_var -> data + __nstr_var -> written , needed + 1 , __VA_ARGS__ ); \
	 __nstr_var -> written += needed ; \
	 } \
	 else \
		 { \
			 nstrprintf( __nstr_var , __VA_ARGS__ ); \
		 } \
		 __nstr_var ; \
	 } )


#ifdef LINUX
#include <inttypes.h>
#endif
		/*! N_STR base unit */
		typedef int32_t NSTRBYTE;

		/*! A double linked box including a string and his lenght */
		typedef struct N_STR
		{
			/*! the string */
			char *data;
			/*! length of string (in case we wanna keep information after the 0 end of string value) */
			NSTRBYTE length ;
			/*! size of the written data inside the string */
			NSTRBYTE written ;          
		} N_STR;

      char *trim_nocopy(char *s);
      char *nfgets( char *buffer , int size , FILE *stream );

		/* create a new string */
		N_STR *new_nstr( NSTRBYTE size );   
		/* reinitialize a nstr */
		int empty_nstr( N_STR *nstr ); 
		/* Make a copy of a N_STR */
		N_STR *nstrdup( N_STR *msg );
		/* Convert a char into a N_STR */
		int char_to_nstr_ex( char *from , NSTRBYTE nboct , N_STR **to );
		/* Convert a char into a N_STR, shorter version */
		N_STR *char_to_nstr( char *src );
		/* cat data inside a N8STR */
		int nstrcat_ex( N_STR *dest , void *src , NSTRBYTE size , NSTRBYTE blk_size ,int resize_flag );
		/* Wrapper to nstrcat_ex to concatenate N_STR *datas */
		int nstrcat( N_STR *dst , N_STR *src );
		/* Wrapper to nstrcat_ex to concatenate void *data */
		int nstrcat_bytes_ex( N_STR *dest , void *src , NSTRBYTE size );
		/* Wrapper to nstrcat_bytes_ex to cat null termined data streams */
		int nstrcat_bytes( N_STR *dest , void *src );
		/* Load a whole file into a N_STR. Be aware of the (4GB ||System Memory) limit */
		N_STR *file_to_nstr( char *filename );
		/* Write a whole N_STR into a file */
		int nstr_to_file( N_STR *n_str , char *filename );
		/* Free a N_STR */
		void free_nstr_ptr( void *ptr );
		/* Free a N_STR structure and set it to NULL */
		int free_nstr( N_STR **ptr );
		/* Free a N_STR */
		void free_nstr_ptr_nolog( void *ptr );
		/* Free a N_STR structure and set it to NULL */
		int free_nstr_nolog( N_STR **ptr );

		/* String to long integer, with error checking */
		int str_to_long_ex( const char *s , NSTRBYTE start , NSTRBYTE end , long int *i, const int base );
		/* String to long integer, shorter version */ 
		int str_to_long(  const char *s , long int *i,  const int base);

		/* String to integer, with error checking */
		int str_to_int_ex( const char *s , NSTRBYTE start , NSTRBYTE end , int *i, const int base );
		/* String to integer, shorter version */ 
		int str_to_int(  const char *s , int *i,  const int base);

		/* Skip character from string while string[iterator] == toskip step inc */
		int skipw( char *string , char toskip , NSTRBYTE *iterator , int inc );
		/* Skip character from string until string[iterator] == toskip step inc */
		int skipu( char *string , char toskip , NSTRBYTE *iterator , int inc );
		/* Upper case a string */
		int strup( char *string , char *dest );
		/* Lower case a string */
		int strlo( char *string , char *dest );
		/* Copy from string to dest until from[ iterator ] == split */
		int strcpy_u( char *from , char *to , NSTRBYTE to_size , char split , NSTRBYTE *it );
		/* Return an array of char pointer to the splitted section */
		char **split( char* chaine , const char* delim , int vide );
		/* Count split elements */
		int split_count( char **split_result );
		/* Free a char **tab and set it to NULL */
		int free_split_result( char ***tab );
		/* Write and fit into the char array */
		int write_and_fit( char **dest , NSTRBYTE *size , NSTRBYTE *written , char *src );
		/* get a list of the file in a directory */
		int scan_dir( const char *dir, LIST *result , const int recurse );
		/* get a list of the file in a directory, extented N_STR version */
   int scan_dir_ex( const char *dir, const char *pattern , LIST *result , const int recurse , const int mode );
		/* pattern matching */
		int wildmat( register const char *text, register const char *p );
		/* pattern matching case insensitive */
		int wildmatcase( register const char *text, register const char *p );

		/* return a replaced string */
		char *str_replace ( const char *string, const char *substr, const char *replacement );

		/*@}*/
#ifdef __cplusplus
		}
#endif
		/* #ifndef N_STR*/
#endif 
