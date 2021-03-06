#include "sound.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include "resources.h"
#include <assert.h>
#include "util.h"
#include <iostream>
#include "mainloop.h"

#ifdef USE_ALSPC
#include <alspc.h>
#endif

using namespace std;

Sound::Sound(): inited(false), soundInstalled(true), fSound(true), fMusic(true)
{
	currentMusic = NULL;
}

void Sound::initSound()
{
#ifdef USE_ALSPC
    hifi = true;
    stereo = true;
    currentMusic = NULL;
    alspc_install();
    alspc_player = NULL;
#endif

	voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
	if (!voice) {
		allegro_message("Could not create ALLEGRO_VOICE.\n");	//TODO: log error.
	}

	mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32,
			ALLEGRO_CHANNEL_CONF_2);
	if (!mixer) {
		allegro_message("Could not create ALLEGRO_MIXER.\n");	//TODO: log error.
	}

	if (!al_attach_mixer_to_voice(mixer, voice)) {
		allegro_message("al_attach_mixer_to_voice failed.\n");	//TODO: log error.
	}

	inited = true;
}

void Sound::getSoundFromConfig(ALLEGRO_CONFIG *config)
{
	fMusic = get_config_int (config, "twist", "music", fMusic);
	fSound = get_config_int (config, "twist", "sound", fSound);
}

void Sound::playSample (ALLEGRO_SAMPLE *s)
{
	if (!(isSoundOn() && isSoundInstalled())) return;
	assert (s);

	bool success = al_play_sample (s, 1.0, 1.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	if (!success) {
		cout << "Could not play sample" << endl; //TODO: log error.
	}
}

void Sound::playMusic (ALLEGRO_AUDIO_STREAM *duh, float volume)
{
	if (!isSoundInstalled()) return;
    if (!(isSoundOn() && isMusicOn())) return;
    if (currentMusic)
    {
        al_detach_audio_stream(currentMusic);
        currentMusic = NULL;
    }
    if (!al_attach_audio_stream_to_mixer(duh, mixer)) {
       allegro_message("al_attach_audio_stream_to_mixer failed.\n"); //TODO: log error.
    }
	currentMusic = duh;
}

void Sound::stopMusic ()
{
    if (currentMusic)
    {
    	al_detach_audio_stream(currentMusic);
        currentMusic = NULL;
    }
}

void Sound::propertyChanged ()
{
	if (!isSoundInstalled()) return;
	if (isMusicOn() && isSoundOn())
	{
		if (currentMusic != NULL)
			playMusic (currentMusic);
	}
	else
	{
		stopMusic();
	}
}

void Sound::setSoundOn(bool value)
{
	if (fSound == value) return;
	fSound = value;
	propertyChanged();
	set_config_int (MainLoop::getMainLoop()->getConfig(), "twist", "sound", fSound);
}

void Sound::setMusicOn(bool value)
{
	if (fMusic == value) return;

	fMusic = value;
	propertyChanged();
	set_config_int (MainLoop::getMainLoop()->getConfig(), "twist", "music", fMusic);
}

#ifdef USE_ALSPC
void Sound::playMusic (ALSPC_DATA *alspc_data)
{
	if (!isSoundInstalled()) return;
	currentMusic = alspc_data;
    if (!(isSoundOn() && isMusicOn())) return;
    assert (alspc_data); // fails if requested SPC is not in datafile.
    stopMusic();
    alspc_player = alspc_start (alspc_data, hifi ? 44100 : 22050, 255, 128, stereo, hifi);
}

void Sound::pollMusic()
{
	assert (inited);
	if (alspc_player) alspc_poll (alspc_player);
}

void Sound::setHifiOn(bool value)
{
    if (value != hifi)
    {
        hifi = value;
        //TODO: reset music playing
    }
}

void Sound::setStereoOn(bool value)
{
    if (value != stereo)
    {
        stereo = value;
        //TODO: reset music playing
    }
}

void Sound::stopMusic()
{
    if (alspc_player)
    {
        alspc_stop(alspc_player);
        alspc_player = NULL;
    }
}

#endif

void Sound::doneSound()
{
	stopMusic();
#ifdef USE_ALSPC
	stopMusic();
    alspc_uninstall();
#endif
}

