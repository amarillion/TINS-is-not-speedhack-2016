#include "LOMGame.h"


/* return TRUE if the level is finished */
int check_level_state( LEVEL *level )
{
	n_assert( level , return FALSE );
	for( int x = 0 ; x < level -> x ; x ++ )
	{
		for( int y = 0 ; y < level -> y ; y ++ )
		{
			for( int z = 0 ; z < level -> z ; z ++ )
			{
				if( level -> board[ x ][ y ][ z ] . active == 1 )
					return FALSE ;
			}
		}
	}
	return TRUE ;
} /* check_level_state */




int free_level( LEVEL *level )
{
	n_assert( level , return FALSE );
	for( int x = 0 ; x < level -> x ; x ++ )
	{
		for( int y = 0 ; y < level -> y ; y ++ )
		{
			Free( level -> board[ x ][ y ] );
		}
		Free( level -> board[ x ] );
	}
	Free( level -> board );
	Free( level );
	return TRUE ;
}



LEVEL *new_level( int sx , int sy , int sz )
{
	LEVEL *level = NULL ;
	Malloc( level , LEVEL , 1 );
	n_assert( level , return NULL );
	level -> x = sx ;
	level -> y = sy ;
	level -> z = sz ;

	level -> board = (LEVEL_CELL ***)malloc( sizeof( LEVEL_CELL ** ) * sx );
	n_assert( level -> board , return NULL );

	for( int x = 0 ; x < level -> x ; x++ )
	{
		level -> board[ x ] = (LEVEL_CELL **)malloc( sizeof( LEVEL_CELL * ) * sy );
		for( int y = 0 ; y < level -> y ; y++ )
		{
			level -> board[ x ][ y ] = (LEVEL_CELL *)malloc( sizeof( LEVEL_CELL ) * sz );
		}
	}

	for( int x = 0 ; x < level -> x ; x++ )
	{
		for( int y = 0 ; y < level -> y ; y++ )
		{
			for( int z = 0 ; z < level -> z ; z++ )
			{
				level -> board[ x ][ y ][ z ] . active = 0 ;
				level -> board[ x ][ y ][ z ] . type = 0 ;
				level -> board[ x ][ y ][ z ] . selected = 0 ;
			}
		}
	}


	int nb_kanjis = ( sx * sy * sz ) / 2 ; 
	int type_count = 0 ;
	int type = 0 ;
	type = rand()%5 ;
	for( int it = 0 ; it < nb_kanjis ; it ++ )
	{
		if( type_count > 1 )
		{
			type_count = 0 ;
			type = rand()%5 ;
		}
		type_count ++ ;

		int done = 0 ;
		while( !done )
		{
			int x = rand()%level -> x ;
			int y = rand()%level -> y ;
			int z_it = 0 ; 
			while( z_it < level -> z )
			{
				if( level -> board[ x ][ y ][ z_it ] . active == 0 )
				{
					/* don't put same type on pile */
					if( z_it > 0 && level -> board[ x ][ y ][ z_it - 1 ] . type == level -> board[ x ][ y ][ z_it ] . type )
						break ;
					level -> board[ x ][ y ][ z_it ] . active = 1 ;
					level -> board[ x ][ y ][ z_it ] . type = type ;
					level -> board[ x ][ y ][ z_it ] . selected = 0 ;
					done = 1 ;
					break ;
				}
				z_it ++ ;
			}
		}
	}
	return level ;
}
