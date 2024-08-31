#include "draw.h"

/**
 * @brief Инициализирует окна для отображения различных элементов игры.
 *
 * Создает и возвращает структуру `GameWindows`, содержащую все окна
 * (стартовое, игровое, информационное, окно следующей фигуры, инструкций, паузы
 * и окончания игры). Эти окна используются для отображения соответствующих
 * частей интерфейса игры.
 *
 * @return GameWindows Структура, содержащая все необходимые окна.
 */

GameWindows initializeWindows() {
  GameWindows windows;
  windows.startWin = newwin(13, 36, 5, 25);
  windows.gameWin = newwin(22, 22, 1, 25);
  windows.infoWin = newwin(10, 20, 1, 2);
  windows.nextWin = newwin(8, 20, 1, 50);
  windows.instrWin = newwin(13, 20, 10, 50);
  windows.pauseWin = newwin(7, 24, 5, 24);
  windows.gameOverWin = newwin(10, 24, 5, 24);

  return windows;
}

/**
 * @brief Инициализирует цветовые пары для игры.
 *
 * Задает цветовые пары, используемые для отображения игровых элементов,
 * таких как тетромино и текст. Каждая пара связывает текстовый цвет с цветом
 * фона.
 */
void initColors() {
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);   // желтый
  init_pair(2, COLOR_MAGENTA, COLOR_BLACK);  // фиолетовый
  init_pair(3, COLOR_GREEN, COLOR_BLACK);    // зеленый
  init_pair(4, COLOR_RED, COLOR_BLACK);      // красный
  init_pair(5, COLOR_CYAN, COLOR_BLACK);     // голубой
  init_pair(6, COLOR_BLUE, COLOR_BLACK);     // синий
  init_pair(7, COLOR_WHITE, COLOR_BLACK);    // белый
}

/**
 * @brief Отображает стартовый экран игры.
 *
 * Очищает указанное окно, рисует его границы и выводит текст,
 * предлагающий пользователю начать игру или выйти.
 *
 * @param win Указатель на окно, в котором отображается стартовый экран.
 */
void drawStartScreen(WINDOW *win) {
  werase(win);
  box(win, 0, 0);
  mvwprintw(win, 2, 2, "Hey, do you want to play Tetris?");
  mvwprintw(win, 4, 2, "\t   ༼ つ ◕‿◕ ༽つ");
  mvwprintw(win, 6, 4, "Press 'y' or Enter to start");
  mvwprintw(win, 8, 4, "\t\tOr");
  mvwprintw(win, 10, 4, "\t Press 'q' to quit");
  wrefresh(win);
}

/**
 * @brief Отображает игровое поле и текущую фигуру тетромино.
 *
 * Очищает указанное окно, рисует его границы и отображает текущее состояние
 * игрового поля. Также отображает текущую фигуру тетромино на указанной позиции
 * с учетом её цвета.
 *
 * @param win Указатель на окно, в котором отображается игровое поле.
 * @param field Матрица, представляющая игровое поле.
 * @param tetromino Матрица, представляющая текущую фигуру тетромино.
 * @param posX Горизонтальная позиция фигуры тетромино на игровом поле.
 * @param posY Вертикальная позиция фигуры тетромино на игровом поле.
 * @param tetromino_color Цвет текущей фигуры тетромино.
 */
void drawGameField(WINDOW *win, int field[20][10], int tetromino[4][4],
                   int posX, int posY, int tetromino_color) {
  werase(win);
  box(win, 0, 0);
  mvwprintw(win, 0, 6, "┤ TETRIS ├");

  for (int y = 0; y < 20; ++y) {
    for (int x = 0; x < 10; ++x) {
      if (field[y][x]) {  // если клетка занята, используем сохранённый цвет
        wattron(win, COLOR_PAIR(field[y][x]));
        mvwprintw(win, y + 1, (x * 2) + 1, "▓▓");  // ⟦⟧ или ▓▓ ░░ ██
        wattroff(win, COLOR_PAIR(field[y][x]));
      } else {
        mvwprintw(win, y + 1, (x * 2) + 1, " ⸱");  // ⋆☆✫⟡✶⚛⸼⸱
      }
    }
  }

  // отрисовка текущей фигуры с использованием её цвета
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (tetromino[y][x] && posY + y >= 0) {
        wattron(win, COLOR_PAIR(tetromino_color));
        mvwprintw(win, posY + y + 1, (posX + x) * 2 + 1, "▓▓");
        wattroff(win, COLOR_PAIR(tetromino_color));
      }
    }
  }

  wrefresh(win);
}

/**
 * @brief Отображает следующую фигуру тетромино.
 *
 * Очищает указанное окно, рисует его границы и отображает следующую фигуру
 * тетромино с учетом её цвета, которая появится на игровом поле.
 *
 * @param win Указатель на окно, в котором отображается следующая фигура
 * тетромино.
 * @param next Матрица, представляющая следующую фигуру тетромино.
 * @param next_color Цвет следующей фигуры тетромино.
 */
