#include <check.h>
#include <stdlib.h>
#include <time.h>

#include "../tetris.h"

// для инициализации структуры GameInfo_t
void setup_game_info(GameInfo_t *gameInfo) { initGame(gameInfo); }

START_TEST(test_userInput_Start) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  SoundController audioManager = {0};

  userInput(&gameInfo, &audioManager, Start, false);

  ck_assert_int_eq(gameInfo.state, PLAY);
}
END_TEST

START_TEST(test_userInput_Pause) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  SoundController audioManager = {0};
  gameInfo.state = PLAY;

  userInput(&gameInfo, &audioManager, Pause, false);

  ck_assert_int_eq(gameInfo.state, PAUSE);
}
END_TEST

START_TEST(test_userInput_Terminate) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  SoundController audioManager = {0};

  userInput(&gameInfo, &audioManager, Terminate, false);

  ck_assert_int_eq(gameInfo.state, GAME_OVER);
}
END_TEST

START_TEST(test_userInput_Mute) {
  SoundController audioManager = {0};
  audioManager.mute = false;
  GameInfo_t gameInfo = {0};

  userInput(&gameInfo, &audioManager, Mute, false);

  ck_assert_int_eq(audioManager.mute, true);
}
END_TEST

START_TEST(test_userInput_Left) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  SoundController audioManager = {0};

  spawnTetromino(&gameInfo);
  gameInfo.current_piece.posX = 5;

  userInput(&gameInfo, &audioManager, Left, false);

  ck_assert_int_eq(gameInfo.current_piece.posX, 4);
}
END_TEST

START_TEST(test_userInput_Right) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  SoundController audioManager = {0};

  spawnTetromino(&gameInfo);
  gameInfo.current_piece.posX = 5;

  userInput(&gameInfo, &audioManager, Right, false);

  ck_assert_int_eq(gameInfo.current_piece.posX, 6);
}
END_TEST

START_TEST(test_userInput_Down) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  SoundController audioManager = {0};

  spawnTetromino(&gameInfo);
  gameInfo.current_piece.posY = 0;

  userInput(&gameInfo, &audioManager, Down, false);

  ck_assert_int_eq(gameInfo.current_piece.posY, 1);
}
END_TEST

START_TEST(test_userInput_Drop) {
  GameInfo_t gameInfo = {0};
  SoundController audioManager = {0};
  setup_game_info(&gameInfo);

  // Изначально флаг is_drop_key_pressed должен быть ложным
  ck_assert_int_eq(gameInfo.is_drop_key_pressed, false);

  // Тестируем Drop с hold = true
  userInput(&gameInfo, &audioManager, Drop, true);
  ck_assert_int_eq(gameInfo.is_drop_key_pressed, true);

  // Сбрасываем флаг и тестируем Drop с hold = false
  gameInfo.is_drop_key_pressed = false;
  userInput(&gameInfo, &audioManager, Drop, false);
  ck_assert_int_eq(gameInfo.is_drop_key_pressed, false);
}
END_TEST

START_TEST(test_userInput_Action) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  SoundController audioManager = {0};

  spawnTetromino(&gameInfo);
  int original[4][4];
  memcpy(original, gameInfo.current_piece.form, sizeof(original));

  if (gameInfo.current_piece.type != 2) {
    userInput(&gameInfo, &audioManager, Action, false);
    ck_assert_int_ne(
        memcmp(original, gameInfo.current_piece.form, sizeof(original)), 0);
  } else {
    ck_assert_int_eq(
        memcmp(original, gameInfo.current_piece.form, sizeof(original)), 0);
  }
}
END_TEST

START_TEST(test_userInput_Default) {
  GameInfo_t gameInfo = {0};
  SoundController audioManager = {0};
  setup_game_info(&gameInfo);

  // Сохраняем исходные значения полей для сравнения после выполнения
  // неизвестного действия
  int initialPosX = gameInfo.current_piece.posX;
  int initialPosY = gameInfo.current_piece.posY;
  int initialState = gameInfo.state;

  // Создаем неизвестное действие
  UserAction_t unknownAction =
      (UserAction_t)(Action +
                     10);  // Неизвестное действие за пределами известных

  // Вызываем userInput с неизвестным действием
  userInput(&gameInfo, &audioManager, unknownAction, false);

  // Проверяем, что никакие поля не изменились
  ck_assert_int_eq(gameInfo.current_piece.posX, initialPosX);
  ck_assert_int_eq(gameInfo.current_piece.posY, initialPosY);
  ck_assert_int_eq(gameInfo.state, initialState);
}
END_TEST

