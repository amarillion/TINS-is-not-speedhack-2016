/**\file n_particles.c
 *
 *  Particle functions
 *
 *\author Castagnier Mickaël aka Gull Ra Driel
 *
 *\version 1.0
 *
 *\date 15/12/2007
 *
 */


#include "n_particles.h"



int init_particle_system( PARTICLE_SYSTEM **psys , int max , double x , double y , double z )
{
	Malloc( (*psys) , PARTICLE_SYSTEM , 1 );
	if( !(*psys) )
		return FALSE ;

	(*psys) -> list_start = (*psys) -> list_end = NULL ;

	start_HiTimer( &(*psys) -> timer );

	(*psys) -> max_particles = max ;
	(*psys) -> nb_particles  = 0 ;
	(*psys) -> x = x ;
	(*psys) -> y = y ;
	(*psys) -> z = z ;

	return TRUE ;
}

int add_particle( PARTICLE_SYSTEM *psys , ALLEGRO_BITMAP *spr , int mode , int off_x , int off_y , int lifetime , int color ,
		double vx , double vy , double vz ,
		double ax , double ay , double az )
{
	if( psys -> nb_particles >= psys -> max_particles )
		return FALSE ;

	if( !psys -> list_start ) /* no particles */
	{
		Malloc( psys -> list_start , PARTICLE , 1 );

		psys -> list_start -> prev = psys -> list_start -> next = NULL ;
		psys -> list_end = psys -> list_start ;

		psys -> list_start -> sprite = spr ;
		psys -> list_start -> mode = mode ;
		psys -> list_start -> off_x = off_x ;
		psys -> list_start -> off_y = off_y ;
		psys -> list_start -> lifetime = lifetime ;
		psys -> list_start -> x = psys -> x + off_x ;
		psys -> list_start -> y = psys -> y + off_y ;
		psys -> list_start -> z = psys -> z ;
		psys -> list_start -> ax = ax ;
		psys -> list_start -> ay = ay ;
		psys -> list_start -> az = az ;
		psys -> list_start -> vx = vx ;
		psys -> list_start -> vy = vy ;
		psys -> list_start -> vz = vz ;
		psys -> list_start -> color = color ;

		psys -> nb_particles ++ ;

		return TRUE;
	}

	Malloc( psys -> list_end -> next , PARTICLE , 1 );
	psys -> list_end -> next -> prev = psys -> list_end ;
	psys -> list_end -> next -> next = NULL ;
	psys -> list_end = psys -> list_end -> next ;
	psys -> list_end -> sprite = spr ;
	psys -> list_end -> mode = mode ;
	psys -> list_end -> off_x = off_x ;
	psys -> list_end -> off_y = off_y ;
	psys -> list_end -> lifetime = lifetime ;
	psys -> list_end -> x = psys -> x + off_x ;
	psys -> list_end -> y = psys -> y + off_y ;
	psys -> list_end -> z = psys -> z ;
	psys -> list_end -> ax = ax ;
	psys -> list_end -> ay = ay ;
	psys -> list_end -> az = az ;
	psys -> list_end -> vx = vx ;
	psys -> list_end -> vy = vy ;
	psys -> list_end -> vz = vz ;
	psys -> list_end -> color = color ;

	psys -> nb_particles ++ ;

	return TRUE ;

}

int manage_particle( PARTICLE_SYSTEM *psys)
{
	PARTICLE *ptr = NULL , *next = NULL , *prev = NULL ;

	double tmp_v = 0 ;

	ptr = psys -> list_start ;

	double delta = 0 ;

	delta = get_msec( &psys -> timer ) ;

	while( ptr )
	{
		ptr -> lifetime -= delta ;

		if( ptr -> lifetime > 0 )
		{
			tmp_v = ptr -> vx + ( ptr -> ax * ((delta * delta)/1000) ) / 2 ;
			ptr -> x = ptr -> x + delta * ( ptr -> vx + tmp_v ) / 2000 ;
			ptr -> vx = tmp_v ;

			tmp_v = ptr -> vy + ( ptr -> ay * ((delta * delta)/1000) ) / 2 ;
			ptr -> y = ptr -> y + delta * ( ptr -> vy + tmp_v ) / 2000 ;
			ptr -> vy = tmp_v ;

			tmp_v = ptr -> vz + ( ptr -> az * ((delta * delta)/1000) ) / 2 ;
			ptr -> z = ptr -> z + delta * ( ptr -> vz + tmp_v ) / 2000 ;
			ptr -> vz = tmp_v ;

			ptr = ptr -> next ;
		}
		else
		{
			next = ptr -> next ;
			prev = ptr -> prev ;

			if( next )
			{
				if( prev )
					next -> prev = prev ;
				else
				{
					next -> prev = NULL ;
					psys -> list_start = next ;
				}
			}

			if( prev )
			{
				if( next )
					prev -> next = next ;
				else
				{
					prev -> next = NULL ;
					psys -> list_end = prev ;
				}
			}

			Free( ptr );
			psys -> nb_particles -- ;
			if( psys -> nb_particles <= 0 )
				psys -> list_start = psys -> list_end = NULL ;

			ptr = next ;
		}
	}

	return TRUE;
}

int draw_particle( PARTICLE_SYSTEM *psys )
{
	PARTICLE *ptr = NULL ;

	double x = 0 , y = 0 ;

	ptr = psys -> list_start ;

	while( ptr )
	{
		x = ptr -> x ;
		y = ptr -> y ;

		if( ptr -> mode == SINUS_PART )
		{
			if( ptr -> vx != 0 )
				x = x + ptr -> vx * sin( (ptr -> x/ptr -> vx) ) ;
			else
				x = x + ptr -> vx * sin( ptr -> x );
			if( ptr -> vy != 0 )
				y = y + ptr -> vy * cos( (ptr -> y/ptr -> vy) ) ;
			else
				y = y + ptr -> vy * sin( ptr -> y ) ;
		}

		al_draw_bitmap( ptr -> sprite , x - al_get_bitmap_width( ptr -> sprite ) / 2 , y -  al_get_bitmap_width( ptr -> sprite ) /2  , 0 );

		ptr = ptr -> next ;
	}

	return TRUE;
}

int free_particle( PARTICLE_SYSTEM *psys , PARTICLE **ptr )
{
	PARTICLE *next = NULL , *prev = NULL ;

	next = (*ptr) -> next ;
	prev = (*ptr) -> prev ;

	if( next )
	{
		if( prev )
			next -> prev = prev ;
		else
		{
			next -> prev = NULL ;
			psys -> list_start = next ;
		}
	}

	if( prev )
	{
		if( next )
			prev -> next = next ;
		else
		{
			prev -> next = NULL ;
			psys -> list_end = prev ;
		}
	}

	Free( (*ptr) );

	psys -> nb_particles -- ;

	if( psys -> nb_particles <= 0 )
		psys -> list_start = psys -> list_end = NULL ;

	(*ptr) = next ;

	return TRUE ;
} /* free_particle(...) */



int free_particle_system( PARTICLE_SYSTEM **psys)
{
	PARTICLE *ptr = NULL , *ptr_next = NULL ;

	ptr = (*psys) -> list_start ;

	while( ptr )
	{
		ptr_next = ptr -> next ;
		Free( ptr ) ;
		ptr = ptr_next ;
	}

	Free( (*psys) );

	return TRUE;
}

