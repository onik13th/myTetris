#include "tetris.h"

/**
 * @brief Фигуры тетромино.
 *
 * Массив, содержащий определения всех 7 типов тетромино и их ориентации.
 */
const int tetrominoes[7][4][4] = {
    {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
    {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // T
    {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // Z
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // S
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // L
    {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // J
};

/**
 * @brief Инициализирует параметры игры.
 *
 * Устанавливает начальные значения для структуры GameInfo_t, включая счет,
 * уровень и скорость игры.
 *
 * @param gameInfo Указатель на структуру с информацией о текущей игре.
 */
void initGame(GameInfo_t *gameInfo) {
  memset(gameInfo, 0, sizeof(GameInfo_t));
  gameInfo->score = 0;
  gameInfo->level = 1;
  gameInfo->speed = 22;  // миллисекунды
  gameInfo->paused = false;
  gameInfo->state = START;
  gameInfo->high_score = loadHighScore();
  gameInfo->is_drop_key_pressed = false;
  gameInfo->holdTimer = 0;
  gameInfo->timer = 0;

  srand(time(0));

  spawnTetromino(gameInfo);
}

/**
 * @brief Обрабатывает ввод пользователя.
 *
 * Выполняет действия в зависимости от переданного действия пользователя
 * (например, движение тетромино или пауза).
 *
 * @param gameInfo Указатель на структуру с информацией о текущей игре.
 * @param audioManager Указатель на аудиоконтроллер для управления звуком.
 * @param action Действие, которое выполнил пользователь.
 * @param hold Флаг, указывает удерживается ли клавиша.
 */
void userInput(GameInfo_t *gameInfo, SoundController *audioManager,
               UserAction_t action, bool hold) {
  switch (action) {
    case Start:
      if (gameInfo->state != PLAY && gameInfo->state != PAUSE) {
        gameInfo->state = PLAY;
        spawnTetromino(gameInfo);
      }
      break;
    case Pause:
      togglePause(gameInfo);
      break;
    case Terminate:
      gameOver(gameInfo);
      break;
    case Mute:
      toggleMute(audioManager);
      break;
    case Left:
      moveLeft(gameInfo);
      break;
    case Right:
      moveRight(gameInfo);
      break;
    case Down:
      moveDown(gameInfo);
      break;
    case Drop:
      gameInfo->is_drop_key_pressed = hold;
      handleDropMovement(gameInfo);
      break;
    case Action:
      handleTetrominoRotation(gameInfo);
      break;
    default:
      break;
  }

  if (!shouldProcessHoldAction(gameInfo, hold)) {
    return;
  }
}

/**
 * @brief Проверяет, следует ли обрабатывать удерживаемую клавишу.
 *
 * Увеличивает таймер удержания клавиши и определяет, достаточно ли времени
 * прошло для повторного выполнения действия.
 *
 * @param gameInfo Указатель на структуру с информацией о текущей игре.
 * @param hold Указывает, удерживается ли клавиша.
 * @return Возвращает true, если действие должно быть выполнено, иначе false.
 */
bool shouldProcessHoldAction(GameInfo_t *gameInfo, bool hold) {
  if (hold) {
    gameInfo->holdTimer++;
    if (gameInfo->holdTimer < HOLD_DELAY) {
      return false;  // пропускаем действия, пока не наступит время для
                     // повторения
    }
    gameInfo->holdTimer = 0;  // сбрасываем таймер для следующей итерации
    return true;
  } else {
    gameInfo->holdTimer = 0;  // сбрасываем таймер, если клавиша не удерживается
    return true;
  }
}

/**
 * @brief Проверяет наличие и создает файл для хранения рекордов.
 *
 * Проверяет существование директории и файла для хранения рекордов. Создает их,
 * если они отсутствуют.
 *
 * @return Возвращает 1 при успешном создании или проверке, 0 в случае ошибки.
 */
static int ensureHighscoreFile() {
  struct stat st = {0};

  if (stat(HIGHSCORE_DIR_PATH, &st) == -1) {
    if (mkdir(HIGHSCORE_DIR_PATH, 0700) != 0) {
      perror("Ошибка создания директории для рекорда");
      return 0;
    }
  }

  FILE *file = fopen(HIGHSCORE_FILE_PATH, "r");
  if (file == NULL) {
    file = fopen(HIGHSCORE_FILE_PATH, "w");
    if (file == NULL) {
      perror("Ошибка создания файла рекорда");
      return 0;
    }
    fprintf(file, "%d\n", 0);
    fclose(file);
  } else {
    fclose(file);
  }

  return 1;
}

/**
 * @brief Загружает рекорд из файла.
 *
 * Загружает максимальный рекорд, сохраненный в файле. Если файл отсутствует или
 * произошла ошибка, возвращает 0.
 *
 * @return Возвращает сохраненный рекорд или 0 в случае ошибки.
 */
int loadHighScore() {
  if (!ensureHighscoreFile()) {
    return 0;  // Возвращаем 0, если не удалось создать директорию или файл
  }

  FILE *file = fopen(HIGHSCORE_FILE_PATH, "r");
  if (file == NULL) {
    perror("Ошибка открытия файла рекорда для чтения");
    return 0;
  }

  int highScore;
  if (fscanf(file, "%d", &highScore) != 1) {
    highScore = -1;
  }

  fclose(file);
  return highScore;
}

/**
 * @brief Сохраняет рекорд в файл.
 *
 * Сохраняет текущий рекорд в файл. Если файл отсутствует, он будет создан.
 *
 * @param score Текущий рекорд для сохранения.
 */
void saveHighScore(int score) {
  if (!ensureHighscoreFile()) {
    return;  // выходим, если не удалось создать директорию или файл
  }

  FILE *file = fopen(HIGHSCORE_FILE_PATH, "w");
  if (file == NULL) {
    perror("Ошибка открытия файла для записи рекорда");
    return;
  }

  fprintf(file, "%d\n", score);
  fclose(file);
}

/**
 * @brief Обрабатывает ускоренное падение тетромино вниз.
 *
 * Если тетромино не сталкивается с другими блоками, оно перемещается вниз с
 * ускорением. В противном случае происходит обработка приземления фигуры.
 *
 * @param gameInfo Указатель на структуру с информацией о текущей игре.
 */
void handleDropMovement(GameInfo_t *gameInfo) {
  if (!checkCollision(gameInfo, 0, 1)) {
    gameInfo->current_piece.posY++;
  } else {
    handleTetrominoLanding(gameInfo);
  }
}

/**
 * @brief Обрабатывает приземление тетромино.
 *
 * Выполняет слияние тетромино с полем, очистку заполненных линий и спавн нового
 * тетромино.
 *
 * @param gameInfo Указатель на структуру с информацией о текущей игре.
 */
void handleTetrominoLanding(GameInfo_t *gameInfo) {
  mergeTetromino(gameInfo);
  clearLines(gameInfo);
  spawnTetromino(gameInfo);
}

/**
 * @brief Перемещает тетромино влево.
 *
 * Проверяет коллизию и перемещает тетромино влево, если движение возможно.
 *
 * @param gameInfo Указатель на структуру с информацией о текущей игре.
 */
void moveLeft(GameInfo_t *gameInfo) {
  if (!checkCollision(gameInfo, -1, 0)) {
    gameInfo->current_piece.posX--;
  }
}

/**
 * @brief Перемещает тетромино вправо.
 *
 * Проверяет коллизию и перемещает тетромино вправо, если движение возможно.
 *
 * @param gameInfo Указатель на структуру с информацией о текущей игре.
 */
void moveRight(GameInfo_t *gameInfo) {
  if (!checkCollision(gameInfo, 1, 0)) {
    gameInfo->current_piece.posX++;
  }
}

/**
 * @brief Перемещает тетромино вниз.
 *
 * Проверяет коллизию и перемещает тетромино вниз, если движение возможно.
 *
 * @param gameInfo Указатель на структуру с информацией о текущей игре.
 */
void moveDown(GameInfo_t *gameInfo) {
  if (!checkCollision(gameInfo, 0, 1)) {
    gameInfo->current_piece.posY++;
  }
}

/**
 * @brief Переключает состояние игры между паузой и воспроизведением.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 */
void togglePause(GameInfo_t *gameInfo) {
  if (gameInfo->state == PLAY) {
    gameInfo->state = PAUSE;
  } else if (gameInfo->state == PAUSE) {
    gameInfo->state = PLAY;
  }
}

/**
 * @brief Вращает тетромино на игровом поле.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 * @param rotated Матрица 4x4 для сохранения результата вращения.
 * @param type Тип тетромино, определяющий, как будет выполнено вращение.
 */
void rotateTetromino(GameInfo_t *gameInfo, int rotated[4][4], int type) {
  if (type == 0 || type == 2) {
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        rotated[x][y] = gameInfo->current_piece.form[3 - y][x];
      }
    }
  } else {
    for (int y = 0; y < 3; y++) {
      for (int x = 0; x < 3; x++) {
        rotated[x][y] = gameInfo->current_piece.form[2 - y][x];
      }
    }
  }
}

/**
 * @brief Создает резервную копию текущей формы тетромино и применяет вращение.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 * @param backup Матрица 4x4 для хранения резервной копии текущей формы
 * тетромино.
 * @param rotated Матрица 4x4 с новой формой тетромино после вращения.
 */
void backupAndRotateTetromino(GameInfo_t *gameInfo, int backup[4][4],
                              int rotated[4][4]) {
  memcpy(backup, gameInfo->current_piece.form,
         sizeof(gameInfo->current_piece.form));
  memcpy(gameInfo->current_piece.form, rotated,
         sizeof(gameInfo->current_piece.form));
}

/**
 * @brief Обрабатывает возможные коллизии после вращения тетромино.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 * @param backup Матрица 4x4, содержащий резервную копию формы тетромино.
 */
void handleCollision(GameInfo_t *gameInfo, int backup[4][4]) {
  if (checkCollision(gameInfo, 0, 0)) {
    // Пробуем сдвинуть фигуру на 1 блок влево
    if (!checkCollision(gameInfo, -1, 0)) {
      gameInfo->current_piece.posX -= 1;
    } else if (!checkCollision(gameInfo, 1, 0)) {
      gameInfo->current_piece.posX += 1;
    } else if (!checkCollision(gameInfo, -2, 0)) {
      gameInfo->current_piece.posX -= 2;
    } else if (!checkCollision(gameInfo, 2, 0)) {
      gameInfo->current_piece.posX += 2;
    } else {
      memcpy(gameInfo->current_piece.form, backup,
             sizeof(gameInfo->current_piece.form));
    }
  }
}

/**
 * @brief Выполняет вращение тетромино с проверкой на коллизии.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 */
void handleTetrominoRotation(GameInfo_t *gameInfo) {
  int rotated[4][4] = {0};
  int type = gameInfo->current_piece.type;
  int backup[4][4] = {0};

  rotateTetromino(gameInfo, rotated, type);  // вращаем фигуру
  backupAndRotateTetromino(
      gameInfo, backup,
      rotated);  // сохраняем резервную копию и применяем вращение
  handleCollision(gameInfo, backup);  // обрабатываем возможную коллизию
}

/**
 * @brief Обновляет текущее состояние игры в зависимости от игровых событий.
 *
 * @param gameInfo Структура GameInfo_t, содержащая текущую информацию об игре.
 * @return Обновленная структура GameInfo_t.
 */
GameInfo_t updateCurrentState(GameInfo_t gameInfo) {
  if (gameInfo.state == PLAY) {
    gameInfo.timer++;
    int effectiveSpeed = gameInfo.speed;

    if (gameInfo.is_drop_key_pressed) {
      effectiveSpeed = 0;
    }

    if (gameInfo.timer >= effectiveSpeed) {
      gameInfo.timer = 0;
      handleDropMovement(&gameInfo);
    }
  }
  return gameInfo;
}

/**
 * @brief Создает новое тетромино на игровом поле.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 */
void spawnTetromino(GameInfo_t *gameInfo) {
  if (gameInfo->next_piece.type != -1) {
    gameInfo->current_piece = gameInfo->next_piece;
  } else {
    int r = rand() % 7;
    memcpy(gameInfo->current_piece.form, tetrominoes[r],
           sizeof(gameInfo->current_piece.form));
    gameInfo->current_piece.color = rand() % 7 + 1;
    gameInfo->current_piece.type = r;
  }

  int r = rand() % 7;
  memcpy(gameInfo->next_piece.form, tetrominoes[r],
         sizeof(gameInfo->next_piece.form));
  gameInfo->next_piece.color = rand() % 7 + 1;
  gameInfo->next_piece.type = r;

  gameInfo->current_piece.posX = 3;
  gameInfo->current_piece.posY = 0;

  gameInfo->is_drop_key_pressed = false;
  gameInfo->holdTimer = 0;
  gameInfo->timer = 0;

  if (checkCollision(gameInfo, 0, 0)) {
    gameOver(gameInfo);
  }
}

/**
 * @brief Проверяет наличие коллизий между тетромино и другими объектами на
 * поле.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 * @param offsetX Смещение по оси X.
 * @param offsetY Смещение по оси Y.
 * @return true, если коллизия есть, иначе false.
 */
bool checkCollision(GameInfo_t *gameInfo, int offsetX, int offsetY) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (gameInfo->current_piece.form[y][x]) {
        int newX = gameInfo->current_piece.posX + x + offsetX;
        int newY = gameInfo->current_piece.posY + y + offsetY;

        if (newX < 0 || newX >= WIDTH || newY >= HEIGHT) {
          return true;
        }

        if (newY >= 0 && gameInfo->field[newY][newX]) {
          return true;
        }
      }
    }
  }

  return false;
}

