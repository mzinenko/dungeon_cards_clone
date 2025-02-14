NAME = endgame

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc

# Framework paths
FRAMEWORK_PATH = resource/framework

# SDL includes using your framework structure
SDL_INCLUDES = -I$(FRAMEWORK_PATH)/SDL2.framework/Headers \
    -I$(FRAMEWORK_PATH)/SDL2_image.framework/Headers \
    -I$(FRAMEWORK_PATH)/SDL2_mixer.framework/Headers \
    -I$(FRAMEWORK_PATH)/SDL2_ttf.framework/Headers

# For Linux linking with your frameworks
SDL_FLAGS = -L$(FRAMEWORK_PATH)/SDL2.framework \
    -L$(FRAMEWORK_PATH)/SDL2_image.framework \
    -L$(FRAMEWORK_PATH)/SDL2_mixer.framework \
    -L$(FRAMEWORK_PATH)/SDL2_ttf.framework \
    -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:%.c=%.o)))
INC_FILES = $(wildcard $(INC_DIR)/*.h)

CC = clang
CFLAGS = -std=c11 $(addprefix -W, all extra error pedantic) -g

MKDIR = mkdir -p
RM = rm -rf

all: $(NAME)

$(NAME): $(OBJ_FILES)
	@$(CC) $(CFLAGS) $^ -o $@ -I $(INC_DIR) $(SDL_FLAGS)
	@printf "\r\33[2K$@\t \033[32;1mcreated\033[0m\n"

$(OBJ_FILES): | $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_FILES)
	@$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) $(SDL_INCLUDES)
	@printf "\r\33[2K$(NAME)\033[33;1m\t compile \033[0m$(<:$(SRC_DIR)/%.c=%)\r"

$(OBJ_DIR):
	@$(MKDIR) $@

clean:
	@$(RM) $(OBJ_DIR)
	@printf "$(OBJ_DIR) in $(NAME)\t \033[31;1mdeleted\033[0m\n"

uninstall: clean
	@$(RM) $(NAME)
	@printf "$(NAME)\t \033[31;1muninstalled\033[0m\n"

reinstall: uninstall all

.PHONY: all uninstall clean reinstall