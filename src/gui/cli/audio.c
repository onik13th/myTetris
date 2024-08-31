#include "audio.h"

/**
 * @brief Инициализирует аудиоподсистему SDL и микшера SDL_mixer.
 *
 * Эта функция инициализирует SDL для работы с аудио и
 * настраивает SDL_mixer для воспроизведения звуков в формате MP3.
 * В случае ошибки выводится сообщение и прекращается дальнейшая инициализация
 * аудио.
 */
void initAudio() {
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    // cleanupAudio(); // Освобождаем ресурсы в случае ошибки
    return;
  }

  if (Mix_Init(MIX_INIT_MP3) == 0) {
    fprintf(stderr, "Mix_Init Error: %s\n", Mix_GetError());
    // cleanupAudio(); // Освобождаем ресурсы в случае ошибки
    return;
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    fprintf(stderr, "Mix_OpenAudio Error: %s\n", Mix_GetError());
    // cleanupAudio(); // Освобождаем ресурсы в случае ошибки
    return;
  }
}

/**
 * @brief Загружает музыкальный файл для воспроизведения.
 *
 * Если ранее загруженная музыка существует, она освобождается,
 * а затем загружается новый музыкальный файл. Если загрузка файла
 * не удалась, выводится сообщение об ошибке.
 *
 * @param audioManager Указатель на структуру SoundController, содержащую данные
 * о текущей музыке.
 */
void loadMusic(SoundController *audioManager) {
  if (audioManager->music != NULL) {
    Mix_FreeMusic(
        audioManager
            ->music);  // освобождаем предыдущую музыку, если она была загружена
    audioManager->music = NULL;
  }

  audioManager->music = Mix_LoadMUS("../sound/my_tetris_sound.mp3");
  if (audioManager->music == NULL) {
    fprintf(stderr, "Mix_LoadMUS Error: %s\n", Mix_GetError());
    // cleanupMusic(gameInfo); // освобождаем ресурсы в случае ошибки
    return;
  }
}

/**
 * @brief Начинает воспроизведение загруженной музыки.
 *
 * Если музыка воспроизводится, она начинает играть
 * бесконечно (указано значение -1 для повтора).
 *
 * @param audioManager Указатель на структуру SoundController, содержащую данные
 * о текущей музыке.
 */
void startMusic(SoundController *audioManager) {
  if (Mix_PlayingMusic() == 0) {
    Mix_PlayMusic(audioManager->music, -1);
  }
}

/**
 * @brief Останавливает воспроизведение текущей музыки.
 *
 * Эта функция прекращает воспроизведение музыки, если она
 * в данный момент играет.
 */
void stopMusic() {
  if (Mix_PlayingMusic()) {
    Mix_HaltMusic();
  }
}

/**
 * @brief Освобождает ресурсы, связанные с музыкальным файлом.
 *
 * Если музыкальный файл был загружен, он освобождается.
 * Затем вызывается функция cleanupAudio() для завершения
 * работы с аудиоподсистемой SDL.
 *
 * @param audioManager Указатель на структуру SoundController, содержащую данные
 * о текущей музыке.
 */
void cleanupMusic(SoundController *audioManager) {
  if (audioManager->music != NULL) {
    Mix_FreeMusic(audioManager->music);
    audioManager->music = NULL;
  }

  cleanupAudio();
}

/**
 * @brief Завершает работу с аудиоподсистемой SDL и SDL_mixer.
 *
 * Эта функция закрывает аудиоподсистему SDL и SDL_mixer,
 * освобождая все связанные с ними ресурсы.
 */
void cleanupAudio() {
  Mix_CloseAudio();
  Mix_Quit();
  SDL_Quit();
}
