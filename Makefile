CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

NAME = dungeon_cards

HEADERS = inc
SRC = src

# Get absolute path to project root
ROOT_DIR := $(shell pwd)

# OS-specific settings
ifeq ($(UNAME_S),Darwin) # macOS
    FRAMEWORKS = -F./resource/frameworks \
                -framework SDL2 \
                -framework SDL2_ttf \
                -framework SDL2_image \
                -framework SDL2_mixer
    RPATH = -rpath @executable_path/resource/frameworks
else # Linux and others
    FRAMEWORKS = -L$(ROOT_DIR)/resource/frameworks/lib \
                -I$(ROOT_DIR)/resource/frameworks/include \
                -Wl,-rpath,$(ROOT_DIR)/resource/frameworks/lib \
                -lSDL2 \
                -lSDL2_ttf \
                -lSDL2_image \
                -lSDL2_mixer
    EXTRA_LIBS = -lm
endif

all: $(NAME)

$(NAME): $(wildcard $(SRC)/*.c)
	$(CC) $(CFLAGS) -I$(HEADERS) $(FRAMEWORKS) $(wildcard $(SRC)/*.c) $(EXTRA_LIBS) -o $@

clean:
	rm -rf $(SRC)/*.o

uninstall:
	rm -rf $(NAME)

reinstall: uninstall all