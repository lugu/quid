#include <SDL.h>
#include <SDL_audio.h>

typedef struct {
  int wav_length;
  unsigned char *wav_data;
  SDL_AudioSpec wav_spec;
  SDL_AudioDeviceID device_id;
} sound_t;

typedef struct {
  sound_t explosion;
  sound_t laser;
} sound_effects_t;


void init_sound_effects(sound_effects_t *sound_effects);
void play_laser_sound(sound_effects_t *sound_effects);
void play_explosion_sound(sound_effects_t *sound_effects);
void destroy_sound_effects(sound_effects_t *sound_effects);
