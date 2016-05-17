#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "main.hpp"
#include "resource-list.hpp"
#include "sprite.hpp"


double
rand_f ()
{
  return rand () / (RAND_MAX + 1.);
}

struct Particle
{
  double x;
  double y;
  double z;
};

#define NUMBER_OF_SNOW_PARTICLES 1500

Particle snow_particle[NUMBER_OF_SNOW_PARTICLES];

Resource_Item resource[NUMBER_OF_RESOURCES] = {
  {RESOURCETYPE_TEXT, "game-text.txt", NULL},
  {RESOURCETYPE_TTF_FONT, "DroidSansFallback.ttf", NULL},
  {RESOURCETYPE_BITMAP_FONT, "ProggyTinySZBP.png", NULL},
  {RESOURCETYPE_SAMPLE, "boop.wav", NULL},
  {RESOURCETYPE_STREAM, "cabin_fever.xm", NULL}
};

#define NUMBER_OF_SPRITES 10

Sprite object[NUMBER_OF_SPRITES];

uint32_t
compute_screen_pixel_height (uint32_t width, uint32_t height)
{
  double i;
  i = (((double) height / (double) width) * 256);
  if (i > 384)
    {
      i = 384;
    }
  else if (i < 144)
    {
      i = 144;
    }
  return (uint32_t) ceil (i);
}

bool debug_text = false;
bool quit = false;
bool redraw = true;
bool is_fullscreen = false;
int32_t frame_number = 0;
int32_t mouse_grab_x = 0;
int32_t mouse_grab_y = 0;
int32_t screen_pixel_height = 160;

double camera_yaw_x = 0.0;
double camera_yaw_y = 0.0;
double camera_pan_x = 0.0;
double camera_pan_y = 2.0;
double particle_field_x = 0.0;

void
print_time ()
{
  struct tm clock_parts;
  time_t clock;

  time (&clock);
  clock_parts = *gmtime (&clock);
  al_draw_textf ((ALLEGRO_FONT *) resource[RESOURCE_DEBUGFONT].item,
                 al_map_rgb (255, 255, 255), 0, 384-screen_pixel_height,
                 ALLEGRO_ALIGN_LEFT, "%08d %3d %d",
                 frame_number, screen_pixel_height, object[0].state);
  al_draw_textf ((ALLEGRO_FONT *) resource[RESOURCE_DEBUGFONT].item,
                 al_map_rgb (255, 255, 255), 256, 384-screen_pixel_height,
                 ALLEGRO_ALIGN_RIGHT, "%4d-%02d-%02dT%02d:%02d:%02dZ",
                 clock_parts.tm_year + 1900, clock_parts.tm_mon,
                 clock_parts.tm_mday, clock_parts.tm_hour,
                 clock_parts.tm_min, clock_parts.tm_sec);
  al_draw_textf ((ALLEGRO_FONT *) resource[RESOURCE_DEBUGFONT].item,
                 al_map_rgb (255, 255, 255), 0, 12+384-screen_pixel_height,
                 ALLEGRO_ALIGN_LEFT, "%.4f %.4f %.4f %.4f",
                 camera_yaw_x, camera_yaw_y, camera_pan_x, camera_pan_y);
  return;
}

void project_point(double x, double y, double z, double yaw_x, double yaw_y, double *out_x, double *out_y)
{
  *out_x = (x * 16 * (z / 16))+(yaw_x*16)+128;
  *out_y = (-y * 16 * (z / 16))+(yaw_y*16)+304;
  return;
}