/**
 * @brief Вставляет текущий тетромино в игровое поле.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 */
void mergeTetromino(GameInfo_t *gameInfo) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (gameInfo->current_piece.form[y][x] &&
          gameInfo->current_piece.posY + y >= 0) {
        gameInfo->field[gameInfo->current_piece.posY + y]
                       [gameInfo->current_piece.posX + x] =
            gameInfo->current_piece.color;
      }
    }
  }
}

/**
 * @brief Очищает заполненные линии на игровом поле и обновляет счет и уровень.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 */
void clearLines(GameInfo_t *gameInfo) {
  int linesCleared = clearFullLines(gameInfo);
  gameInfo->lines_cleared += linesCleared;

  int points = calculateScore(linesCleared);
  gameInfo->score += points;

  if (gameInfo->score > gameInfo->high_score) {
    gameInfo->high_score = gameInfo->score;
    saveHighScore(gameInfo->high_score);
  }

  updateLevelAndSpeed(gameInfo);
}

/**
 * @brief Очищает полностью заполненные линии на игровом поле.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 * @return Количество очищенных линий.
 */
int clearFullLines(GameInfo_t *gameInfo) {
  int linesCleared = 0;

  for (int y = 0; y < HEIGHT; y++) {
    bool isFull = true;
    for (int x = 0; x < WIDTH; x++) {
      if (gameInfo->field[y][x] == 0) {
        isFull = false;
        break;
      }
    }

    if (isFull) {
      for (int row = y; row > 0; row--) {
        for (int col = 0; col < WIDTH; col++) {
          gameInfo->field[row][col] = gameInfo->field[row - 1][col];
        }
      }

      for (int col = 0; col < WIDTH; col++) {
        gameInfo->field[0][col] = 0;
      }

      linesCleared++;
    }
  }
  return linesCleared;
}

