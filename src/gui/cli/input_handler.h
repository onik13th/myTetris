#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <ncurses.h>

#include "../../brick_game/tetris/tetris.h"

UserAction_t getUserAction(bool *keyDownPressed);

#endif  // INPUT_HANDLER_H