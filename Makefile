CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

NAME = dungeon_cards

HEADERS = inc
SRC = src

FRAMEWORKS = -F./resource/frameworks \
            -framework SDL2 \
            -framework SDL2_ttf \
            -framework SDL2_image \
            -framework SDL2_mixer

all: $(NAME)

$(NAME): $(wildcard $(SRC)/*.c)
	$(CC) $(CFLAGS) -I$(HEADERS) $^ -o $@ $(FRAMEWORKS) \
	-rpath @executable_path/resource/frameworks

clean:
	rm -rf $(SRC)/*.o

uninstall:
	rm -rf $(NAME)

reinstall: uninstall all