#include "LOMConfig.h"



void abort_game(char const *format, ...)
{
   char str[1024];
   va_list args;
   ALLEGRO_DISPLAY *display;

   va_start(args, format);
   vsnprintf(str, sizeof str, format, args);
   va_end(args);

   if (al_init_native_dialog_addon()) {
      display = al_is_system_installed() ? al_get_current_display() : NULL;
      al_show_native_message_box(display, "Quitting", "LandOfMagic", str, NULL, 0);
   }
   else {
      fprintf(stderr, "%s", str);
   }
   exit(1);
}

 /* get a color from a r,g,b,a string */
ALLEGRO_COLOR get_color_from_string( char *str , char *cfg_param )
{
   n_assert( str , abort_game( "Couldn't read %s to make a color" , cfg_param ) );
   
   char **result=split( str , "," , 0 );
   int count = split_count( result );
   if( count != 4 )
   {
      free_split_result( &result );
      abort_game( "Invalid number of colors (must be r,g,b,a): %d" , count );
   }

   int r = 255 , g = 0 , b = 0 , a = 0 ;
   str_to_int( trim_nocopy( result[ 0 ] ), &r , 10 );
   str_to_int( trim_nocopy( result[ 1 ] ), &g , 10 );
   str_to_int( trim_nocopy( result[ 2 ] ), &b , 10 );
   str_to_int( trim_nocopy( result[ 3 ] ), &a , 10 );
   free_split_result( &result );
   
   return al_map_rgba( r , g , b , a );
}



