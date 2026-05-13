CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
LDFLAGS = -lSDL2 -lSDL2_mixer# -lncurses или -lncursesw добавляется в зависимости от ОС
P = -pedantic
DB = -g -O0
LIB_DIR = brick_game/tetris
GUI_DIR = gui/cli
BIN_DIR = bin
OBJ_DIR = obj
TEST_DIR = brick_game/tetris/tests
LIB_NAME = libtetris.a
DOXYFILE = Doxyfile
PROJECT_NAME = C7_BrickGame_v1.0-1

OS := $(shell uname)
UBUNTU := $(shell hostnamectl | grep -o 'Ubuntu')
MAC_X86 := $(shell uname -a | grep -o _X86_64)
MAC_M1 := $(shell uname -a | grep -o _ARM64)
DARWIN := $(shell uname -a | grep -o 'Darwin')

# настройка путей к Chrome в зависимости от ОС
ifeq ($(OS), Linux)
	LDFLAGS += -lncursesw
	TEST_FLAGS = -lcheck -pthread -lrt -lm -lsubunit
	MATH = -lm
	OPEN = google-chrome
else ifeq ($(UBUNTU), Ubuntu)
	LDFLAGS += -lncursesw
	TEST_FLAGS = -lpthread -lcheck -Wno-nonnull -Wno-memset-transposed-args -lm -lsubunit
	OPEN = google-chrome
endif

ifeq ($(MAC_M1), _ARM64)
	LDFLAGS += -lncurses
	TEST_FLAGS = -I/opt/homebrew/opt/check/include -L/opt/homebrew/opt/check/lib -lcheck
	OPEN = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
endif
ifeq ($(MAC_X86), _X86_64)
	LDFLAGS += -lncurses
	TEST_FLAGS = $(shell pkg-config --libs --cflags check) -lm
	OPEN = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
endif
ifeq ($(DARWIN), Darwin)
	LDFLAGS += -lncurses
	TEST_FLAGS = -I/opt/homebrew/opt/check/include -L/opt/homebrew/opt/check/lib -lcheck
	OPEN = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
endif

# источники и объекты для библиотеки
LIB_SRC = $(LIB_DIR)/tetris.c
LIB_OBJ = $(LIB_SRC:$(LIB_DIR)/%.c=$(OBJ_DIR)/%.o)

# источники и объекты для GUI
GUI_SRC = $(GUI_DIR)/main.c $(GUI_DIR)/draw.c $(GUI_DIR)/audio.c $(GUI_DIR)/input_handler.c $(GUI_DIR)/game_state_handlers.c
GUI_OBJ = $(GUI_SRC:$(GUI_DIR)/%.c=$(OBJ_DIR)/%.o)

# источники и объекты для тестов
TEST_SRC = brick_game/tetris/tests/test_tetris.c
TEST_OBJ = $(TEST_SRC:tests/%.c=$(OBJ_DIR)/%.o) 

# основная цель
TARGET = $(BIN_DIR)/tetris

# правила сборки
all: $(TARGET)