/**
 * @brief Рассчитывает очки за очищенные линии.
 *
 * @param linesCleared Количество очищенных линий.
 * @return Количество очков.
 */
int calculateScore(int linesCleared) {
  switch (linesCleared) {
    case 1:
      return 100;
    case 2:
      return 300;
    case 3:
      return 700;
    case 4:
      return 1500;
    default:
      return 0;
  }
}

/**
 * @brief Обновляет уровень и скорость игры на основе текущего счета.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 */
void updateLevelAndSpeed(GameInfo_t *gameInfo) {
  int newLevel =
      gameInfo->score / 600 + 1;  // определяем новый уровень каждые 600 очков
  if (newLevel > 10) {
    newLevel = 10;  // максимум 10 уровней
  }
  gameInfo->level = newLevel;
  gameInfo->speed =
      22 -
      (gameInfo->level *
       2);  // уменьшаем скорость(задержу падения) по мере увеличения уровня
}

/**
 * @brief Завершает игру и сохраняет рекорд, если он был побит.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 */
void gameOver(GameInfo_t *gameInfo) {
  gameInfo->state = GAME_OVER;
  gameInfo->gameOverTime = time(NULL);  // устанавливаем время завершения игры

  // если текущий счет выше рекорда
  if (gameInfo->score > gameInfo->high_score) {
    gameInfo->high_score = gameInfo->score;
    saveHighScore(gameInfo->high_score);  // сохраняем новый рекорд
  }
}

/**
 * @brief Проверяет, следует ли завершить игру.
 *
 * @param gameInfo Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 * @param action Действие пользователя (например, завершение игры).
 * @param running Указатель на флаг, показывающий, продолжается ли игра.
 */

void shouldExitGame(GameInfo_t *gameInfo, UserAction_t action, bool *running) {
  time_t currentTime = time(NULL);
  if (difftime(currentTime, gameInfo->gameOverTime) >= 5) {
    *running = false;  // заканчиваем игру через 5 секунд
  } else if (action == Terminate) {
    *running = false;  // завершаем игру, если пользователь нажал 'q'
  }
}

/**
 * @brief Переключает режим звука между включенным и выключенным.
 *
 * @param audioManager Указатель на структуру SoundController, управляющую
 * звуком.
 */
void toggleMute(SoundController *audioManager) {
  if (audioManager->mute) {
    Mix_VolumeMusic(MIX_MAX_VOLUME);  // возвращаем громкость на максимум
    audioManager->mute = false;
  } else {
    Mix_VolumeMusic(0);  // устанавливаем громкость в 0
    audioManager->mute = true;
  }
}