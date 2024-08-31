#include "input_handler.h"

/**
 * @brief Получает действие пользователя на основе ввода с клавиатуры, учитывая
 * настройки ncurses.
 *
 * Функция `getUserAction` обрабатывает пользовательский ввод, используя
 * библиотеку ncurses, и возвращает соответствующее действие. Она поддерживает
 * стандартные клавиши управления и определенные символы, такие как Enter,
 * пробел и другие.
 *
 * @param keyDropPressed Указатель на логическую переменную, указывающую, была
 * ли нажата клавиша Drop (пробел). Если клавиша пробела была нажата и не
 * удерживалась, функция установит этот флаг в true и вернет действие Drop. В
 * противном случае, сбросит флаг в false.
 *
 * @return UserAction_t Возвращает действие пользователя (например, Move Left,
 * Start, Drop и т. д.) на основе нажатой клавиши. Если нажатие клавиши не
 * распознано, возвращает None.
 */
UserAction_t getUserAction(bool *keyDropPressed) {
  int ch = getch();
  UserAction_t action = None;

  switch (ch) {
    case 'm':
    case 'M':
      action = Mute;
      break;
    case '\n':  // Enter
    case 'y':
      action = Start;
      break;
    case 'q':
      action = Terminate;
      break;
    case 'p':
      action = Pause;
      break;
    case KEY_LEFT:
    case 'a':
      action = Left;
      break;
    case KEY_RIGHT:
    case 'd':
      action = Right;
      break;
    case KEY_DOWN:
    case 's':
      action = Down;
      break;
    case ' ':  // Space
      if (!(*keyDropPressed)) {
        *keyDropPressed = true;
        action = Drop;
      }
      break;
    case KEY_UP:
    case 'w':
      action = Action;
      break;
    default:
      *keyDropPressed = false;
      break;
  }

  return action;
}