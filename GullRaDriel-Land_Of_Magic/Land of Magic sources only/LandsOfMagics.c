#include "LOMConfig.h"
#include "LOMGfx.h"
#include "LOMGame.h"
#include "n_particles.h"

PARTICLE_SYSTEM *particles = NULL ;

GLOBAL_INSTANCE *game = NULL ;

int w = 0 , h = 0 , fps = 0 , fps_count = 0 , selx = 0 , sely = 0 ; 

int main( int argc , char *argv[] )
{
	/* Remove warn for unused argc,argv */
	(void) argc; (void) argv;

	set_log_level( LOG_NOTICE );

	game = load_instance_config( "LandsOfMagics.cfg" );  
	if( !game )
	{
		abort_game( "Unable to load config or init problem" );
	}
	N_STR *nstr = NULL ;
	nstrprintf( nstr , "LandsOfMagics [Build on %s at %s]" , __DATE__  , __TIME__ );
	al_set_window_title( game -> display, _nstr( nstr ) );


	ALLEGRO_BITMAP *scrbuf = al_create_bitmap( game -> w , game -> h );
	al_set_target_bitmap( scrbuf );
	al_clear_to_color( al_map_rgba( 0 , 0 , 0 , 255 ) );



	fps = game -> refresh_rate ;
	int mx = 0 ,  my =  0 ;
	int selected = 0 ;
	int selection[ 3 ][ 2 ] = { { 0 , 0 } , { 0 ,  0 } , { 0 , 0 } } ;

	int current_level = 1 ;
	int life = 100 ;
	int timeleft = 0 ;
	int total_time = 0 ;

	N_STR *str = new_nstr( 100 );
	nstrcat_bytes( str , "Welcome to the Land Of Magic board (TINS2016)!\n\n" );
	nstrcat_bytes( str , "-Click on pair of elements to clean them\n" );
	nstrcat_bytes( str , " Be quick, let's catch all the gems\n" );
	nstrcat_bytes( str , "-Go through the time limited four levels\n" );
	nstrcat_bytes( str , " Change the source the make a tower of babel\n" );
	nstrcat_bytes( str , "-Errors cost life\n" );
	nstrcat_bytes( str , " And you don't have a knife !\n" );
	nstrcat_bytes( str , "\n\nBy Mickaël Castagnier aka GullRaDriel\n" );


    int button = al_show_native_message_box(  al_get_current_display() , 
			  "LandOfMagic", 
			  "Presentation", _nstr( str ) , NULL, ALLEGRO_MESSAGEBOX_YES_NO );

	n_log( LOG_NOTICE , "Button: %d" , button );
	if( button == 0 )
		exit( 0 );

	init_particle_system( &particles , 1000 , game -> w/2 , 0 , 0 );
	ALLEGRO_BITMAP *snow = al_load_bitmap( "data/snow2.png" );
	if( !snow )
		abort_game( "UNABLE TO LOAD data/snow2.png !!" );

	for( int i = 0 ; i < 100 ; i ++ )
	{

		add_particle( particles , snow , TRANS_PART , (game->w/2)-rand()%game->w , -rand()%game->h , 20000 + rand()%10000 , 0 , 
				20-rand()%40 , 20+rand()%40 , 0 ,
				0 , 0 , 0 );
	}

	while( current_level < 4 )
	{
		al_flush_event_queue( game -> queue );
		timeleft = timespan * current_level ;

		game -> level = new_level( 8 , 6 , current_level );

		while (true && check_level_state( game -> level ) == FALSE )
		{
			ALLEGRO_EVENT event;
			bool do_draw = false;
			al_get_next_event( game -> queue, &event);
			switch( event.type )
			{
				case ALLEGRO_EVENT_MOUSE_AXES:
					mx = event.mouse.x;
					my = event.mouse.y;
					break;

				case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
					if( selx >= 0 && selx < game -> level -> x && 
							sely >= 0 && sely < game -> level -> y )
					{
						if( game -> level -> board[ selx ][ sely ][ 0 ] . active == 1 )
						{
							selection[ 0 ][ selected ] = selx ;
							selection[ 1 ][ selected ] = sely ;
							game -> level -> board[ selx ][ sely ][ 0 ] . selected = 1 -  game -> level -> board[ selx ][ sely ][ 0 ] . selected ;
							selected ++ ;
							if( selected > 1 )
							{
								selected = 0 ;
								if( selection[ 0 ][ 0 ] !=  selection[ 0 ][ 1 ] ||
										selection[ 1 ][ 0 ] !=  selection[ 1 ][ 1 ] )
								{
									if( game -> level -> board[ selection[ 0 ][ 0 ] ][ selection[ 1 ][ 0 ] ][ 0 ] . type ==  game -> level -> board[ selection[ 0 ][ 1 ] ][ selection[ 1 ][ 1 ] ][ 0 ] . type )
									{
										for( int it = 0 ; it <= game -> level -> z - 2 ; it ++ )
										{
											game -> level -> board[ selection[ 0 ][ 0 ] ][ selection[ 1 ][ 0 ] ][ it ] . active =
												game -> level -> board[ selection[ 0 ][ 0 ] ][ selection[ 1 ][ 0 ] ][ it + 1 ] . active ;
											game -> level -> board[ selection[ 0 ][ 1 ] ][ selection[ 1 ][ 1 ] ][ it ] . active =
												game -> level -> board[ selection[ 0 ][ 1 ] ][ selection[ 1 ][ 1 ] ][ it + 1 ] . active ;
											game -> level -> board[ selection[ 0 ][ 0 ] ][ selection[ 1 ][ 0 ] ][ it ] . type =
												game -> level -> board[ selection[ 0 ][ 0 ] ][ selection[ 1 ][ 0 ] ][ it + 1 ] . type ;
											game -> level -> board[ selection[ 0 ][ 1 ] ][ selection[ 1 ][ 1 ] ][ it ] . type =
												game -> level -> board[ selection[ 0 ][ 1 ] ][ selection[ 1 ][ 1 ] ][ it + 1 ] . type ;
										}
										game -> level -> board[ selection[ 0 ][ 0 ] ][ selection[ 1 ][ 0 ] ][ game -> level -> z - 1 ] . active =
											game -> level -> board[ selection[ 0 ][ 1 ] ][ selection[ 1 ][ 1 ] ][ game -> level -> z - 1 ] . active = 
											game -> level -> board[ selection[ 0 ][ 0 ] ][ selection[ 1 ][ 0 ] ][ game -> level -> z - 1 ] . type =
											game -> level -> board[ selection[ 0 ][ 1 ] ][ selection[ 1 ][ 1 ] ][ game -> level -> z - 1 ] . type = 0 ;
										timeleft += 2000 ;
									}
									else
									{
										/* loosing life when doing errors */
										life -= 13 ;
										if( life < 0 )
										{
											abort_game( "Because you're life went too low, the party have to stop by now" );
										}
									}
								}
								game -> level -> board[ selection[ 0 ][ 0 ] ][ selection[ 1 ][ 0 ] ][ 0 ] . selected =  
									game -> level -> board[ selection[ 0 ][ 1 ] ][ selection[ 1 ][ 1 ] ][ 0 ] . selected = 0 ; 
							}
							break ;
						}
					}
					break;

				case ALLEGRO_EVENT_TIMER :
					if( timeleft < 0 )
					{
						abort_game( "Time sure pass and you took yours, but the magic didn't wait and went vaporous\nTry a new time ! Don't be shime !" );
					}
					if( al_get_timer_event_source( game -> fps_timer ) == event.any.source ) 
					{
						fps = ( fps + fps_count ) / 2 ;
						fps_count = 0 ;
					}
					else
					{
						do_draw = true ;
						timeleft -= 30 ;
						total_time += 30;
					}
					break ;
				case ALLEGRO_EVENT_KEY_UP:
					if( event.type == ALLEGRO_EVENT_KEY_UP && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE ) 
					{
						exit( 0 );
					}
					break ;
				case ALLEGRO_EVENT_DISPLAY_CLOSE :
					{
						exit( 0 );
					}
					break ;
			}
			if( do_draw == true ) 
			{
				fps_count ++ ;
				al_set_target_bitmap( scrbuf );

				manage_particle( particles );
				if( particles -> nb_particles < 90 )
				{
					add_particle( particles , snow , TRANS_PART , (game->w/2)-rand()%game->w , -rand()%game->h , 20000 + rand()%10000  , 0 , 
							20-rand()%40 , 20+rand()%40 , 0 ,
							0 , 0 , 0 );
				}

				w = game -> w ;
				h = game -> h ;

				al_clear_to_color(al_map_rgb(0,0,0));
				if( game -> bg )
					al_draw_bitmap( game -> bg , 0, 0, 0);

				/* call drawings here */

				al_acknowledge_resize( game -> display );
				w = al_get_display_width(  game -> display );
				h = al_get_display_height( game -> display );

				al_set_target_bitmap( al_get_backbuffer( game -> display ) );
				al_clear_to_color( al_map_rgba( 0 , 0 , 0 , 255 ) );
				al_draw_bitmap( scrbuf , w/2 - al_get_bitmap_width( scrbuf ) /2 , h/2 - al_get_bitmap_height( scrbuf ) / 2 , 0 );
				draw_particle( particles );

				N_STR *nstr = NULL ;
				nstrprintf( nstr , "%d i/sec , mx:%d , my:%d Timeleft: %d" , fps , mx , my , timeleft );
				al_draw_text( game -> font[ SMALL_FONT ] , al_map_rgb( 255 , 0 , 0 ) , 10 , 10 , 0 , _nstr( nstr ) );
				free_nstr( &nstr );

				/* level draw */
				draw_level( game );
				/* mouse highlight */
				selx = ( mx - ( game -> w / 2 ) + ( game -> level -> x * size ) /2 ) / size ;
				sely = ( my - ( game -> h / 2 ) + ( game -> level -> y * size ) /2 ) / size ;

				nstrprintf( nstr , "seldx: %d sely: %d" , selx , sely );
				al_draw_text( game -> font[ SMALL_FONT ] , al_map_rgb( 255 , 0 , 0 ) , 10 , 30 , 0 , _nstr( nstr ) );
				free_nstr( &nstr );

				al_draw_filled_rectangle( game -> w - 40 , game -> h - 10 , game -> w - 10 , (game -> h - 10) - (game -> h - 20 ) * life / 100 , al_map_rgb( 255 , 0 , 0 ) );
				al_draw_filled_rectangle( game -> w - 60 , game -> h - 10 , game -> w - 30 , (game -> h - 10) - (game -> h - 20 ) * timeleft / (current_level * timespan) , al_map_rgb( 200 , 200 , 200 ) );


				al_flip_display();
				do_draw = false ;
			}
		}
		free_level( game -> level );
		current_level ++ ;
	}
	abort_game( "You won ! Better next time ? \nTotal time: %d" , total_time/2 ); 
	return 0;
}
