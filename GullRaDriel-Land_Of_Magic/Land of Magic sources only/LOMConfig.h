#ifndef LandOfMagicConfigHeader
#define LandOfMagicConfigHeader

#ifdef __cplusplus
extern "C"
{
#endif

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

#include "n_list.h"
#include "n_log.h"
#include "n_str.h" 

#include "LOMGame.h"

#define SMALL_FONT 0
#define REGULAR_FONT 1
#define HUGE_FONT 2

   /*! Quiniou Studio Config structure */
   typedef struct GLOBAL_INSTANCE
   {
      /* DISPLAY */
      /*! path to ttf font file */
      const char *font_file_small ,
            /*! path to ttf font file */
            *font_file_regular ,
            /*! path to ttf font file */
            *font_file_huge ,
            /*! path to background picture */  
            *background_file ;
      /*! background image */
      ALLEGRO_BITMAP *bg ;

      /*! size for little text output */
      int font_size_small ,
          /*! size for normal text output */
          font_size_regular ,
          /*! size for huge text output */
          font_size_huge ,
          /*! Screen W size */
          w ,
          /*! Screen H size */
          h ;
      /*! Internal loop freq */
      float refresh_rate ;

      /*! The 3 loaded fonts with their respective sizes */
      ALLEGRO_FONT *font[ 3 ];
      /*! Acutal display */
      ALLEGRO_DISPLAY *display ;
      /*! Event queue */   
      ALLEGRO_EVENT_QUEUE *queue ;
      /*! Timer */
      ALLEGRO_TIMER *timer ;
      /*! FPS Timer */
      ALLEGRO_TIMER *fps_timer ;
      /*! internal: display mod */
      unsigned int fullscreen ;

	  LEVEL *level ;

   } GLOBAL_INSTANCE ;

   /* abort and tell the problem in a native dialog box if possible */
   void abort_game(char const *format, ...);
   /* get a color from a r,g,b,a string  */
   ALLEGRO_COLOR get_color_from_string( char *str , char *cfg_param );
   /* load global LandOfMagic config */
   GLOBAL_INSTANCE *load_instance_config( char *file );
   /* initialize after laoding config */
   int init_instance( GLOBAL_INSTANCE *game );

#ifdef __cplusplus
}
#endif

#endif
