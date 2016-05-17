#ifndef LandOfMagicGameHeader
#define LandOfMagicGameHeader

#ifdef __cplusplus
extern "C"
{
#endif

#include "n_common.h"
#include "n_log.h"

    #define size 60 
	#define fake_zz 15 
	#define fake_z 10 
    #define timespan 300000 


	typedef struct LEVEL_CELL
	{
		int type , /* earth, water , fire , air */
			selected , /* display status. normal (0) or selected (1) */
			active ;   /* cell status. all the above isn't checked if the cell have been inactivated */
	}LEVEL_CELL ;

	typedef struct LEVEL
	{
		/* array for cells */
		LEVEL_CELL ***board ;

		int x , /* size x */
			y , /* size y */
			z ; /* size z */

	} LEVEL ;

LEVEL *new_level( int sx , int sy , int sz );
int check_level_state( LEVEL *level );
int free_level( LEVEL *level );

#ifdef __cplusplus
}
#endif

#endif
