#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_physfs.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <physfs.h>

#include "title.h"
#include "game.h"
#include "main.h"
#include "resscale.h"

//static const int TARGET_W = 640, TARGET_H = 480;
static const int TARGET_W = 854, TARGET_H = 480;
//static const int TARGET_W = 1024, TARGET_H = 768;
//static const int TARGET_W = 1280, TARGET_H = 720;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_PATH *path = NULL;
bool hiRes = false;

static bool init(const char* arg) {
	ALLEGRO_MONITOR_INFO desktop;
	int w, h;

	srand(time(NULL));
	if (!al_init()) {
		al_show_native_message_box(NULL, "Error", "Cannot run", "al_init() failed!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return false;
	}

	al_install_keyboard();
	al_install_mouse();
	al_install_joystick();

#ifdef DEBUG
#pragma warning(disable:4127)
	if (true) {
		al_get_monitor_info(0, &desktop);
#pragma warning(default:4127)
#else
	if (!al_get_monitor_info(0, &desktop)) {
#endif
		al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL | ALLEGRO_GENERATE_EXPOSE_EVENTS);
		w = TARGET_W;
		h = TARGET_H;
	} else {
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_OPENGL | ALLEGRO_GENERATE_EXPOSE_EVENTS);
		w = desktop.x2;
		h = desktop.y2;
	}
	display = al_create_display(w, h);
	if (!display) {
		al_show_native_message_box(NULL, "Error", "Cannot run", "Can not create the display.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return false;
	}
	al_inhibit_screensaver(true);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

	al_set_window_title(display, "Haiku Rescue");

	if (al_install_audio()) {
		al_reserve_samples(16);
		al_init_acodec_addon();
	}

	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();

	timer = al_create_timer(1.0/60.0);

	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_mouse_event_source());

	if (!PHYSFS_init(arg)) {
		return false;
	}
	path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
#ifdef ALLEGRO_MSVC
	{
		const char *last = al_get_path_component(path, -1);
		if (0 == strcmp(last, "Debug")
			|| 0 == strcmp(last, "RelWithDebInfo")
			|| 0 == strcmp(last, "Release")
			|| 0 == strcmp(last, "Profile")) {
				al_remove_path_component(path, -1);
		}
	}
#endif
	al_append_path_component(path, "data");
	if (!PHYSFS_addToSearchPath(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP), 1)) {
		arg = PHYSFS_getLastError();
		return false;
	}
	al_remove_path_component(path, -1);
	if (al_filename_exists("extradata.zip")) {
		al_append_path_component(path, "extradata.zip");
		hiRes = PHYSFS_addToSearchPath(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP), 1);
	}
	al_set_physfs_file_interface();

	return true;
}

void deinit(void) {
	al_inhibit_screensaver(false);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(queue);
	al_destroy_path(path);
}

static void logo(void) {
	float x, y, w = 207, h = 108;
#ifdef DEBUG
	w = 0;
#endif
	if (w == 0) {
		return;
	}
	ALLEGRO_TRANSFORM t, td;
	ALLEGRO_COLOR red = al_map_rgb(255, 0, 0), black = al_map_rgb(0, 0, 0);
	ALLEGRO_BITMAP *buffer;
	ResScale mfLogo(display, w, h);

	mfLogo.getTransform(&t);
	al_transform_coordinates(&t, &w, &h);
	buffer = al_create_bitmap(w, h);
	if (!buffer) {
		// Can't create a 
		return;
	}
	al_set_target_bitmap(buffer);

	al_clear_to_color(black);

	// The Interesting M
	al_identity_transform(&td);
	al_translate_transform(&td, 9.0, 9.0);
	al_compose_transform(&td, &t);
	al_use_transform(&td);

	al_draw_filled_rectangle( 0.0,  0.0, 9.0,  90.0, red);
	al_draw_filled_rectangle(81.0,  0.0, 90.0, 90.0, red);
	al_draw_filled_triangle( 9.0,  0.0, 45.0, 79.0,  9.0, 20.0, red);
	al_draw_filled_triangle( 9.0, 20.0, 45.0, 79.0, 40.5, 90.0, red);
	al_draw_filled_triangle(81.0,  0.0, 81.0, 90.0, 45.0, 79.0, red);
	al_draw_filled_triangle(45.0, 79.0, 81.0, 90.0, 40.5, 90.0, red);

	// The flaming F
	al_identity_transform(&td);
	al_translate_transform(&td, 108.0, 9);
	al_compose_transform(&td, &t);
	al_use_transform(&td);
	
	 // Base
	al_draw_filled_rectangle( 9.0,  9.0, 81.0, 18.0, red);
	al_draw_filled_rectangle( 9.0, 18.0, 18.0, 81.0, red);
	al_draw_filled_rectangle(18.0, 36.0, 54.0, 45.0, red);
	al_draw_filled_triangle(81.0,  9.0, 90.0,  9.0, 81.0, 18.0, red);
	al_draw_filled_triangle( 9.0, 81.0, 18.0, 81.0,  9.0, 90.0, red);
	al_draw_filled_triangle(54.0, 36.0, 63.0, 36.0, 54.0, 45.0, red);

	 // Flames
	al_draw_filled_triangle( 9.0,  9.0,  9.5,  4.5, 18.0,  9.0, red);
	for (x = 18.0; x < 81.5; x += 9.0) {
		al_draw_filled_triangle(x, 9.0, x + 0.5, 0.0, x + 9.0, 9.0, red);
		al_draw_filled_triangle(9.0, x, 0.0, x, 9.0, x + 9.0, red);
	}
	al_draw_filled_triangle( 4.5, 9.0, 9.0, 9.0, 9.0, 18.0, red);

	al_set_target_backbuffer(display);

	x = mfLogo.getOffsetx();
	y = mfLogo.getOffsety();
	
	// Fade in
	for (w = 0.0; w < 1.0; w += 0.025) {
		al_clear_to_color(black);
		al_draw_tinted_bitmap(buffer, al_map_rgba_f(1.0, 1.0, 1.0, w), x, y, 0);
		al_flip_display();
		al_rest(1.0/60.0);
	}

	al_clear_to_color(black);
	al_draw_bitmap(buffer, x, y, 0);
	al_flip_display();
	al_rest(1.0); // TODO: Wait for sound to finish playing, only wait if no sound is playing

	// Fade out
	for (w = 1.0; w > 0.0; w -= 0.025) {
		al_clear_to_color(black);
		al_draw_tinted_bitmap(buffer, al_map_rgba_f(1.0, 1.0, 1.0, w), x, y, 0);
		al_flip_display();
		al_rest(1.0/60.0);
	}
	al_clear_to_color(black);
	al_flip_display();

	al_destroy_bitmap(buffer);
}

int main(int /*argc*/, const char* argv[]) {
	if (!init(argv[0])) {
		deinit();
		exit(-1);
	}

	logo();

	while (title_screen()) {
		play_game();
	}

	deinit();
	return 0;
}