void drawNextTetromino(WINDOW *win, int next[4][4], int next_color) {
  werase(win);
  box(win, 0, 0);
  mvwprintw(win, 0, 6, "┤ NEXT ├");

  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (next[y][x]) {
        wattron(win, COLOR_PAIR(next_color));
        mvwprintw(win, y + 2, (x * 2) + 7, "▓▓");
        wattroff(win, COLOR_PAIR(next_color));
      }
    }
  }
  wrefresh(win);
}

/**
 * @brief Отображает информацию о текущем состоянии игры.
 *
 * Очищает указанное окно, рисует его границы и выводит информацию о текущем
 * счёте, рекорде, уровне и количестве очищенных линий.
 *
 * @param win Указатель на окно, в котором отображается информация о состоянии
 * игры.
 * @param gameInfo Указатель на структуру `GameInfo_t`, содержащую текущую
 * информацию о состоянии игры.
 */
void drawGameInfo(WINDOW *win, GameInfo_t *gameInfo) {
  werase(win);
  box(win, 0, 0);

  mvwprintw(win, 0, 6, "┤ INFO ├");

  mvwprintw(win, 2, 2, "Score:      %d", gameInfo->score);
  mvwprintw(win, 3, 2, "High Score: %d", gameInfo->high_score);
  mvwprintw(win, 4, 2, "Level:      %d", gameInfo->level);
  mvwprintw(win, 5, 2, "Lines:      %d", gameInfo->lines_cleared);
  // mvwprintw(win, 6, 2, "Speed:     %d", gameInfo->speed); для отладки

  wrefresh(win);
}

/**
 * @brief Отображает инструкции по управлению игрой.
 *
 * Очищает указанное окно, рисует его границы и выводит текст с указанием клавиш
 * управления игрой.
 *
 * @param win Указатель на окно, в котором отображаются инструкции по
 * управлению.
 */
void drawInstructions(WINDOW *win) {
  werase(win);
  box(win, 0, 0);

  mvwprintw(win, 0, 4, "┤ CONTROLS ├");

  mvwprintw(win, 2, 2, "Rotate:     w, ↑");
  mvwprintw(win, 3, 2, "Left:       a, ←");
  mvwprintw(win, 4, 2, "Right:      d, →");
  mvwprintw(win, 5, 2, "Down:       s, ↓");
  mvwprintw(win, 6, 2, "Drop:      space");
  mvwprintw(win, 7, 2, "Pause:      p");
  mvwprintw(win, 8, 2, "Mute:       m");
  mvwprintw(win, 9, 2, "Quit:       q");

  wrefresh(win);
}

/**
 * @brief Отображает экран завершения игры.
 *
 * Очищает указанное окно, рисует его границы и выводит сообщение о завершении
 * игры, а также текущий и рекордный счёт.
 *
 * @param win Указатель на окно, в котором отображается экран завершения игры.
 * @param gameInfo Указатель на структуру `GameInfo_t`, содержащую информацию о
 * счёте.
 */
void drawGameOver(WINDOW *win, GameInfo_t *gameInfo) {
  werase(win);
  box(win, 0, 0);
  mvwprintw(win, 2, 5, "  GAME OVER!");
  mvwprintw(win, 4, 4, "(ノಠ益ಠ)ノ彡┻━┻");
  mvwprintw(win, 6, 2, "  Your Score: %d", gameInfo->score);
  mvwprintw(win, 7, 2, "  High Score: %d", gameInfo->high_score);
  wrefresh(win);
}

/**
 * @brief Отображает экран паузы.
 *
 * Очищает указанное окно, рисует его границы и выводит сообщение о паузе в
 * игре.
 *
 * @param win Указатель на окно, в котором отображается экран паузы.
 */
void drawGamePaused(WINDOW *win) {
  werase(win);
  box(win, 0, 0);
  mvwprintw(win, 2, 6, "GAME PAUSED!");
  mvwprintw(win, 4, 7, "☝ (°ロ°)");
  wrefresh(win);
}

/**
 * @brief Освобождает память, занятую окнами, и завершает ncurses.
 *
 * Удаляет все созданные окна и завершает работу ncurses, освобождая занятые
 * ресурсы.
 *
 * @param windows Указатель на структуру `GameWindows`, содержащую все окна
 * игры.
 */
void cleanupWindows(GameWindows *windows) {
  if (windows->startWin) delwin(windows->startWin);
  if (windows->gameWin) delwin(windows->gameWin);
  if (windows->infoWin) delwin(windows->infoWin);
  if (windows->nextWin) delwin(windows->nextWin);
  if (windows->instrWin) delwin(windows->instrWin);
  if (windows->pauseWin) delwin(windows->pauseWin);
  if (windows->gameOverWin) delwin(windows->gameOverWin);
  endwin();
}
