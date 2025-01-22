# Compiler and Flags
CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c11 -O2

# Targets
NAME = dungeon_cards_clone
LIBRARY = libgrid.a
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj

# Source files separation
GRID_SRC = $(SRC_DIR)/grid.c
MAIN_SRC = $(SRC_DIR)/main.c
PLAYER_SRC = $(SRC_DIR)/player.c
CARD_SRC = $(SRC_DIR)/card.c
GAMESTATE_SRC = $(SRC_DIR)/gamestate.c

# Object files
GRID_OBJ = $(GRID_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
MAIN_OBJ = $(MAIN_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
PLAYER_OBJ = $(PLAYER_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CARD_OBJ = $(CARD_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
GAMESTATE_OBJ = $(GAMESTATE_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Include directories
INCLUDES = -I$(SRC_DIR)

# Default Target
all: $(NAME)

# Build Executable
$(NAME): $(LIB_DIR)/$(LIBRARY) $(MAIN_OBJ) $(PLAYER_OBJ) $(CARD_OBJ) $(GAMESTATE_OBJ)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) $(PLAYER_OBJ) $(CARD_OBJ) $(GAMESTATE_OBJ) -L$(LIB_DIR) -lgrid `sdl2-config --libs` -lSDL2_ttf

# Build Static Library
$(LIB_DIR)/$(LIBRARY): $(GRID_OBJ)
	mkdir -p $(LIB_DIR)
	ar rcs $@ $^

# Object Files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean Object Files
clean:
	rm -rf $(OBJ_DIR)

# Clean All Build Files
uninstall: clean
	rm -rf $(NAME) $(LIB_DIR)

# Rebuild Project
reinstall: uninstall all

.PHONY: all clean uninstall reinstall