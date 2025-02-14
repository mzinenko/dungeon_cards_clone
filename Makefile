NAME = dungeon_cards

CC = clang
CFLAGS = -Wall -Wextra -Werror -I./inc -I./resource/framework/include
LDFLAGS = -L./resource/framework/lib

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc
FRAMEWORK_DIR = resource/framework

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# SDL2 static libraries
SDL_LIBS = $(FRAMEWORK_DIR)/lib/libSDL2.a \
           $(FRAMEWORK_DIR)/lib/libSDL2_image.a \
           $(FRAMEWORK_DIR)/lib/libSDL2_ttf.a \
           $(FRAMEWORK_DIR)/lib/libSDL2_mixer.a

# Linux dependencies
LINUX_DEPS = -lm -ldl -lpthread -lrt

# Check OS and set appropriate flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS specific flags
    PLATFORM_FLAGS = -framework CoreVideo -framework CoreAudio \
                    -framework AudioToolbox -framework CoreFoundation \
                    -framework Carbon -framework ForceFeedback \
                    -framework IOKit -framework Cocoa -framework Metal \
                    -framework GameController
else
    # Linux specific flags
    PLATFORM_FLAGS = $(LINUX_DEPS)
endif

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	$(CC) $(OBJS) $(SDL_LIBS) $(PLATFORM_FLAGS) -o $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

uninstall:
	rm -rf $(OBJ_DIR)
	rm -f $(NAME)

reinstall: uninstall all

.PHONY: all clean uninstall reinstall deps