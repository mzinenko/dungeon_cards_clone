CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c11 -O2

NAME = dungeon_cards_clone
LIBRARY = libgrid.a
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Separate grid.c object for library
GRID_OBJ = $(OBJ_DIR)/grid.o
MAIN_OBJS = $(filter-out $(GRID_OBJ),$(OBJS))

INCLUDES = -I$(SRC_DIR)

all: $(NAME)

$(NAME): $(LIB_DIR)/$(LIBRARY) $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJS) -L$(LIB_DIR) -lm -lgrid `sdl2-config --libs` -lSDL2_ttf -lSDL2_image

$(LIB_DIR)/$(LIBRARY): $(GRID_OBJ)
	mkdir -p $(LIB_DIR)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

uninstall: clean
	rm -f $(NAME)
	rm -rf $(LIB_DIR)

reinstall: uninstall all

.PHONY: all clean uninstall reinstall