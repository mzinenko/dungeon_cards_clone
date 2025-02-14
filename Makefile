CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

NAME = dungeon_cards

HEADERS = inc
SRC = src

# Detect OS
UNAME_S := $(shell uname -s)

# OS-specific settings
ifeq ($(UNAME_S),Darwin) # macOS
    FRAMEWORKS = -F./resource/frameworks \
                -framework SDL2 \
                -framework SDL2_ttf \
                -framework SDL2_image \
                -framework SDL2_mixer
    RPATH = -rpath @executable_path/resource/frameworks
else # Linux and others
    FRAMEWORKS = -L./resource/frameworks \
                -lSDL2 \
                -lSDL2_ttf \
                -lSDL2_image \
                -lSDL2_mixer \
                -lm
    RPATH = -Wl,-rpath,'$$ORIGIN/resource/frameworks'
endif

all: $(NAME)

$(NAME): $(wildcard $(SRC)/*.c)
	$(CC) $(CFLAGS) -I$(HEADERS) $(wildcard $(SRC)/*.c) $(FRAMEWORKS) $(RPATH) -o $@

clean:
	rm -rf $(SRC)/*.o

uninstall:
	rm -rf $(NAME)

reinstall: uninstall all