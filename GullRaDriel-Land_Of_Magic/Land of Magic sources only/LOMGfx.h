#ifndef LandOfMagicGfxHeader
#define LandOfMagicGfxHeader

#ifdef __cplusplus
extern "C"
{
#endif

#include "LOMConfig.h"
#include "float.h"
#include "allegro5/utf8.h" 

	#define K_AIR 0
	#define K_WATER 1
	#define K_FIRE 2
	#define K_EARTH 3
	#define K_SNOW 4


	int draw_level( GLOBAL_INSTANCE *game );

#ifdef __cplusplus
}
#endif

#endif
