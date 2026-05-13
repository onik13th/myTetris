#ifndef TETRIS_H
#define TETRIS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>   // для функции mkdir
#include <sys/types.h>  // для типа mode_t
#include <time.h>

#define WIDTH 10
#define HEIGHT 20
#define HIGHSCORE_DIR_PATH \
  "../game_info"  // относительный путь к директории с рекордом
#define HIGHSCORE_FILE_PATH \
  "../game_info/highscore.txt"  // относительный путь к файлу с рекордом
#define HOLD_DELAY \
  2  // задержка для повторного срабатывания при удержании (можно настроить)

/**
 * @brief Состояния игры.
 */
typedef enum {
  START, /**< Игра только началась. */
  PLAY, /**< Игра активна, пользователь управляет тетромино. */
  PAUSE,     /**< Игра приостановлена. */
  GAME_OVER, /**< Игра завершена. */
} GameState_t;

/**
 * @brief Действия пользователя.
 */
typedef enum {
  None,  /**< Нет действия. */
  Start, /**< Начать игру. */
  Pause, /**< Поставить игру на паузу или снять с паузы. */
  Terminate, /**< Завершить игру. */
  Left,      /**< Двигать тетромино влево. */
  Right,     /**< Двигать тетромино вправо. */
  Up, /**< Действие при движении вверх (не используется). */
  Down,   /**< Двигать тетромино вниз. */
  Drop,   /**< Уронить тетромино сразу на дно. */
  Action, /**< Повернуть тетромино. */
  Mute    /**< Включить или выключить звук. */
} UserAction_t;

/**
 * @brief Структура для представления текущего и следующего тетромино.
 */
typedef struct {
  int form[4][4]; /**< Матрица 4x4 для хранения формы тетромино. */
  int type;       /**< Тип тетромино (I, O, T и т.д.). */
  int color;      /**< Цвет тетромино. */
  int posX; /**< Позиция тетромино по оси X на игровом поле. */
  int posY; /**< Позиция тетромино по оси Y на игровом поле. */
} Fragment_t;

/**
 * @brief Основная структура для хранения состояния игры.
 */
typedef struct {
  int field[HEIGHT][WIDTH]; /**< Двумерный массив для хранения состояния
                               игрового поля. */
  Fragment_t current_piece; /**< Текущий тетромино, управляемый игроком. */
  Fragment_t
      next_piece; /**< Следующий тетромино, который появится после текущего. */
  int score;      /**< Текущий счет игрока. */
  int high_score; /**< Рекордный счет. */
  int level;      /**< Текущий уровень игры. */
  int speed;      /**< Скорость падения тетромино. */
  int lines_cleared; /**< Количество убранных линий. */
  bool paused; /**< Флаг, показывающий, находится ли игра на паузе. */
  GameState_t state;   /**< Текущее состояние игры. */
  time_t gameOverTime; /**< Время окончания игры. */
  int holdTimer; /**< Счетчик для обработки удержания клавиш. */
  int timer; /**< Таймер для контроля скорости падения фигуры. */
  bool
      is_drop_key_pressed; /**< Флаг для отслеживания удержания клавиши Drop. */
} GameInfo_t;

/**
 * @brief Структура для управления звуковыми эффектами.
 */
typedef struct {
  Mix_Music *music; /**< Указатель на текущую фоновую музыку. */
  bool mute; /**< Флаг, показывающий, выключен ли звук. */
} SoundController;

/**
 * @brief Инициализация состояния игры.
 * @param gameInfo Указатель на структуру GameInfo_t, которая будет
 * инициализирована.
 */
void initGame(GameInfo_t *gameInfo);

/**
 * @brief Обработка ввода пользователя.
 * @param gameInfo Указатель на структуру GameInfo_t.
 * @param audioManager Указатель на структуру SoundController.
 * @param action Действие пользователя.
 * @param hold Флаг, указывающий на удержание клавиши.
 */
void userInput(GameInfo_t *gameInfo, SoundController *audioManager,
               UserAction_t action, bool hold);

/**
 * @brief Проверка необходимости обработки удержания клавиши.
 * @param gameInfo Указатель на структуру GameInfo_t.
 * @param hold Флаг удержания клавиши.
 * @return Возвращает true, если удержание клавиши должно быть обработано.
 */
bool shouldProcessHoldAction(GameInfo_t *gameInfo, bool hold);