GLOBAL_INSTANCE *load_instance_config( char *file )
{
   GLOBAL_INSTANCE *internals = NULL ;
   ALLEGRO_CONFIG *cfg = NULL ;
   const char *strptr = NULL ;

   int tmpval = 0 ;
   
   cfg = al_load_config_file( file );
   if( !cfg )
   {
      abort_game( "Couldn't load config file %s\n" , file );
   }

   Malloc( internals , GLOBAL_INSTANCE , 1 );
   if( !internals )
   {
      abort_game( "Couldn't Malloc QVS_INTERNALS\n" );
   }

   /* DISPLAY */
   if( !(internals -> font_file_small = al_get_config_value(cfg, "DISPLAY", "font_file_small") ) )
   {
      abort_game( "Couldn't load config [DISPLAY] font_file_small\n" );
   }
   fprintf( stderr , "font_file_small: %s\n" , internals -> font_file_small );
   if( !(internals -> font_file_regular = al_get_config_value(cfg, "DISPLAY", "font_file_regular") ) )
   {
      abort_game( "Couldn't load config [DISPLAY] font_file_regular\n" );
   }
   fprintf( stderr , "font_file_regular: %s\n" , internals -> font_file_regular );
   if( !(internals -> font_file_huge = al_get_config_value(cfg, "DISPLAY", "font_file_huge") ) )
   {
      abort_game( "Couldn't load config [DISPLAY] font_file_huge\n" );
   }
   fprintf( stderr , "font_file_huge: %s\n" , internals -> font_file_huge );

   internals -> font_size_small = 0 ;
   if( str_to_int( al_get_config_value(cfg, "DISPLAY", "font_size_small") , &internals -> font_size_small , 10 ) != TRUE )
   {
      abort_game( "Couldn't load config [DISPLAY] font_size_small\n" );
   }
   fprintf( stderr , "font_size_small: %d\n" , internals -> font_size_small );
   internals -> font_size_regular = 0 ;
   if( str_to_int( al_get_config_value(cfg, "DISPLAY", "font_size_regular" ) , &internals -> font_size_regular , 10 ) != TRUE )
   {
      abort_game( "Couldn't load config [DISPLAY] font_size_regular\n" );
   }
   fprintf( stderr , "font_size_regular: %d\n" , internals -> font_size_regular );
   internals -> font_size_huge = 0 ;
   if( str_to_int( al_get_config_value(cfg, "DISPLAY", "font_size_huge") , &internals -> font_size_huge , 10 ) != TRUE )
   {
      abort_game( "Couldn't load config [DISPLAY] font_size_huge\n" );
   }
   fprintf( stderr , "font_size_huge: %d\n" , internals -> font_size_huge );
   internals -> w = 0 ;
   if( str_to_int( al_get_config_value(cfg, "DISPLAY", "w") , &internals -> w , 10 ) != TRUE )
   {
      abort_game( "Couldn't load config [DISPLAY] w\n" );
   }
   fprintf( stderr , "w: %d\n" , internals -> w );
   internals -> h = 0 ;
   if( str_to_int( al_get_config_value(cfg, "DISPLAY", "h") , &internals -> h , 10 ) != TRUE )
   {
      abort_game( "Couldn't load config [DISPLAY] h\n" );
   }
   fprintf( stderr , "h: %d\n" , internals -> h );

   internals -> refresh_rate = 0 ;
   if( !( strptr = al_get_config_value(cfg, "DISPLAY", "refresh_rate") ) )
   {
      abort_game( "Couldn't load config [DISPLAY] refresh_rate\n" );
   }
   internals -> refresh_rate = strtod( strptr , NULL );
   if(  internals -> refresh_rate == 0.0 || abs(  internals -> refresh_rate ) == HUGE_VAL )
   {     
      abort_game( "[RECORDER] latency: incorrect float number\n" );
   }
   fprintf( stderr , "refresh_rate: %f\n" , internals -> refresh_rate );

   internals -> fullscreen = 0 ;
   tmpval = 0 ;
   if( str_to_int( al_get_config_value(cfg, "DISPLAY", "fullscreen") , &tmpval , 10 ) != TRUE )
   {
      abort_game( "Couldn't load config [DISPLAY] fullscreen\n" );
   }
   internals -> fullscreen = tmpval ;
   fprintf( stderr , "fullscreen: %d\n" , internals -> fullscreen );

   for( int it = 0 ; it <= 3 ; it ++ )
      internals -> font[ it ] = NULL ;

   internals -> queue = NULL ;
   internals -> timer = NULL ;
   internals -> display = NULL ;

   /* 
    * Config loading OK ; proceed initialisations 
    */


   /* allegro 5 + addons loading */
   if (!al_init()) {
      abort_game("Could not init Allegro.\n");
   }
   if (!al_install_audio()) {
      abort_game("Unable to initialize audio addon\n");
   }
   if (!al_init_acodec_addon()) {
      abort_game("Unable to initialize acoded addon\n");
   }
   if (!al_init_image_addon()) {
      abort_game("Unable to initialize image addon\n");
   }
   if (!al_init_primitives_addon() ) {
      abort_game("Unable to initialize primitives addon\n");
   }
   if( !al_init_font_addon() ) {
      abort_game("Unable to initialize font addon\n");
   }
   if( !al_init_ttf_addon() ) {
      abort_game("Unable to initialize ttf_font addon\n");
   }
   if( !al_install_keyboard() ) {
      abort_game("Unable to initialize keyboard handler\n");
   }
   if( !al_install_mouse()) {
      abort_game("Unable to initialize mouse handler\n");
   }

   if( internals -> fullscreen != 0 )
      al_set_new_display_flags( ALLEGRO_OPENGL|ALLEGRO_FULLSCREEN_WINDOW );
   else
      al_set_new_display_flags( ALLEGRO_OPENGL|ALLEGRO_WINDOWED );

   internals -> display = al_create_display( internals -> w , internals -> h );
   if( !internals -> display )
   {
      abort_game("Unable to create display\n");
   }
   al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);

   internals -> font[ 0 ] = al_load_font( internals -> font_file_small , internals -> font_size_small , 0 );
   if (! internals -> font[ 0 ] ) {
      abort_game("Unable to load %s\n" , internals -> font_file_small );
   }
   internals -> font[ 1 ] = al_load_font( internals -> font_file_regular , internals -> font_size_regular , 0 );
   if (! internals -> font[ 0 ] ) {
      abort_game("Unable to load %s\n" , internals -> font_file_regular );
   }
   internals -> font[ 2 ] = al_load_font( internals -> font_file_huge , internals -> font_size_huge , 0 );
   if (! internals -> font[ 0 ] ) {
      abort_game("Unable to load %s\n" , internals -> font_file_huge );
   }

   internals -> queue = al_create_event_queue();
   if( !internals -> queue )
   {
      abort_game("Unable to create event queue\n");
   }
   internals -> timer = al_create_timer( 1.0 / internals -> refresh_rate );
   al_start_timer( internals -> timer );
   internals -> fps_timer = al_create_timer( 1.0 );
   al_start_timer( internals -> fps_timer );

   al_register_event_source( internals -> queue, al_get_display_event_source( internals -> display ) );
   al_register_event_source( internals -> queue, al_get_timer_event_source( internals -> timer ) );
   al_register_event_source( internals -> queue, al_get_timer_event_source( internals -> fps_timer ) );
   al_register_event_source( internals -> queue, al_get_keyboard_event_source() );
   al_register_event_source( internals -> queue, al_get_mouse_event_source()) ;

   al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

   return internals ;
}
