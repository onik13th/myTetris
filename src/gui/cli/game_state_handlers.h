#ifndef GAME_STATE_HANDLERS_H
#define GAME_STATE_HANDLERS_H

#include "../../brick_game/tetris/tetris.h"
#include "audio.h"
#include "draw.h"

void handleStartState(GameWindows *windows);
void handlePlayState(GameWindows *windows, GameInfo_t *gameInfo);
void handlePauseState(GameWindows *windows);
void handleGameOverState(GameWindows *windows, GameInfo_t *gameInfo,
                         UserAction_t action, bool *running);

#endif  // GAME_STATE_HANDLERS_H