$(TARGET): $(LIB_OBJ) $(GUI_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(LIB_OBJ) $(GUI_OBJ) $(LDFLAGS)

# компиляция источников библиотеки
$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# компиляция источников GUI
$(OBJ_DIR)/%.o: $(GUI_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# компиляция источников тестов
$(OBJ_DIR)/test_tetris.o: $(LIB_DIR)/tests/test_tetris.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(LIB_DIR)/tests/test_tetris.c -o $(OBJ_DIR)/test_tetris.o

# создание библиотеки
$(LIB_DIR)/$(LIB_NAME): $(LIB_OBJ)
	ar rcs $(LIB_DIR)/$(LIB_NAME) $(LIB_OBJ)
	ranlib $(LIB_DIR)/$(LIB_NAME)

# тестирование
test: $(LIB_DIR)/$(LIB_NAME) $(OBJ_DIR)/test_tetris.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_tetris $(OBJ_DIR)/test_tetris.o $(LIB_DIR)/$(LIB_NAME) $(LDFLAGS) $(TEST_FLAGS)

# отчёт о покрытии с использованием gcov и gcovr
gcov_flag:
	$(eval CFLAGS += --coverage)

gcov_report: clean gcov_flag test
	@mkdir -p report
	@$(BIN_DIR)/test_tetris
	@gcovr -r $(shell pwd) --object-directory=$(OBJ_DIR) --html --html-details -o report/coverage.html
	$(OPEN) report/coverage.html

# установка зависимостей
install: install-check install-ncurses install-sdl2 install-sdl2-mixer install-doxygen install-gcov

install-check:
ifeq ($(shell uname), Linux)
	sudo apt-get install check
else ifeq ($(shell uname), Darwin)
	brew install check
endif

install-ncurses:
ifeq ($(shell uname), Linux)
	sudo apt-get install -y libncurses5-dev
else ifeq ($(shell uname), Darwin)
	brew install ncurses
endif

install-sdl2:
ifeq ($(shell uname), Linux)
	sudo apt-get install -y libsdl2-dev
else ifeq ($(shell uname), Darwin)
	brew install sdl2
endif

install-sdl2-mixer:
ifeq ($(shell uname), Linux)
	sudo apt-get install -y libsdl2-mixer-dev
else ifeq ($(shell uname), Darwin)
	brew install sdl2_mixer
endif

install-doxygen:
ifeq ($(shell uname), Linux)
	sudo apt-get install -y doxygen
else ifeq ($(shell uname), Darwin)
	brew install doxygen
endif

install-gcov:
ifeq ($(shell uname), Linux)
	sudo apt-get install -y gcc gcovr
else ifeq ($(shell uname), Darwin)
	brew install gcc gcovr
endif

# удаление установленного
uninstall:
	sudo rm -rf $(TARGET)
# форматирование кода
format:
	clang-format -style=google -n $(GUI_DIR)/*.c $(GUI_DIR)/*.h $(LIB_DIR)/*.c $(LIB_DIR)/*.h $(TEST_DIR)/*.c
	sleep 1
	reset
	clang-format -style=google -i $(GUI_DIR)/*.c $(GUI_DIR)/*.h $(LIB_DIR)/*.c $(LIB_DIR)/*.h $(TEST_DIR)/*.c
	clang-format -style=google -n $(GUI_DIR)/*.c $(GUI_DIR)/*.h $(LIB_DIR)/*.c $(LIB_DIR)/*.h $(TEST_DIR)/*.c

# пересборка
rebuild: clean all

# создание архива
dist:
	@mkdir -p dist
	@tar -czvf dist/$(PROJECT_NAME).tar.gz $(LIB_DIR) $(GUI_DIR) Makefile
	@echo "Архив с исходным кодом создан: dist/$(LIB_NAME).tar.gz"

# создание документации
dvi: $(DOXYFILE)
	@doxygen $(DOXYFILE)
	@echo "Документация создана."
	@$(OPEN) html/index.html

# создание и настройка Doxyfile
$(DOXYFILE):
	@echo "Создание конфигурационного файла Doxygen..."
	@doxygen -g $(DOXYFILE)
	@sed -i.bak 's|^PROJECT_NAME.*$$|PROJECT_NAME = $(notdir $(CURDIR))|' $(DOXYFILE)
	@sed -i.bak 's|^# INPUT.*$$|INPUT = $(LIB_DIR) $(GUI_DIR)|' $(DOXYFILE)
	@sed -i.bak 's|^# RECURSIVE.*$$|RECURSIVE = YES|' $(DOXYFILE)
	@sed -i.bak 's|^# EXTRACT_ALL.*$$|EXTRACT_ALL = YES|' $(DOXYFILE)
	@rm -f $(DOXYFILE).bak
	@echo "Файл Doxyfile создан и настроен."

# проверка на утечки памяти
memory:
ifeq ($(OS), Linux)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind_report.txt ./$(TARGET)
else ifeq ($(DARWIN), Darwin)
	leaks --atExit -- ./$(TARGET)
endif

# флаги для дебаггера и педантик
add_db_flag:
	$(eval CFLAGS += $(DB))

add_p_flag:
	$(eval CFLAGS += $(P))

# Установка Homebrew
brew:
	curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh
	reset
	brew install lcov check

# очистка
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)/*.a *.gcno *.gcda *.gcov *.info report a.out
	rm -rf html doxygen.log $(DOXYFILE) $(DOXYFILE).dox latex dist
