#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "../../brick_game/tetris/tetris.h"

void initAudio();
void loadMusic(SoundController *audioManager);
void startMusic(SoundController *audioManager);
void stopMusic();
void cleanupMusic(SoundController *audioManager);
void cleanupAudio();

#endif  // AUDIO_H