START_TEST(test_handleCollision) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  int backup[4][4] = {0};

  spawnTetromino(&gameInfo);
  memcpy(backup, gameInfo.current_piece.form, sizeof(backup));

  // Проверяем поведение на границе, где должен сработать сдвиг на 2 позиции
  gameInfo.current_piece.posX = 0;  // Левая граница
  handleCollision(&gameInfo, backup);

  ck_assert_int_eq(memcmp(gameInfo.current_piece.form, backup, sizeof(backup)),
                   0);

  // Проверяем поведение на границе справа
  gameInfo.current_piece.posX = WIDTH - 1;  // Правая граница
  handleCollision(&gameInfo, backup);

  ck_assert_int_eq(memcmp(gameInfo.current_piece.form, backup, sizeof(backup)),
                   0);
}
END_TEST

START_TEST(test_calculateScore) {
  ck_assert_int_eq(calculateScore(1), 100);
  ck_assert_int_eq(calculateScore(2), 300);
  ck_assert_int_eq(calculateScore(3), 700);
  ck_assert_int_eq(calculateScore(4), 1500);
  ck_assert_int_eq(calculateScore(0), 0);
}
END_TEST

START_TEST(test_shouldExitGame) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  bool running = true;

  // Проверка, что игра завершается через 5 секунд после gameOverTime
  gameInfo.gameOverTime = time(NULL) - 6;
  shouldExitGame(&gameInfo, None, &running);
  ck_assert_int_eq(running, false);

  // Проверка завершения игры при нажатии Terminate (клавиша 'q')
  running = true;
  gameInfo.gameOverTime = time(NULL);  // Устанавливаем время внутри 5 секунд
  shouldExitGame(&gameInfo, Terminate, &running);
  ck_assert_int_eq(running, false);
}
END_TEST

START_TEST(test_initGame) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  ck_assert_int_eq(gameInfo.score, 0);
  ck_assert_int_eq(gameInfo.level, 1);
  ck_assert_int_eq(gameInfo.speed, 22);
  ck_assert_int_eq(gameInfo.paused, false);
  ck_assert_int_eq(gameInfo.state, START);
  ck_assert_int_eq(gameInfo.is_drop_key_pressed, false);
}
END_TEST

START_TEST(test_spawnTetromino) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  spawnTetromino(&gameInfo);

  ck_assert_int_ge(gameInfo.current_piece.type, 0);
  ck_assert_int_lt(gameInfo.current_piece.type, 7);
  ck_assert_int_ge(gameInfo.next_piece.type, 0);
  ck_assert_int_lt(gameInfo.next_piece.type, 7);
}
END_TEST

START_TEST(test_moveLeft) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  spawnTetromino(&gameInfo);
  gameInfo.current_piece.posX = 5;

  moveLeft(&gameInfo);

  ck_assert_int_eq(gameInfo.current_piece.posX, 4);
}
END_TEST

START_TEST(test_moveRight) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  spawnTetromino(&gameInfo);
  gameInfo.current_piece.posX = 5;

  moveRight(&gameInfo);

  ck_assert_int_eq(gameInfo.current_piece.posX, 6);
}
END_TEST

START_TEST(test_handleDropMovement) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  spawnTetromino(&gameInfo);
  gameInfo.current_piece.posY = 0;

  handleDropMovement(&gameInfo);

  ck_assert_int_eq(gameInfo.current_piece.posY, 1);
}
END_TEST

START_TEST(test_handleTetrominoRotation) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  spawnTetromino(&gameInfo);
  int original[4][4];
  memcpy(original, gameInfo.current_piece.form, sizeof(original));
  if (gameInfo.current_piece.type != 2) {
    handleTetrominoRotation(&gameInfo);
    ck_assert_int_ne(
        memcmp(original, gameInfo.current_piece.form, sizeof(original)), 0);
  } else {
    ck_assert_int_eq(
        memcmp(original, gameInfo.current_piece.form, sizeof(original)), 0);
  }
}
END_TEST