int
main (void /*int argc, char **argv */ )
{
  ALLEGRO_DISPLAY *display = NULL;
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;
  ALLEGRO_PATH *path = NULL;
  ALLEGRO_TIMER *timer = NULL;
  ALLEGRO_BITMAP *screen_buf = NULL;
  ALLEGRO_COLOR temp_color;
  ALLEGRO_KEYBOARD_STATE keyboard_state;

  int32_t x, y, z, i, w, h;
  uint8_t r, g, b;
  double f = 0;
  double fx = 0, fy = 0;
  const char *s = NULL;

  al_set_app_name ("tins-2016");
  al_set_org_name ("com.gmail.jroatch");

  if (!al_init ())
    {
      /* TODO: test for presence of native_dialog addon,
         and send message to stderr if needed. */
      al_show_native_message_box (display,
                                  "Fatal Error!",
                                  "Could not initialize Allegro 5.2.",
                                  "This application will now shutdown.",
                                  NULL, ALLEGRO_MESSAGEBOX_ERROR);
      return 1;
    }

  if (!al_install_keyboard ())
    {
      /* TODO make this optional */
      al_show_native_message_box (display,
                                  "Fatal Error!",
                                  "Could not get a keyboard.",
                                  "This application will now shutdown.",
                                  NULL, ALLEGRO_MESSAGEBOX_ERROR);
      return 1;
    }

  timer = al_create_timer (ALLEGRO_BPS_TO_SECS (72));
  if (!timer)
    {
      al_show_native_message_box (display,
                                  "Fatal Error!",
                                  "Failed to create timer.",
                                  "This application will now shutdown.",
                                  NULL, ALLEGRO_MESSAGEBOX_ERROR);
      return 1;
    }

  al_set_new_display_flags (ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_FRAMELESS | ALLEGRO_GENERATE_EXPOSE_EVENTS | ALLEGRO_PROGRAMMABLE_PIPELINE   /*|
                                                                                                                                                           ALLEGRO_FULLSCREEN_WINDOW */ );
  al_set_new_display_option (ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option (ALLEGRO_FLOAT_COLOR, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option (ALLEGRO_FLOAT_DEPTH, 1, ALLEGRO_SUGGEST);
  al_set_new_window_title ("TINS 2016 - JRoatch");
  display = al_create_display (512, 320);
  if (!display)
    {
      al_show_native_message_box (display,
                                  "Fatal Error!",
                                  "Could not create graphics window.",
                                  "This application will now shutdown.",
                                  NULL, ALLEGRO_MESSAGEBOX_ERROR);
      return 1;
    }

  al_set_window_constraints (display, 512, 288, 512, 768);

  screen_buf = al_create_bitmap (256, 384);
  //screen_buf = al_create_bitmap(256, 160);

  /* TODO: check for errors! */
  al_init_image_addon ();
  al_install_audio ();
  al_init_acodec_addon ();
  al_init_primitives_addon ();
  al_install_mouse ();
  al_init_font_addon ();
  al_init_ttf_addon ();

  al_reserve_samples (16);

  path = al_get_standard_path (ALLEGRO_RESOURCES_PATH);
  al_append_path_component (path, "media");
  for (i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
      //ALLEGRO_DEBUG (resource[i].filename);
      al_set_path_filename (path, resource[i].filename);
      s = al_path_cstr (path, ALLEGRO_NATIVE_PATH_SEP);
      switch (resource[i].type)
        {
        case RESOURCETYPE_IMAGE:
          resource[i].item = al_load_bitmap (s);
          break;
        case RESOURCETYPE_SAMPLE:
          resource[i].item = al_load_sample (s);
          break;
        case RESOURCETYPE_BITMAP_FONT:
          resource[i].item = al_load_bitmap_font (s);
          break;
        case RESOURCETYPE_TTF_FONT:
          resource[i].item = al_load_ttf_font (s, -64, 0);
          break;
        case RESOURCETYPE_STREAM:
          resource[i].item = al_load_audio_stream (s, 2, 1024);
          break;
        case RESOURCETYPE_TEXT:
          /* TODO */
          break;
        case RESOURCETYPE_TILEMAP:
          al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
          resource[i].item = al_load_bitmap(s);
          w = al_get_bitmap_width((ALLEGRO_BITMAP *)resource[i].item);
          h = al_get_bitmap_height((ALLEGRO_BITMAP *)resource[i].item);
          al_set_target_bitmap ((ALLEGRO_BITMAP *)resource[i].item);
          for (y = 0; y < h; y++)
            {
              for (x = 0; x < w; x++)
                {
                  temp_color = al_get_pixel((ALLEGRO_BITMAP *)resource[i].item, x, y);
                  al_unmap_rgb(temp_color, &r, &g, &b);
                  z = (((r&0x30) + (g&0x30) + (b&0x30))/3)&0x30;
                  r = (r&0xc0)|z;
                  g = (g&0xc0)|z;
                  b = (b&0xc0)|z;
                  r = r|r>>4;
                  g = g|g>>4;
                  b = b|b>>4;
                  temp_color = al_map_rgb(r, g, b);
                  al_put_pixel(x, y, temp_color);
                }
            }
          al_set_target_backbuffer (display);
          al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP);
          break;
        case RESOURCETYPE_UNKOWN:
        default:
          break;
        }
    }
  al_destroy_path (path);

  event_queue = al_create_event_queue ();
  /* TODO: check for error! */
  al_register_event_source (event_queue, al_get_timer_event_source (timer));
  al_register_event_source (event_queue,
                            al_get_display_event_source (display));
  al_register_event_source (event_queue, al_get_keyboard_event_source ());
  al_register_event_source (event_queue, al_get_mouse_event_source ());

  al_start_timer (timer);

  for (i = 0; i < NUMBER_OF_SNOW_PARTICLES; i++)
    {
      snow_particle[i].x = (rand_f () - 0.5) * 48;
      snow_particle[i].y = (rand_f () * 48) - 4.0;
      snow_particle[i].z = (rand_f () * 16) + 8;
    }
    
  for (i = 0; i < NUMBER_OF_SPRITES; i++)
    {
      object[i].type = SPRITETYPE_NONE;
      object[i].state = 0;
      object[i].x = 0;
      object[i].y = 0;
      object[i].z = 0;
      object[i].dx = 0;
      object[i].dy = 0;
      object[i].dz = 0;
      object[i].t = 0;
    }
    
  object[0].type = SPRITETYPE_PLAYER;

  //al_set_audio_stream_gain ((ALLEGRO_AUDIO_STREAM *) resource[RESOURCE_BGM].
  //                          item, 0.5);
  al_set_audio_stream_gain ((ALLEGRO_AUDIO_STREAM *) resource[RESOURCE_BGM].
                            item, 0.5);
  al_attach_audio_stream_to_mixer ((ALLEGRO_AUDIO_STREAM *)
                                   resource[RESOURCE_BGM].item,
                                   al_get_default_mixer ());

  while (!quit)
    {
      ALLEGRO_EVENT ev;
      al_wait_for_event (event_queue, &ev);

      if (ev.type == ALLEGRO_EVENT_TIMER)
        {
          //if ((frame_number % 64) == 0) {
          //  al_play_sample ((ALLEGRO_SAMPLE*)resource[RESOURCE_BOOP_SFX].item, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
          //}
          al_get_keyboard_state (&keyboard_state);
          for (i = 0; i < NUMBER_OF_SNOW_PARTICLES; i++)
            {
              snow_particle[i].x += (rand_f () - 0.5) / 16;
              snow_particle[i].y += (rand_f () - 0.5) / 16;
              snow_particle[i].z += (rand_f () - 0.5) / 16;
              snow_particle[i].y -= 1.0 / 40;
              if (snow_particle[i].y < -1)
                {
                  snow_particle[i].x = (rand_f () - 0.5) * 48;
                  snow_particle[i].y = 48 - 4.0;
                  snow_particle[i].z = (rand_f () * 16) + 8;
                }
            }

          for (i = 0; i < NUMBER_OF_SPRITES; i++)
            {
              if (object[i].type == SPRITETYPE_PLAYER)
                {
                  /* do gravity first */
                  object[i].dy += -9.8/(72*72);
                  
                  object[i].dx += -object[i].dx*0.1; //friction
                  
                  /* then logic */
                  /* TODO make a virtual joystick thing that cancels out left and right */
                  if (al_key_down (&keyboard_state, ALLEGRO_KEY_LEFT))
                    {
                      object[i].dx += -1.0/100;
                      object[i].state &= 4;
                      object[i].state |= 1;
                    }
                  if (al_key_down (&keyboard_state, ALLEGRO_KEY_RIGHT))
                    {
                      object[i].dx += 1.0/100;
                      object[i].state &= 4;
                      object[i].state |= 2;
                    }
                  if (!(object[i].state & 4) && al_key_down (&keyboard_state, ALLEGRO_KEY_SPACE))
                    {
                      object[i].dy = 1.0/7;
                      object[i].state |= 4;
                    }
                  if ((object[i].dx < 0.01) && (object[i].dx > -0.01))
                    {
                      object[i].state &= 4;
                    }
                  /* last add vectors */
                  object[i].x += object[i].dx;
                  object[i].y += object[i].dy;
                  object[i].z += object[i].dz;
                  object[i].t += 1.0;

                  if (object[i].y < 0)
                    {
                      object[i].y = 0;
                      object[i].dy = 0;
                      object[i].state = object[i].state & 3;
                    }
                  /* do cammera stuff here 'cus I'm a hack */
                    //double camera_yaw_x = 0.0;
                    //double camera_yaw_y = 0.0;
                    //double camera_pan_x = 0.0;
                    //double camera_pan_y = 3.0;
                  if (object[i].dy > 0)
                    {
                      camera_yaw_y = ((camera_yaw_y-2.0)*0.98)+2.0;
                    }
                  else
                    {
                      camera_yaw_y = camera_yaw_y*0.98;
                    }
                  if (object[i].y > 2)
                    {
                      camera_pan_y = object[i].y*0.02 + camera_pan_y*0.98;
                    }
                  else
                    {
                      camera_pan_y = ((camera_pan_y-2.0)*0.98)+2.0;
                    }
                  if ((object[i].state & 3) == 1)
                    {
                      camera_yaw_x = ((camera_yaw_x-4.0)*0.98)+4.0;
                    }
                  else if ((object[i].state & 3) == 2)
                    {
                      camera_yaw_x = ((camera_yaw_x+4.0)*0.98)-4.0;
                    }
                  camera_pan_x = object[i].x*0.02 + camera_pan_x*0.98;
                }
            }

          /*if (al_key_down (&keyboard_state, ALLEGRO_KEY_LEFT))
              camera_yaw_x += -1.0/16;
          if (al_key_down (&keyboard_state, ALLEGRO_KEY_RIGHT))
              camera_yaw_x += 1.0/16;
          if (al_key_down (&keyboard_state, ALLEGRO_KEY_UP))
              camera_yaw_y += -1.0/16;
          if (al_key_down (&keyboard_state, ALLEGRO_KEY_DOWN))
              camera_yaw_y += 1.0/16;
          if (al_key_down (&keyboard_state, ALLEGRO_KEY_A))
              camera_pan_x += 1.0/16;
          if (al_key_down (&keyboard_state, ALLEGRO_KEY_D))
              camera_pan_x += -1.0/16;
          if (al_key_down (&keyboard_state, ALLEGRO_KEY_W))
              camera_pan_y += -1.0/16;
          if (al_key_down (&keyboard_state, ALLEGRO_KEY_S))
              camera_pan_y += 1.0/16;*/
          frame_number++;
          redraw = true;
        }
      else if (ev.type == ALLEGRO_EVENT_KEY_CHAR)
        {
          if ((ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
              ||
              ((ev.keyboard.modifiers & (ALLEGRO_KEYMOD_ALT |
                                         ALLEGRO_KEYMOD_ALTGR))
               && (ev.keyboard.keycode == ALLEGRO_KEY_F4))
              ||
              ((ev.keyboard.modifiers & (ALLEGRO_KEYMOD_CTRL |
                                         ALLEGRO_KEYMOD_COMMAND))
               && (ev.keyboard.keycode == ALLEGRO_KEY_Q)))
            {
              quit = true;
            }
          if ((ev.keyboard.keycode == ALLEGRO_KEY_F11)
              || (ev.keyboard.keycode == ALLEGRO_KEY_F))
            {
              is_fullscreen = !is_fullscreen;
              if (is_fullscreen)
                {
                  al_set_window_constraints (display, 0, 0, 0, 0);
                }
              else
                {
                  al_set_window_constraints (display, 512, 288, 512, 768);
                }
              al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW,
                                   is_fullscreen);
            }
          if (ev.keyboard.keycode == ALLEGRO_KEY_TILDE)
            {
              debug_text = !debug_text;
            }
        }
      else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
          quit = true;
        }
      else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
          if (ev.mouse.pressure > 0.5)
            {
              //al_get_window_position(d, &x, &y);
              al_get_mouse_cursor_position (&x, &y);
              x = x - mouse_grab_x;
              y = y - mouse_grab_y;
              //printf("%d %d\n", x, y);
              al_set_window_position (display, x, y);
            }
          else
            {
              mouse_grab_x = ev.mouse.x;
              mouse_grab_y = ev.mouse.y;
            }
        }
      else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
        {
          screen_pixel_height =
            compute_screen_pixel_height (ev.display.width, ev.display.height);
          al_acknowledge_resize (display);
        }

      /* a side effect of having separate logic and draw rates is that
         I can not use the contents of the last visual frame for anything. */
      if (redraw && al_is_event_queue_empty (event_queue))
        {
          al_set_target_bitmap (screen_buf);
          al_set_clipping_rectangle (0, 384 - screen_pixel_height, 256,
                                     screen_pixel_height);
          al_clear_to_color (al_map_rgb (32, 32, 32));
          //al_draw_filled_rectangle(0, 0, 256, 160, al_map_rgba_f (0, 0, 0, 0.1));

          
          project_point(0-camera_pan_x, -1-camera_pan_y, 8, camera_yaw_x, camera_yaw_y, &fx, &fy);
          al_draw_line(0, fy, 256, fy, al_map_rgba_f (0, 0, 0.2, 0.2), 1);
          project_point(0-camera_pan_x, -1-camera_pan_y, 11.31, camera_yaw_x, camera_yaw_y, &fx, &fy);
          al_draw_line(0, fy, 256, fy, al_map_rgba_f (0, 0, 0.4, 0.4), 1);
          project_point(0-camera_pan_x, -1-camera_pan_y, 16, camera_yaw_x, camera_yaw_y, &fx, &fy);
          al_draw_line(0, fy, 256, fy, al_map_rgba_f (0.4, 0, 0.8, 0.8), 1);
          project_point(0-camera_pan_x, -1-camera_pan_y, 24, camera_yaw_x, camera_yaw_y, &fx, &fy);
          al_draw_line(0, fy, 256, fy, al_map_rgba_f (0, 0, 0.8, 0.8), 1);
          for (i = 0; i < NUMBER_OF_SPRITES; i++)
            {
              if (object[i].type == SPRITETYPE_PLAYER)
                {
                  project_point(object[i].x-camera_pan_x, object[i].y-camera_pan_y, 16, camera_yaw_x, camera_yaw_y, &fx, &fy);
                  al_draw_filled_rectangle(fx-8, fy-16, fx+8, fy+16, al_map_rgba_f (0, 0.2, 0, 0.2));
                  al_draw_rectangle(fx-8, fy-16, fx+8, fy+16, al_map_rgba_f (0, 1, 0, 1), 1);
                }
            }

          al_lock_bitmap (screen_buf, ALLEGRO_PIXEL_FORMAT_ANY,
                          ALLEGRO_LOCK_READWRITE);
          for (i = 0; i < NUMBER_OF_SNOW_PARTICLES; i++)
            {
              f = snow_particle[i].x - (camera_pan_x - particle_field_x);
              if (f < -24){
                  f += 48;
              }else if (f > 24){
                  f -= 48;
              }
              snow_particle[i].x = f;
              project_point(snow_particle[i].x, snow_particle[i].y-camera_pan_y, snow_particle[i].z, camera_yaw_x, camera_yaw_y, &fx, &fy);
              x = round(fx);
              y = round(fy);
              f = (snow_particle[i].z * 16) - 40;
              if (f > 255)
                z = 255;
              else if (f < 0)
                z = 0;
              else
                z = round(f);
              al_put_pixel (x, y, al_map_rgb (z, z, z));
            }
          particle_field_x = camera_pan_x;
          al_unlock_bitmap (screen_buf);
          //al_draw_filled_rectangle(0, 0, 16, 16, al_map_rgba_f (1, 1, 1, 1));
          //al_draw_filled_rectangle(240, 368, 256, 384, al_map_rgba_f (1, 1, 1, 1));

          if (debug_text)
            {
              fx = camera_yaw_x*16+128;
              fy = camera_yaw_y*16+304;
              al_draw_line(fx-8, fy-8, fx+8, fy+8, al_map_rgba_f (0.6, 0.6, 0.8, 0.0), 0);
              al_draw_line(fx+8, fy-8, fx-8, fy+8, al_map_rgba_f (0.6, 0.6, 0.8, 0.0), 0);
              print_time();
            }

          al_set_target_backbuffer (display);
          al_clear_to_color (al_map_rgb (255, 0, 255));
          al_draw_scaled_bitmap (screen_buf, 0, 384 - screen_pixel_height,
                                 256, screen_pixel_height, 0, 0,
                                 al_get_display_width (display),
                                 al_get_display_height (display),
                                 ALLEGRO_FLIP_NONE);
          //al_draw_scaled_bitmap(screen_buf, 0, 0, 256, 384, 0, 0, al_get_display_width(display), al_get_display_height(display), ALLEGRO_FLIP_NONE);
          //al_draw_scaled_bitmap(screen_buf, 0, 0, 256, 160, 32, 0, 256*3.75, 160*3.75, ALLEGRO_FLIP_NONE);
          /*al_draw_textf ((ALLEGRO_FONT*)resource[RESOURCE_DEBUGFONT].item,
             al_map_rgb (255, 255, 255), 3, 3,
             ALLEGRO_ALIGN_LEFT, "%d %d",
             frame_number, z); */

          /* char temp_filename[50];
             sprintf (temp_filename, "frames/frame-%d.png", z);
             al_save_bitmap((const char*)&temp_filename, screen_buf);
             z++; */

          al_flip_display ();
          redraw = false;
        }
    }


  for (i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
      if (resource[i].item != NULL)
        {
          switch (resource[i].type)
            {
            case RESOURCETYPE_IMAGE:
            case RESOURCETYPE_TILEMAP:
              al_destroy_bitmap ((ALLEGRO_BITMAP *) resource[i].item);
              break;
            case RESOURCETYPE_SAMPLE:
              al_destroy_sample ((ALLEGRO_SAMPLE *) resource[i].item);
              break;
            case RESOURCETYPE_BITMAP_FONT:
            case RESOURCETYPE_TTF_FONT:
              al_destroy_font ((ALLEGRO_FONT *) resource[i].item);
              break;
            case RESOURCETYPE_STREAM:
              al_drain_audio_stream ((ALLEGRO_AUDIO_STREAM *) resource[i].
                                     item);
              al_destroy_audio_stream ((ALLEGRO_AUDIO_STREAM *) resource[i].
                                       item);
              break;
            case RESOURCETYPE_TEXT:
            case RESOURCETYPE_UNKOWN:
            default:
              break;
            }
        }
    }

  if (screen_buf != NULL)
    {
      al_destroy_bitmap (screen_buf);
    }
  al_destroy_timer (timer);
  al_destroy_display (display);
  al_destroy_event_queue (event_queue);

  return 0;
}
