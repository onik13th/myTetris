#include "game_state_handlers.h"

/**
 * @brief Обрабатывает начальное состояние игры.
 *
 * Отображает стартовый экран игры, используя окно, предназначенное для этого
 * экрана.
 *
 * @param windows Указатель на структуру `GameWindows`, содержащую все окна
 * игры.
 */
void handleStartState(GameWindows *windows) {
  drawStartScreen(windows->startWin);
}

/**
 * @brief Обрабатывает состояние игры во время игрового процесса.
 *
 * Очищает окна паузы и стартового экрана, если они активны, и отображает
 * основное игровое поле, информацию о текущем состоянии игры,
 * следующую фигуру тетромино и инструкции по управлению.
 *
 * @param windows Указатель на структуру `GameWindows`, содержащую все окна
 * игры.
 * @param gameInfo Указатель на структуру `GameInfo_t`, содержащую информацию о
 * текущем состоянии игры.
 */
void handlePlayState(GameWindows *windows, GameInfo_t *gameInfo) {
  if (windows->pauseWin != NULL) {
    werase(windows->pauseWin);
    wrefresh(windows->pauseWin);
  }

  if (windows->startWin != NULL) {
    werase(windows->startWin);
    wrefresh(windows->startWin);
    delwin(windows->startWin);
    windows->startWin = NULL;
  }

  drawGameField(windows->gameWin, gameInfo->field, gameInfo->current_piece.form,
                gameInfo->current_piece.posX, gameInfo->current_piece.posY,
                gameInfo->current_piece.color);
  drawGameInfo(windows->infoWin, gameInfo);
  drawNextTetromino(windows->nextWin, gameInfo->next_piece.form,
                    gameInfo->next_piece.color);
  drawInstructions(windows->instrWin);
}

/**
 * @brief Обрабатывает состояние паузы в игре.
 *
 * Отображает экран паузы, используя соответствующее окно.
 *
 * @param windows Указатель на структуру `GameWindows`, содержащую все окна
 * игры.
 */
void handlePauseState(GameWindows *windows) {
  drawGamePaused(windows->pauseWin);
}

/**
 * @brief Обрабатывает состояние завершения игры.
 *
 * Отображает экран окончания игры, а также проверяет, нужно ли завершить игру
 * в зависимости от действия пользователя.
 *
 * @param windows Указатель на структуру `GameWindows`, содержащую все окна
 * игры.
 * @param gameInfo Указатель на структуру `GameInfo_t`, содержащую информацию о
 * текущем состоянии игры.
 * @param action Действие пользователя, которое может повлиять на завершение
 * игры.
 * @param running Указатель на флаг, который определяет, продолжается ли игра.
 */
void handleGameOverState(GameWindows *windows, GameInfo_t *gameInfo,
                         UserAction_t action, bool *running) {
  drawGameOver(windows->gameOverWin, gameInfo);
  shouldExitGame(gameInfo, action, running);
}