START_TEST(test_checkCollision) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  spawnTetromino(&gameInfo);
  gameInfo.current_piece.posX = 0;
  gameInfo.current_piece.posY = 0;

  // -2 потому что некоторые фигуры спавнясб в x = 0
  // надо сместить на 2 позиции для коллизии
  ck_assert_int_eq(checkCollision(&gameInfo, -2, 0), true);
}
END_TEST

START_TEST(test_clearLines) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  for (int i = 0; i < WIDTH; ++i) {
    gameInfo.field[0][i] = 1;
  }

  clearLines(&gameInfo);

  for (int i = 0; i < WIDTH; ++i) {
    ck_assert_int_eq(gameInfo.field[0][i], 0);
  }
}
END_TEST

START_TEST(test_gameOver) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  int originalScore = loadHighScore();

  gameInfo.high_score = 500;

  gameInfo.score = 1000;
  gameOver(&gameInfo);

  int current = loadHighScore();

  ck_assert_int_eq(gameInfo.state, GAME_OVER);
  ck_assert_int_eq(gameInfo.high_score, current);

  saveHighScore(originalScore);
}
END_TEST

START_TEST(test_updateCurrentState) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  gameInfo.state = PLAY;
  gameInfo.timer = 21;

  gameInfo.is_drop_key_pressed = true;
  gameInfo = updateCurrentState(gameInfo);

  ck_assert_int_eq(gameInfo.timer, 0);
}
END_TEST

START_TEST(test_togglePause) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  gameInfo.state = PLAY;
  togglePause(&gameInfo);

  ck_assert_int_eq(gameInfo.state, PAUSE);

  togglePause(&gameInfo);

  ck_assert_int_eq(gameInfo.state, PLAY);
}
END_TEST

START_TEST(test_saveLoadHighScore) {
  int original = loadHighScore();
  int score = 1234;
  saveHighScore(score);

  int loadedScore = loadHighScore();

  ck_assert_int_eq(score, loadedScore);
  saveHighScore(original);
}
END_TEST

START_TEST(test_mergeTetromino) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  gameInfo.current_piece.type = 1;
  gameInfo.current_piece.color = 2;
  gameInfo.current_piece.posX = 0;
  gameInfo.current_piece.posY = 0;

  gameInfo.current_piece.form[0][0] = 1;

  mergeTetromino(&gameInfo);

  ck_assert_int_eq(gameInfo.field[0][0], 2);
}
END_TEST

START_TEST(test_spawnTetromino_with_default) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  gameInfo.next_piece.type = -1;

  spawnTetromino(&gameInfo);

  ck_assert_int_ge(gameInfo.current_piece.type, 0);
  ck_assert_int_lt(gameInfo.current_piece.type, 7);
  ck_assert_int_ge(gameInfo.current_piece.color, 1);
  ck_assert_int_lt(gameInfo.current_piece.color, 8);
}
END_TEST

START_TEST(test_shouldProcessHoldAction) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  gameInfo.holdTimer = 0;

  bool result = shouldProcessHoldAction(&gameInfo, true);
  ck_assert_int_eq(result, false);

  gameInfo.holdTimer = HOLD_DELAY;

  result = shouldProcessHoldAction(&gameInfo, true);
  ck_assert_int_eq(result, true);
}
END_TEST

START_TEST(test_handleCollision_with_backup) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  int backup[4][4] = {0};

  spawnTetromino(&gameInfo);
  memcpy(backup, gameInfo.current_piece.form, sizeof(backup));

  gameInfo.current_piece.posX = 0;
  handleCollision(&gameInfo, backup);

  ck_assert_int_eq(memcmp(gameInfo.current_piece.form, backup, sizeof(backup)),
                   0);
}
END_TEST

