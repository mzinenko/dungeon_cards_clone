#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#define WINDOW_WIDTH 1720
#define WINDOW_HEIGHT 1080
#define GRID_SIZE 750   // 5x5 grid, each cell 150x150
#define CARD_SIZE 150   // Each card is 150x150 pixels
#define FRAME_DELAY 150 // Milliseconds per frame
#define MAX_ANIMATIONS 128

#define BUTTON_WIDTH 180
#define BUTTON_HEIGHT 180
#define BUTTON_SPACING 20

#define SAVE_DIR "resource/saves"
#define MAX_SAVE_FILES 10
#define SAVE_FILE_MAGIC 0x44434C47 // "DCLG" in hex
#define SAVE_VERSION 3

// #define M_PI 3.14159265358979323846
// #define DT_REG 8

#define MAX_PATH_LENGTH 1024
#define MAX_FILES_PER_PAGE 10
#define MAX_FILE_ENTRIES 256

#define MAX_FACTION_UPGRADES 10
#define MAX_FACTION_QUESTS 15
#define MAX_ACTIVE_QUESTS 2

#define VIRTUAL_WIDTH 854
#define VIRTUAL_HEIGHT 480
#define VIRTUAL_SCALE 1  // Base scale for pixel art

#define MENU_BUTTON_WIDTH 90   // Original button sprite is 248x248, scaled down for virtual resolution
#define MENU_BUTTON_HEIGHT 90
#define MENU_BUTTON_SPACING 10

#define SAVE_BUTTON_HEIGHT 24
#define SAVE_BUTTON_SPACING 4
#define SAVE_LIST_PADDING 6
#define ACTION_BUTTON_WIDTH 100
#define ACTION_BUTTON_HEIGHT 24

#define BROWSER_WIDTH (VIRTUAL_WIDTH * 0.8f)  // 70% of virtual width
#define BROWSER_HEIGHT (VIRTUAL_HEIGHT * 0.9f)  // 75% of virtual height
#define BROWSER_PADDING 4
#define BROWSER_BUTTON_HEIGHT 24
#define BROWSER_BUTTON_WIDTH 48
#define FILE_ENTRY_HEIGHT 24
#define FILE_ENTRY_SPACING 4

#define HUB_BANNER_WIDTH 200
#define HUB_BANNER_HEIGHT 180
#define HUB_BANNER_SPACING 20
#define HUB_BUTTON_WIDTH 60
#define HUB_BUTTON_HEIGHT 20
#define HUB_PANEL_PADDING 8
#define HUB_STATS_HEIGHT 120
#define HUB_PLAYER_PHOTO_SIZE 100

#define FACTION_PADDING 8
#define FACTION_BUTTON_WIDTH 60
#define FACTION_BUTTON_HEIGHT 20
#define FACTION_TAB_WIDTH 80
#define FACTION_TAB_HEIGHT 24
#define FACTION_DIALOG_HEIGHT 100
#define FACTION_ITEM_HEIGHT 80
#define FACTION_ITEM_SPACING 4

// Existing enums
typedef enum
{
    MOVE_NONE,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT
} MoveDirection;

typedef enum
{
    ARMOR_PHYSICAL,
    ARMOR_RANGED,
    ARMOR_NEUTRAL
} ArmorType;

typedef enum
{
    CARD_NONE,
    CARD_ENEMY,
    CARD_WEAPON,
    CARD_SHIELD,
    CARD_POTION,
    CARD_COIN,
} CardType;

typedef enum
{
    RARITY_COMMON,
    RARITY_UNCOMMON,
    RARITY_RARE,
    RARITY_EPIC,
    RARITY_LEGENDARY,
} CardRarity;

/*

type -> rarity -> []

CARD_ENEMY -> RARITY_COMMON -> [
{
texture: ....,
attack: {
}
},
]

*/

typedef enum
{
    STATE_MAIN_MENU,
    STATE_SAVE_SELECT, // New state for save file selection
    STATE_HUB,         // New state for the hub interface
    STATE_FRACTION,    // New state for fraction interface
    STATE_GAMEPLAY,
    STATE_GAME_OVER,
} GameState;

typedef enum
{
    BUTTON_STATE_NORMAL,
    BUTTON_STATE_HOVER,
    BUTTON_STATE_CLICKED
} ButtonState;

// Rarity chance probabilities
static float RARITY_CHANCES[] = {
    0.50f, // RARITY_COMMON
    0.25f, // RARITY_UNCOMMON
    0.15f, // RARITY_RARE
    0.08f, // RARITY_EPIC
    0.02f  // RARITY_LEGENDARY
};

// Rarity range multipliers
static struct
{
    float damageMin;
    float damageMax;
    float hpMin;
    float hpMax;
    float armorMin;
    float armorMax;
} RARITY_RANGES[] = {
    {1, 3, 3, 5, 0, 1},   // RARITY_COMMON
    {2, 5, 4, 7, 1, 2},   // RARITY_UNCOMMON
    {3, 7, 6, 10, 2, 3},  // RARITY_RARE
    {4, 10, 8, 15, 3, 5}, // RARITY_EPIC
    {6, 15, 12, 20, 4, 7} // RARITY_LEGENDARY
};

typedef enum
{
    FACTION_NONE,
    FACTION_VANGUARD,
    FACTION_CRIMSON_PATH
} FactionType;

typedef enum
{
    DIALOG_FIRST_ENCOUNTER,
    DIALOG_GREETING,
    DIALOG_SHOP,
    DIALOG_QUEST
} DialogState;

typedef enum
{
    MUTATION_NONE,
    MUTATION_BEAST,
    MUTATION_VENOM,
    MUTATION_BLOOD
} MutationType;