/**
 * @brief Загрузка рекордного счета из файла.
 * @return Возвращает рекордный счет.
 */
int loadHighScore();

/**
 * @brief Сохранение рекордного счета в файл.
 * @param score Счет, который будет сохранен как рекордный.
 */
void saveHighScore(int score);

/**
 * @brief Обработка падения тетромино.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void handleDropMovement(GameInfo_t *gameInfo);

/**
 * @brief Обработка приземления тетромино.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void handleTetrominoLanding(GameInfo_t *gameInfo);

/**
 * @brief Движение тетромино влево.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void moveLeft(GameInfo_t *gameInfo);

/**
 * @brief Движение тетромино вправо.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void moveRight(GameInfo_t *gameInfo);

/**
 * @brief Движение тетромино вниз.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void moveDown(GameInfo_t *gameInfo);

/**
 * @brief Переключение паузы в игре.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void togglePause(GameInfo_t *gameInfo);

/**
 * @brief Поворот тетромино.
 * @param gameInfo Указатель на структуру GameInfo_t.
 * @param rotated Матрица 4x4 для хранения повёрнутой формы.
 * @param type Тип тетромино, который нужно повернуть.
 */
void rotateTetromino(GameInfo_t *gameInfo, int rotated[4][4], int type);

/**
 * @brief Резервное копирование и поворот тетромино.
 * @param gameInfo Указатель на структуру GameInfo_t.
 * @param backup Матрица 4x4 для хранения резервной копии текущей формы.
 * @param rotated Матрица 4x4 для хранения повёрнутой формы.
 */
void backupAndRotateTetromino(GameInfo_t *gameInfo, int backup[4][4],
                              int rotated[4][4]);

/**
 * @brief Обработка коллизий для тетромино.
 * @param gameInfo Указатель на структуру GameInfo_t.
 * @param backup Матрица 4x4 для хранения резервной копии текущей формы.
 */
void handleCollision(GameInfo_t *gameInfo, int backup[4][4]);

/**
 * @brief Обработка поворота тетромино с учетом коллизий.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void handleTetrominoRotation(GameInfo_t *gameInfo);

/**
 * @brief Обновление текущего состояния игры.
 * @param gameInfo Текущая структура состояния игры.
 * @return Возвращает обновленное состояние игры.
 */
GameInfo_t updateCurrentState(GameInfo_t gameInfo);

/**
 * @brief Создание нового тетромино на игровом поле.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void spawnTetromino(GameInfo_t *gameInfo);

/**
 * @brief Проверка на коллизию для текущего тетромино.
 * @param gameInfo Указатель на структуру GameInfo_t.
 * @param offsetX Смещение по оси X.
 * @param offsetY Смещение по оси Y.
 * @return Возвращает true, если есть коллизия.
 */
bool checkCollision(GameInfo_t *gameInfo, int offsetX, int offsetY);

/**
 * @brief Слияние текущего тетромино с игровым полем.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void mergeTetromino(GameInfo_t *gameInfo);

/**
 * @brief Очистка заполненных линий на игровом поле.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void clearLines(GameInfo_t *gameInfo);

/**
 * @brief Очистка заполненных линий и подсчет количества очищенных линий.
 * @param gameInfo Указатель на структуру GameInfo_t.
 * @return Возвращает количество очищенных линий.
 */
int clearFullLines(GameInfo_t *gameInfo);

/**
 * @brief Расчет количества очков за очищенные линии.
 * @param linesCleared Количество очищенных линий.
 * @return Возвращает количество очков, заработанных за очищенные линии.
 */
int calculateScore(int linesCleared);

/**
 * @brief Обновление уровня и скорости игры на основе очищенных линий.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void updateLevelAndSpeed(GameInfo_t *gameInfo);

/**
 * @brief Завершение игры.
 * @param gameInfo Указатель на структуру GameInfo_t.
 */
void gameOver(GameInfo_t *gameInfo);

/**
 * @brief Включение или отключение звука.
 * @param audioManager Указатель на структуру SoundController.
 */
void toggleMute(SoundController *audioManager);

/**
 * @brief Проверка необходимости завершения игры.
 * @param gameInfo Указатель на структуру GameInfo_t.
 * @param action Действие пользователя.
 * @param running Указатель на флаг, показывающий, продолжается ли игра.
 */
void shouldExitGame(GameInfo_t *gameInfo, UserAction_t action, bool *running);

#endif  // TETRIS_H