START_TEST(test_rotateTetrominoSecond) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  const int tetrominoes[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
      {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // T
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // Z
      {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // S
      {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // L
      {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // J
  };

  int rotated[4][4] = {0};

  spawnTetromino(&gameInfo);

  for (int type = 0; type < 7; type++) {
    if (type == 2) {
      continue;
    }

    // Устанавливаем тип тетромино
    gameInfo.current_piece.type = type;

    // Обновляем форму для данного типа тетромино
    memcpy(gameInfo.current_piece.form, tetrominoes[type],
           sizeof(gameInfo.current_piece.form));

    // Сохраняем оригинальную форму для проверки
    int original[4][4];
    memcpy(original, gameInfo.current_piece.form, sizeof(original));

    // Поворачиваем тетромино
    rotateTetromino(&gameInfo, rotated, type);

    // Проверяем, что форма изменилась
    ck_assert_int_ne(memcmp(original, rotated, sizeof(rotated)), 0);
  }
}
END_TEST

START_TEST(test_backupAndRotateTetrominoSecond) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  int rotated[4][4] = {0};
  int backup[4][4] = {0};

  spawnTetromino(&gameInfo);

  memcpy(backup, gameInfo.current_piece.form, sizeof(backup));

  backupAndRotateTetromino(&gameInfo, backup, rotated);

  ck_assert_int_ne(memcmp(backup, rotated, sizeof(rotated)), 0);
}
END_TEST

START_TEST(test_handleCollisionSecond) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);
  int backup[4][4] = {0};

  spawnTetromino(&gameInfo);
  memcpy(backup, gameInfo.current_piece.form, sizeof(backup));

  // Симулируем коллизию
  gameInfo.current_piece.posX = 0;
  handleCollision(&gameInfo, backup);

  ck_assert_int_eq(memcmp(gameInfo.current_piece.form, backup, sizeof(backup)),
                   0);
}
END_TEST

START_TEST(test_handleTetrominoRotationSecond) {
  GameInfo_t gameInfo = {0};
  setup_game_info(&gameInfo);

  spawnTetromino(&gameInfo);
  int original[4][4];
  memcpy(original, gameInfo.current_piece.form, sizeof(original));

  handleTetrominoRotation(&gameInfo);

  if (gameInfo.current_piece.type != 2) {
    ck_assert_int_ne(
        memcmp(original, gameInfo.current_piece.form, sizeof(original)), 0);
  } else {
    ck_assert_int_eq(
        memcmp(original, gameInfo.current_piece.form, sizeof(original)), 0);
  }
}
END_TEST

START_TEST(test_toggleMuteSecond) {
  SoundController audioManager = {0};
  audioManager.mute = false;

  toggleMute(&audioManager);
  ck_assert_int_eq(audioManager.mute, true);

  toggleMute(&audioManager);
  ck_assert_int_eq(audioManager.mute, false);
}
END_TEST

Suite *tetris_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Tetris");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_initGame);
  tcase_add_test(tc_core, test_spawnTetromino);
  tcase_add_test(tc_core, test_moveLeft);
  tcase_add_test(tc_core, test_moveRight);
  tcase_add_test(tc_core, test_handleDropMovement);
  tcase_add_test(tc_core, test_handleTetrominoRotation);
  tcase_add_test(tc_core, test_checkCollision);
  tcase_add_test(tc_core, test_clearLines);
  tcase_add_test(tc_core, test_gameOver);
  tcase_add_test(tc_core, test_updateCurrentState);
  tcase_add_test(tc_core, test_togglePause);
  tcase_add_test(tc_core, test_saveLoadHighScore);
  tcase_add_test(tc_core, test_userInput_Start);
  tcase_add_test(tc_core, test_userInput_Pause);
  tcase_add_test(tc_core, test_userInput_Terminate);
  tcase_add_test(tc_core, test_userInput_Mute);
  tcase_add_test(tc_core, test_userInput_Left);
  tcase_add_test(tc_core, test_userInput_Right);
  tcase_add_test(tc_core, test_userInput_Down);
  tcase_add_test(tc_core, test_userInput_Drop);
  tcase_add_test(tc_core, test_userInput_Action);
  tcase_add_test(tc_core, test_userInput_Default);
  tcase_add_test(tc_core, test_handleCollision);
  tcase_add_test(tc_core, test_calculateScore);
  tcase_add_test(tc_core, test_shouldExitGame);
  tcase_add_test(tc_core, test_mergeTetromino);
  tcase_add_test(tc_core, test_spawnTetromino_with_default);
  tcase_add_test(tc_core, test_shouldProcessHoldAction);
  tcase_add_test(tc_core, test_handleCollision_with_backup);
  tcase_add_test(tc_core, test_rotateTetrominoSecond);
  tcase_add_test(tc_core, test_backupAndRotateTetrominoSecond);
  tcase_add_test(tc_core, test_handleCollisionSecond);
  tcase_add_test(tc_core, test_handleTetrominoRotationSecond);
  tcase_add_test(tc_core, test_toggleMuteSecond);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = tetris_suite();
  sr = srunner_create(s);

  // srunner_run_all(sr, CK_NORMAL);
  srunner_run_all(sr, CK_VERBOSE);

  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
