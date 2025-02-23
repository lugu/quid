
#include "sound.h"

static void init_sound(sound_t *sound, const char *filename) {
  if (SDL_LoadWAV(filename, &sound->wav_spec, &sound->wav_data, &sound->wav_length) == NULL) {
    fprintf(stderr, "SDL_LoadWAV failed: %s\n", SDL_GetError());
    exit(1);
  }
  sound->device_id = SDL_OpenAudioDevice(NULL, 0, &sound->wav_spec, NULL, 0);

  if (sound->device_id <= 0) {
    fprintf(stderr, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
    exit(1);
  }
  SDL_PauseAudioDevice(sound->device_id, 0);
}

static void destroy_sound(sound_t *sound) {
  SDL_FreeWAV(sound->wav_data);
  SDL_CloseAudioDevice(sound->device_id);
}


static void play_sound(sound_t *sound) {
    if (SDL_QueueAudio(sound->device_id, sound->wav_data, sound->wav_length) < 0) {
        fprintf(stderr, "SDL_QueueAudio failed: %s\n", SDL_GetError());
  }
}

void init_sound_effects(sound_effects_t *sound_effects) {
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    exit(1);
  }
  init_sound(&sound_effects->laser, "laser.wav");
  init_sound(&sound_effects->explosion, "plosion.wav");
}

void play_laser_sound(sound_effects_t *sound_effects) {
  play_sound(&sound_effects->laser);
}

void play_explosion_sound(sound_effects_t *sound_effects) {
  play_sound(&sound_effects->explosion);
}

void destroy_sound_effects(sound_effects_t *sound_effects) {
  destroy_sound(&sound_effects->laser);
  destroy_sound(&sound_effects->explosion);
}
