#include "LOMGfx.h"



int draw_kanji( GLOBAL_INSTANCE *game , int x , int y , int type , int alpha )
{
	static ALLEGRO_USTR *kanjis[ 5 ]={ NULL , NULL , NULL , NULL , NULL };
	ALLEGRO_COLOR kanjis_color[ 5 ] ;

	if( type < 0 || type > 4 )
		return FALSE ;

	if( !kanjis[ 0 ] )
	{
		kanjis[ K_AIR   ] =  al_ustr_new( "空" );
		kanjis[ K_FIRE  ] =  al_ustr_new( "火" );
		kanjis[ K_EARTH ] =  al_ustr_new( "土" );
		kanjis[ K_WATER ] =  al_ustr_new( "水" );
		kanjis[ K_SNOW ] =  al_ustr_new( "雪" );
	}
	kanjis_color[ K_AIR   ]   =  al_map_rgba( 200 , 200 , 200 , alpha );
	kanjis_color[ K_FIRE   ]  =  al_map_rgba( 200 , 10  , 10  , alpha );
	kanjis_color[ K_EARTH   ] =  al_map_rgba( 88  , 41  , 5   , alpha );
	kanjis_color[ K_WATER   ] =  al_map_rgba( 0   , 0   , 200 , alpha );
	kanjis_color[ K_SNOW    ] =  al_map_rgba( 40   , 40   , 240 , alpha );
	al_draw_ustr( game -> font[ HUGE_FONT ] , kanjis_color[ type ] , x , y , 0 , kanjis[ type ] );

	return TRUE ;
} /* draw_kanji */


int draw_level( GLOBAL_INSTANCE *game )
{
	n_assert( game , return FALSE );

	/*static ALLEGRO_BITMAP *lvl_scrbuf = NULL ;
	  ALLEGRO_BITMAP *oldtarget = al_get_target_bitmap() ;
	  if( !lvl_scrbuf )
	  {
	  lvl_scrbuf = al_create_bitmap( game -> w, game -> h );
	  if( !lvl_scrbuf )
	  {
	  abort_game( "Unable to create lvl_scrbuf bitmap of size %d,%d" , game -> w, game -> h );
	  }   
	  }
	  al_clear_to_color( al_map_rgba( 0 , 0 , 0 , 155 ) );

	  al_set_target_bitmap( lvl_scrbuf ); */



	int wx = game -> w / 2 - ( game -> level -> x * size ) / 2 ;
	int wy = game -> h / 2 - ( game -> level -> y * size ) / 2 ;

	for( int z = game -> level -> z - 1 ; z >= 0 ; z -- )
	{
		for( int y = game -> level -> y - 1 ; y >= 0 ; y -- )
		{
			for( int x = game -> level -> x - 1 ; x >= 0 ; x -- )
			{
				if(  game -> level -> board[ x ][ y ][ z ] . active )
				{
					int alpha = 0 ;
					if( z != 0 )
					{
						if( game -> level -> board[ x ][ y ][ z - 1 ] . active == 0 )
							alpha = 255 ;
						else
							alpha = 255 * z / game -> level -> z ;
					}
					else
						alpha = 255 ;

					al_draw_filled_rectangle( wx + x * size - fake_zz * z , wy + y * size - fake_z * z , size + wx + x * size - fake_zz * z , size + wy + y * size - fake_z * z , al_map_rgba( 155 , 155 , 155 , alpha ) );
					if( game -> level -> board[ x ][ y ][ z ] . selected == 1 )
						al_draw_rectangle( wx + x * size - fake_zz * z , wy + y * size - fake_z * z , size + wx + x * size - fake_zz * z , size + wy + y * size - fake_z * z , al_map_rgba( 255 , 55 , 55 , alpha ) , 5.5 );
					al_draw_rectangle( wx + x * size - fake_zz * z , wy + y * size - fake_z * z , size + wx + x * size - fake_zz * z , size + wy + y * size - fake_z * z , al_map_rgba( 55 , 55 , 55 , alpha ) , 2.5 );
					draw_kanji( game , wx + x * size - fake_zz * z , wy + y * size - fake_z * z , game -> level -> board[ x ][ y ][ z ] . type , alpha );
				}
			}
		}
	}
	/*al_set_target_bitmap( oldtarget );

	  al_draw_bitmap( lvl_scrbuf , 0 , 0 , 0 ); */

	return TRUE ;
}
