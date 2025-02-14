CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic
NAME = dungeon_cards
HEADERS = inc
SRC = src

# SDL Configuration
SDL_FRAMEWORKS = -F./resource/frameworks
SDL_RPATH = -Wl,-rpath,@executable_path/resource/frameworks
FRAMEWORKS = $(SDL_FRAMEWORKS) -framework SDL2
EXTRA_LIBS = $(SDL_RPATH)

# Get absolute path to handle relative paths correctly
PWD = $(shell pwd)

.PHONY: all clean uninstall reinstall

all: $(NAME)

$(NAME): $(wildcard $(SRC)/*.c)
	$(CC) $(CFLAGS) -I$(HEADERS) $(FRAMEWORKS) $(wildcard $(SRC)/*.c) $(EXTRA_LIBS) -o $@

clean:
	rm -rf $(SRC)/*.o

uninstall:
	rm -rf $(NAME)

reinstall: uninstall all