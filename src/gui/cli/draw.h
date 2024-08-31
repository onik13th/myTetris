#ifndef DRAW_H
#define DRAW_H

#include <ncurses.h>

#include "../../brick_game/tetris/tetris.h"

typedef struct {
  WINDOW *startWin;
  WINDOW *gameWin;
  WINDOW *infoWin;
  WINDOW *nextWin;
  WINDOW *instrWin;
  WINDOW *pauseWin;
  WINDOW *gameOverWin;
} GameWindows;

GameWindows initializeWindows();
void initColors();
void drawStartScreen(WINDOW *win);
void drawGameField(WINDOW *win, int field[20][10], int tetromino[4][4],
                   int posX, int posY, int tetromino_color);
void drawNextTetromino(WINDOW *win, int next[4][4], int next_color);
void drawGameInfo(WINDOW *win, GameInfo_t *gameInfo);
void drawInstructions(WINDOW *win);
void drawGameOver(WINDOW *win, GameInfo_t *gameInfo);
void drawGamePaused(WINDOW *win);
void cleanupWindows(GameWindows *windows);

#endif  // DRAW_H