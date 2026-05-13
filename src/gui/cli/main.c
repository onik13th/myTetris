#include <ncurses.h>

#include "../../brick_game/tetris/tetris.h"
#include "audio.h"
#include "draw.h"
#include "game_state_handlers.h"
#include "input_handler.h"

/**
 * @brief Основная функция игры Tetris.
 *
 * Инициализирует все необходимые компоненты игры, включая аудио, графику и
 * игровое состояние. Выполняет основной игровой цикл, обрабатывает
 * пользовательский ввод, обновляет состояние игры и управляет переходами между
 * состояниями (старт игры, игра, пауза, завершение игры). По завершении игры,
 * освобождает все ресурсы и корректно завершает работу.
 *
 * @return int Возвращает 0 после завершения игры.
 */
int main() {
  setlocale(LC_ALL, "");  // отображение символов UTF-8
  initAudio();
  initscr();
  initColors();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);

  GameWindows windows = initializeWindows();
  GameInfo_t gameInfo = {0};
  SoundController audioManager = {0};

  initGame(&gameInfo);
  loadMusic(&audioManager);
  startMusic(&audioManager);

  bool running = true;
  bool keyDownPressed = false;

  while (running) {
    UserAction_t action = getUserAction(&keyDownPressed);
    userInput(&gameInfo, &audioManager, action, keyDownPressed);

    gameInfo = updateCurrentState(gameInfo);

    switch (gameInfo.state) {
      case START:
        handleStartState(&windows);
        break;

      case PLAY:
        handlePlayState(&windows, &gameInfo);
        break;

      case PAUSE:
        handlePauseState(&windows);
        break;

      case GAME_OVER:
        handleGameOverState(&windows, &gameInfo, action, &running);
        break;

      default:
        break;
    }

    refresh();
    napms(80);  // задержка для плавного отображения (в миллисекундах)
  }

  stopMusic();
  cleanupMusic(&audioManager);
  cleanupWindows(&windows);  // очистка окон и завершение работы с ncurses

  return 0;
}
